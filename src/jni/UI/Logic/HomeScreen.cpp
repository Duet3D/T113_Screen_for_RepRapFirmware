/*
 * HomeScreen.cpp
 *
 *  Created on: 24 April 2024
 *      Author: Andy Everitt
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "HomeScreen.h"

#include "ObjectModel/Heightmap.h"
#include "ObjectModel/Sensor.h"
#include "UI/Graph.h"
#include "UI/Logic/Heightmap.h"
#include "UI/Logic/Webcam.h"
#include <entry/EasyUIContext.h>
#include <std_fixed/functional.h>

namespace UI::HomeScreen
{
	static Graph s_temperatureGraph;
	static ToolsList s_toolsList;

	static WindowSelectItem s_windows[] = {
		{"move", []() { UI::WINDOW.OpenWindow(ID_MAIN_MoveWindow); }},
		{"extrude", []() { UI::WINDOW.OpenWindow(ID_MAIN_ExtrudeWindow); }},
		{"status", []() { UI::WINDOW.OpenWindow(ID_MAIN_PrintWindow); }},
		{"heightmap",
		 []() {
			 OM::RequestHeightmapFiles();
			 UI::WINDOW.OpenWindow(ID_MAIN_HeightMapWindow);
			 UI::Heightmap::RenderHeightmap(OM::GetCurrentHeightmap());
		 }},
		{"fans", []() { UI::WINDOW.OpenWindow(ID_MAIN_FanWindow); }},
		{"files",
		 []() {
			 OM::FileSystem::RequestFiles("0:/gcodes");
			 UI::WINDOW.OpenWindow(ID_MAIN_FilesWindow);
		 }},
		{"object_cancel", []() { UI::WINDOW.OpenWindow(ID_MAIN_ObjectCancelWindow); }},
		{"webcam",
		 []() {
			 UI::Webcam::RegisterUpdateLoop();
			 UI::WINDOW.OpenWindow(ID_MAIN_WebcamWindow);
		 }},
		{"network", []() { EASYUICONTEXT->openActivity("WifiSettingActivity"); }},
		{"settings", []() { UI::WINDOW.OpenWindow(ID_MAIN_SettingsWindow); }},
	};

	void Init()
	{
		info("Initialising HomeScreen UI...");
		s_toolsList.Init(UI::GetUIControl<ZKListView>(ID_MAIN_ToolListView));
		s_temperatureGraph.Init(UI::GetUIControl<ZKDiagram>(ID_MAIN_TempGraph),
								UI::GetUIControl<ZKListView>(ID_MAIN_TempGraphXLabels),
								UI::GetUIControl<ZKListView>(ID_MAIN_TempGraphYLabels),
								UI::GetUIControl<ZKListView>(ID_MAIN_TemperatureGraphLegend));
	}

	size_t GetWindowSelectCount()
	{
		return ARRAY_SIZE(s_windows);
	}

	void SetWindowSelectListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		if (index < 0 || index >= (int)ARRAY_SIZE(s_windows))
		{
			warn("Invalid window index %d", index);
			return;
		}

		pListItem->setTextTr(s_windows[index].id);
	}

	void WindowSelectListItemCallback(const int index)
	{
		dbg("index %d", index);
		if (index < 0 || index >= (int)ARRAY_SIZE(s_windows))
		{
			warn("Invalid window index %d", index);
			return;
		}

		s_windows[index].callback();
	}

	size_t GetToolsListCount()
	{
		size_t count = s_toolsList.GetTotalHeaterCount(false);
		dbg("GetToolsListCount: %d", count);
		return count;
	}

	void SetToolsListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		s_toolsList.ObtainListItemData(pListItem,
									   index,
									   ID_MAIN_ToolNameSubItem,
									   ID_MAIN_ToolStatusSubItem,
									   ID_MAIN_ToolCurrentTemperatureSubItem,
									   ID_MAIN_ToolActiveTemperatureSubItem,
									   ID_MAIN_ToolStandbyTemperatureSubItem);
	}

	void ToolsListItemCallback(const int index, const int id)
	{
		s_toolsList.OnListItemClick(index,
									id,
									ID_MAIN_ToolNameSubItem,
									ID_MAIN_ToolStatusSubItem,
									ID_MAIN_ToolActiveTemperatureSubItem,
									ID_MAIN_ToolStandbyTemperatureSubItem);
	}

	size_t GetTemperatureGraphLegendCount()
	{
		return OM::GetAnalogSensorCount();
	}

	void SetTemperatureGraphLegendItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		OM::AnalogSensor* sensor = OM::GetAnalogSensorBySlot(index);
		if (sensor == nullptr)
		{
			pListItem->setText("");
			return;
		}
		pListItem->setText(sensor->name.c_str());
		pListItem->setSelected(!s_temperatureGraph.IsWaveVisible(index));
	}

	void TemperatureGraphLegendItemCallback(const int index)
	{
		s_temperatureGraph.SetWaveVisible(index, !s_temperatureGraph.IsWaveVisible(index));
	}

	void SetTemperatureGraphXLabel(ZKListView* pListView, ZKListView::ZKListItem* pListItem, const int index)
	{
		int range = s_temperatureGraph.GetTimeRange();
		int time = -range + (index * range / (pListView->getCols() - 1));
		pListItem->setTextf("%ds", time);
	}

	void SetTemperatureGraphYLabel(ZKListView* pListView, ZKListView::ZKListItem* pListItem, const int index)
	{
		float yMax = s_temperatureGraph.GetYMax();
		float label = yMax - (yMax / (pListView->getRows() - 1)) * index;
		pListItem->setTextf("%.1f", label);
	}

	void RefreshTemperatureGraph()
	{
		s_temperatureGraph.RefreshLegend();
	}

	void UpdateTemperatureGraph()
	{
		for (size_t i = 0; i < s_temperatureGraph.GetWaveCount(); i++)
		{
			OM::AnalogSensor* sensor = OM::GetAnalogSensor(i, true);
			if (sensor == nullptr || sensor->name.IsEmpty())
			{
				verbose("Sensor incomplete or does not exist");
				s_temperatureGraph.Clear(i);
				continue;
			}

			float value = sensor->lastReading;
#if 0
			long long now = TimeHelper::getCurrentTime();
			if (now - sensor->lastReadingTime > 3 * Comm::DUET.GetPollInterval() + PRINTER_REQUEST_TIMEOUT)
			{
				dbg("Sensor %d has timed out", i);
				value = -273;
			}
#endif
			s_temperatureGraph.AddData(i, value);
		}
	}

	void ClearTemperatureGraph()
	{
		s_temperatureGraph.ClearAll();
	}
} // namespace UI::HomeScreen
