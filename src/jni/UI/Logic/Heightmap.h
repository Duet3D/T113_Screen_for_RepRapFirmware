/*
 * Heightmap.h
 *
 *  Created on: 15 Apr 2024
 *      Author: andy
 */

#ifndef JNI_UI_LOGIC_HEIGHTMAP_H_
#define JNI_UI_LOGIC_HEIGHTMAP_H_

#include "ObjectModel/Heightmap.h"

namespace UI::Heightmap
{
	enum class HeightmapRenderMode
	{
		Fixed = 0,
		Deviation,
	};

	void Init();

	void Open();

	const std::string& GetVisibleHeightmapName();

	size_t GetHeightmapCount();
	void SetHeightmapListItem(ZKListView::ZKListItem* pListItem, const int index);
	void HeightmapListItemCallback(const int index, const int id);

	void Refresh();

	void SetHeightmapRenderMode(HeightmapRenderMode mode);
	HeightmapRenderMode GetHeightmapRenderMode();
	void SetHeightmapRenderModeListItem(ZKListView::ZKListItem* pListItem, const int index);

	void SetHeightmapXAxisText(ZKListView* pListView, ZKListView::ZKListItem* pListItem, const int index);
	void SetHeightmapYAxisText(ZKListView* pListView, ZKListView::ZKListItem* pListItem, const int index);
	void SetHeightmapScaleAt(ZKListView* pListView, ZKListView::ZKListItem* pListItem, const int index);

	void RenderScale();
	bool RenderHeightmap(const std::string& heightmapName);
	void ClearHeightmap();
} // namespace UI

#endif /* JNI_UI_LOGIC_HEIGHTMAP_H_ */
