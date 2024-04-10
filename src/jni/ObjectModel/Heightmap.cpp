/*
 * Heightmap.cpp
 *
 *  Created on: 26 Mar 2024
 *      Author: andy
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "Hardware/Duet.h"
#include "Heightmap.h"
#include "rapidcsv/rapidcsv.h"
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
		m_axis0 = "X";
		m_axis1 = "Y";
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

		std::string axis;
		if (!doc.GetCell("axis0", 0, m_axis0))
		{
			m_axis0 = "X";
		}

		if (!doc.GetCell("axis1", 0, m_axis1))
		{
			m_axis1 = "Y";
		}

		doc.GetCell("min0", 0, m_min[0]);
		doc.GetCell("min1", 0, m_min[1]);
		doc.GetCell("max0", 0, m_max[0]);
		doc.GetCell("max1", 0, m_max[1]);

		doc.GetCell("radius", 0, m_radius);

		doc.GetCell("spacing0", 0, m_spacing[0]);
		doc.GetCell("spacing1", 0, m_spacing[1]);

		doc.GetCell("num0", 0, m_samples[0]);
		doc.GetCell("num1", 0, m_samples[1]);

		dbg("Axes: %s, %s", m_axis0.c_str(), m_axis1.c_str());
		dbg("Min: %f, %f", m_min[0], m_min[1]);
		dbg("Max: %f, %f", m_max[0], m_max[1]);
		dbg("Radius: %f", m_radius);
		dbg("Spacing: %f, %f", m_spacing[0], m_spacing[1]);
		dbg("Samples: %u, %u", m_samples[0], m_samples[1]);
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
		dbg("Meta: %s", metaStr.c_str());

		meta.Parse(metaStr);
	}

	void Heightmap::ParseData(const std::string& csvContents)
	{
		info("Parsing data for heightmap %s", m_fileName.c_str());
	}
} // namespace OM
