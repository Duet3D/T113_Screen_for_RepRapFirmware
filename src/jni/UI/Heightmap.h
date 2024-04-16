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
		Fixed,
		Deviation,
	};

	void SetHeightmapRenderMode(HeightmapRenderMode mode);
	HeightmapRenderMode GetHeightmapRenderMode();
	double GetHeightmapScaleAt(int index);
	bool RenderHeightmap(const OM::Heightmap& heightmap);
} // namespace UI

#endif /* JNI_UI_HEIGHTMAP_H_ */
