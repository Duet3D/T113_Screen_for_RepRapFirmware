/*
 * Heightmap.cpp
 *
 *  Created on: 26 Mar 2024
 *      Author: andy
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "Heightmap.h"

#include "Hardware/Duet.h"
#include "utils/csv.h"
#include <sstream>

namespace OM
{
	static std::string GetLocalFilePath(const char* filename)
	{
		return utils::format("/tmp/heightmaps/%s", filename);
	}

	HeightmapMeta::HeightmapMeta()
	{
		Reset();
	}

	HeightmapMeta::~HeightmapMeta() {}

	void HeightmapMeta::Reset()
	{
		m_axis[0] = "X";
		m_axis[1] = "Y";
		m_min[0] = 0.0f;
		m_min[1] = 0.0f;
		m_max[0] = 0.0f;
		m_max[1] = 0.0f;
		m_radius = 0.0f;
		m_spacing[0] = 0.0f;
		m_spacing[1] = 0.0f;
		m_samples[0] = 0;
		m_samples[1] = 0;
	}

	void HeightmapMeta::Parse(const std::string& meta)
	{
		utils::CSV doc(meta, true);
		const std::vector<std::string> headers = doc.GetHeaders();
		for (const std::string& header : headers)
		{
			dbg("Header: \"%s\"", header.c_str());
		}

		doc.GetCell("axis0", 0, m_axis[0]);
		doc.GetCell("axis1", 0, m_axis[1]);

		doc.GetCell("min0", 0, m_min[0]);
		doc.GetCell("min1", 0, m_min[1]);
		doc.GetCell("max0", 0, m_max[0]);
		doc.GetCell("max1", 0, m_max[1]);

		doc.GetCell("radius", 0, m_radius);

		doc.GetCell("spacing0", 0, m_spacing[0]);
		doc.GetCell("spacing1", 0, m_spacing[1]);

		doc.GetCell("num0", 0, m_samples[0]);
		doc.GetCell("num1", 0, m_samples[1]);

		dbg("Axes: %s, %s", m_axis[0].c_str(), m_axis[1].c_str());
		dbg("Min: %f, %f", m_min[0], m_min[1]);
		dbg("Max: %f, %f", m_max[0], m_max[1]);
		dbg("Radius: %f", m_radius);
		dbg("Spacing: %f, %f", m_spacing[0], m_spacing[1]);
		dbg("Samples: %u, %u", m_samples[0], m_samples[1]);
	}

	Move::Axis* HeightmapMeta::GetAxis(size_t index) const
	{
		const std::string& label = m_axis[index];
		Move::Axis* axis = Move::GetAxisByLetter(label[0]);
		if (axis == nullptr)
		{
			error("Axis %s not found", label.c_str());
		}
		return axis;
	}

	Heightmap::Heightmap(const char* filename)
	{
		m_fileName = filename;
		std::string csvContents;
		if (!Comm::duet.DownloadFile(utils::format("/sys/%s", filename).c_str(), csvContents))
		{
			error("Failed to download heightmap file %s", filename);
			return;
		}

		if (csvContents.find("RepRapFirmware height map") == std::string::npos)
		{
			warn("CSV file \"%s\" not a heightmap", filename);
			return;
		}

		// Write the file to disk
		std::string localFilePath = GetLocalFilePath(filename);
		std::ofstream file(localFilePath.c_str(), std::ios::out | std::ios::binary);
		if (!file.is_open())
		{
			error("Failed to open file %s for writing", localFilePath.c_str());
			return;
		}

		info("Writing heightmap to %s", localFilePath.c_str());
		file.write(csvContents.c_str(), csvContents.length());

		ParseMeta(csvContents);
		ParseData(csvContents);
	}

	void Heightmap::ParseMeta(const std::string& csvContents)
	{
		info("Parsing meta data for heightmap %s", m_fileName.c_str());
		size_t metaStart = utils::findInstance(csvContents, "\n", 1);
		size_t metaEnd = utils::findInstance(csvContents, "\n", 3);
		if (metaStart == std::string::npos || metaEnd == std::string::npos)
		{
			error("Corrupt heightmap file %s", m_fileName.c_str());
			return;
		}

		std::string metaStr = csvContents.substr(metaStart, metaEnd - metaStart);
		dbg("Meta:\n%s", metaStr.c_str());

		meta.Parse(metaStr);
	}

	void Heightmap::ParseData(const std::string& csvContents)
	{
		info("Parsing data for heightmap %s", m_fileName.c_str());
		size_t dataStart = utils::findInstance(csvContents, "\n", 3) + 1;
		if (dataStart == std::string::npos)
		{
			error("Corrupt heightmap file %s", m_fileName.c_str());
			return;
		}

		std::string dataStr = csvContents.substr(dataStart);
		dbg("Data:\n%s", dataStr.c_str());
		utils::CSV doc(dataStr, false);

		m_heightmap.clear();
		m_minError = 9999.9f;
		m_maxError = -9999.9f;

		size_t rows = doc.GetRowCount();
		size_t cols = doc.GetColumnCount();

		double errorSum = 0.0f;

		for (size_t rowIdx = 0; rowIdx < rows; rowIdx++)
		{
			std::vector<Point> row(cols);
			for (size_t colIdx = 0; colIdx < cols; colIdx++)
			{
				Point val;
				val.x = meta.GetMin(0) + colIdx * meta.GetSpacing(0);
				val.y = meta.GetMin(1) + rowIdx * meta.GetSpacing(1);
				if (!doc.GetCell(colIdx, rowIdx, val.z))
				{
					error("Failed to get cell %u, %u", colIdx, rowIdx);
					val.isNull = true;
				}
				dbg("Cell %u, %u%s: (%.3f, %.3f, %.3f)",
					colIdx,
					rowIdx,
					val.isNull ? "[INVALID]" : "",
					val.x,
					val.y,
					val.z);
				if (val.z < m_minError)
				{
					m_minError = val.z;
				}
				if (val.z > m_maxError)
				{
					m_maxError = val.z;
				}
				errorSum += val.z;
				row.push_back(val);
			}
			m_heightmap.push_back(row);
		}
		m_meanError = errorSum / (rows * cols);

		dbg("Heightmap: %u rows, %u cols, minError=%.3f, maxError=%.3f, meanError=%.3f",
			rows,
			cols,
			m_minError,
			m_maxError,
			m_meanError);
	}

	void RequestHeightMapFiles()
	{
		FileSystem::RequestFiles("/sys");
	}

	std::vector<FileSystem::FileSystemItem*> GetHeightMapFiles()
	{
		std::vector<FileSystem::FileSystemItem*> files = FileSystem::GetItems();
		std::vector<FileSystem::FileSystemItem*> csvFiles;

		for (FileSystem::FileSystemItem* item : files)
		{
			if (item->GetType() != FileSystem::FileSystemItemType::file)
				continue;

			// Ignore non CSV files
			if (item->GetName().find(".csv\0") == std::string::npos)
				continue;

			if (item->GetName().find(DEFAULT_FILAMENTS_FILE) != std::string::npos)
				continue;

			csvFiles.push_back(item);
		}

		return csvFiles;
	}
} // namespace OM
