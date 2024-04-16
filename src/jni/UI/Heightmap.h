/*
 * Heightmap.h
 *
 *  Created on: 15 Apr 2024
 *      Author: andy
 */

#ifndef JNI_UI_HEIGHTMAP_H_
#define JNI_UI_HEIGHTMAP_H_

#include "ObjectModel/Heightmap.h"

namespace UI
{
	enum class HeightmapRenderMode
	{
		Fixed = 0,
		Deviation,
	};

	void SetHeightmapRenderMode(HeightmapRenderMode mode);
	HeightmapRenderMode GetHeightmapRenderMode();
	std::string GetHeightmapRenderModeText(HeightmapRenderMode mode);
	double GetHeightmapScaleAt(int index);
	bool RenderHeightmap(const std::string& heightmapName);
} // namespace UI

#endif /* JNI_UI_HEIGHTMAP_H_ */
