/*
 * Heightmap.cpp
 *
 *  Created on: 26 Mar 2024
 *      Author: andy
 */

#include "Heightmap.h"
#include "Hardware/Duet.h"
#include "rapidcsv/rapidcsv.h"

namespace OM
{
	static std::string GetLocalFilePath(const char* filename)
	{
		return utils::format("/tmp/heightmaps/%s", filename);
	}

	HeightmapMeta::HeightmapMeta() {}

	HeightmapMeta::~HeightmapMeta() {}

	Heightmap::Heightmap(const char* filename)
	{
		m_fileName = filename;
		std::string csvContents;
		if (!Comm::duet.DownloadFile(filename, csvContents))
		{
			error("Failed to download heightmap file %s", filename);
			return;
		}

		rapidcsv::Document doc(filename,
							   rapidcsv::LabelParams(0, -1),
							   rapidcsv::SeparatorParams(',', true),
							   rapidcsv::ConverterParams(true, 0.0f, 0LL));
	}
} // namespace OM
