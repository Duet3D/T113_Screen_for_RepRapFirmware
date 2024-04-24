/*
 * HomeScreen.h
 *
 *  Created on: 24 April 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_UI_LOGIC_HOMESCREEN_H_
#define JNI_UI_LOGIC_HOMESCREEN_H_

#include "UI/UserInterface.h"

namespace UI::HomeScreen
{
	void Init();

	/* Tools List */
	size_t GetToolsListCount();
	void SetToolsListItem(ZKListView::ZKListItem* pListItem, const int index);
	void ToolsListItemCallback(const int index, const int id);

	/* Temperature Graph */
	size_t GetTemperatureGraphLegendCount();
	void SetTemperatureGraphLegendItem(ZKListView::ZKListItem* pListItem, const int index);
	void TemperatureGraphLegendItemCallback(const int index);
	void SetTemperatureGraphXLabel(ZKListView* pListView, ZKListView::ZKListItem* pListItem, const int index);
	void SetTemperatureGraphYLabel(ZKListView* pListView, ZKListView::ZKListItem* pListItem, const int index);
	void RefreshTemperatureGraph();
	void UpdateTemperatureGraph();
	void ClearTemperatureGraph();

	/* Slide Window */
	void SlideWindowCallback(const int index);
} // namespace UI::HomeScreen

#endif /* JNI_UI_LOGIC_HOMESCREEN_H_ */