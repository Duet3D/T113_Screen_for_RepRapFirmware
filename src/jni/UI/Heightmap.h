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

	const std::string& GetVisibleHeightmapName();
	void SetHeightmapRenderMode(HeightmapRenderMode mode);
	HeightmapRenderMode GetHeightmapRenderMode();
	std::string GetHeightmapRenderModeText(HeightmapRenderMode mode);
	std::string GetHeightmapXAxisText(int index);
	std::string GetHeightmapYAxisText(int index);
	std::string GetHeightmapScaleAt(int index);
	bool RenderHeightmap(const std::string& heightmapName);
	void ClearHeightmap();
} // namespace UI

#endif /* JNI_UI_HEIGHTMAP_H_ */
