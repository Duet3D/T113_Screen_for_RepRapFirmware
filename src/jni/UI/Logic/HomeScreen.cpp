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

namespace UI::HomeScreen
{
	static Graph s_temperatureGraph;
	static ToolsList s_toolsList;

	void Init()
	{
		s_toolsList.Init(UI::GetUIControl<ZKListView>(ID_MAIN_ToolListView));
		s_temperatureGraph.Init(UI::GetUIControl<ZKDiagram>(ID_MAIN_TempGraph),
								UI::GetUIControl<ZKListView>(ID_MAIN_TempGraphXLabels),
								UI::GetUIControl<ZKListView>(ID_MAIN_TempGraphYLabels),
								UI::GetUIControl<ZKListView>(ID_MAIN_TemperatureGraphLegend));
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

	void SlideWindowCallback(const int index)
	{
		dbg(" onSlideItemClick_ SlideWindow1 %d !!!\n", index);
		switch (index)
		{
		case (int)UI::SlideWindowIndex::move:
			UI::WINDOW.OpenWindow(ID_MAIN_MoveWindow);
			break;
		case (int)UI::SlideWindowIndex::extrude:
			UI::WINDOW.OpenWindow(ID_MAIN_ExtrudeWindow);
			break;
		case (int)UI::SlideWindowIndex::status:
			UI::WINDOW.OpenWindow(ID_MAIN_PrintWindow);
			break;
		case (int)UI::SlideWindowIndex::heightmap: {
			OM::RequestHeightmapFiles();
			UI::WINDOW.OpenWindow(ID_MAIN_HeightMapWindow);
			UI::RenderHeightmap(OM::GetCurrentHeightmap());
			break;
		}
		case (int)UI::SlideWindowIndex::fans:
			UI::WINDOW.OpenWindow(ID_MAIN_FanWindow);
			break;
		case (int)UI::SlideWindowIndex::print:
			OM::FileSystem::RequestFiles("0:/gcodes");
			UI::WINDOW.OpenWindow(ID_MAIN_FilesWindow);
			break;
		case (int)UI::SlideWindowIndex::network:
			// UI::WINDOW.OpenWindow(mNetworkWindowPtr);
			EASYUICONTEXT->openActivity("WifiSettingActivity");
			break;
		case (int)UI::SlideWindowIndex::settings:
			UI::WINDOW.OpenWindow(ID_MAIN_SettingsWindow);
			break;
		case (int)UI::SlideWindowIndex::object_cancel:
			UI::WINDOW.OpenWindow(ID_MAIN_ObjectCancelWindow);
			break;
		case (int)UI::SlideWindowIndex::webcam:
			UI::Webcam::RegisterUpdateLoop();
			UI::WINDOW.OpenWindow(ID_MAIN_WebcamWindow);
			break;
		default:
			break;
		}
	}

} // namespace UI::HomeScreen
