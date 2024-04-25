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

	/* Slide Window */
	size_t GetWindowSelectCount();
	void SetWindowSelectListItem(ZKListView::ZKListItem* pListItem, const int index);
	void WindowSelectListItemCallback(const int index);

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

} // namespace UI::HomeScreen

#endif /* JNI_UI_LOGIC_HOMESCREEN_H_ */