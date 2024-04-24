/*
 * ExtrusionControl.cpp
 *
 *  Created on: 24 April 2024
 *      Author: Andy Everitt
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "ExtrusionControl.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Tool.h"

namespace UI::ExtrusionControl
{
	static uint32_t s_extrusionFeedRates[] = {50, 10, 5, 2, 1};
	static uint32_t s_extrusionFeedDistances[] = {100, 50, 20, 10, 5, 2, 1};
	static uint32_t s_defaultExtrusionFeedRate = 5;
	static uint32_t s_defaultExtrusionFeedDistance = 10;

	static OM::Tool* s_filamentDialogTool = nullptr;
	static ToolsList s_toolsList;

	void Init()
	{
		dbg("ExtrusionControl: Init");

		ZKListView* toolList = UI::GetUIControl<ZKListView>(ID_MAIN_ExtrudeToolList);
		if (toolList == nullptr)
		{
			error("ExtrusionControl: Failed to get tool list");
			return;
		}

		s_toolsList.Init(toolList);
	}

	size_t GetListCount()
	{
		return s_toolsList.GetTotalHeaterCount(true, true, false, false);
	}

	void SetExtrudeListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		ZKListView::ZKListSubItem* pfilament = pListItem->findSubItemByID(ID_MAIN_ExtrudeToolFilamentSubItem);

		s_toolsList.ObtainListItemData(pListItem,
									   index,
									   ID_MAIN_ExtrudeToolNameSubItem,
									   ID_MAIN_ExtrudeToolStatusSubItem,
									   ID_MAIN_ExtrudeToolCurrentTemperatureSubItem,
									   ID_MAIN_ExtrudeToolActiveTemperatureSubItem,
									   ID_MAIN_ExtrudeToolStandbyTemperatureSubItem);

		UI::ToolsList::ToolListItemData data = s_toolsList.GetToolListItemDataBySlot(index);
		if (data.tool == nullptr)
		{
			return;
		}

		StringRef filament = data.tool->GetFilament();
		pfilament->setText((data.tool->filamentExtruder >= 0 && filament.IsEmpty())
							   ? LANGUAGEMANAGER->getValue("load_filament")
							   : data.tool->GetFilament().c_str());
	}

	void ExtrudeListItemCallback(const int index, const int id)
	{
		s_toolsList.OnListItemClick(index,
									id,
									ID_MAIN_ExtrudeToolNameSubItem,
									ID_MAIN_ExtrudeToolStatusSubItem,
									ID_MAIN_ExtrudeToolActiveTemperatureSubItem,
									ID_MAIN_ExtrudeToolStandbyTemperatureSubItem);
		if (id == ID_MAIN_ExtrudeToolFilamentSubItem)
		{
			UI::ToolsList::ToolListItemData data = s_toolsList.GetToolListItemDataBySlot(index);
			s_filamentDialogTool = data.tool;
			if (s_filamentDialogTool == nullptr || s_filamentDialogTool->filamentExtruder < 0)
			{
				s_filamentDialogTool = nullptr;
				UI::WINDOW.CloseOverlay();
				return;
			}
			Comm::DUET.RequestFileList("/filaments");
			// TODO: need a way to recognise the response to this request compared to other file list requests
			UI::GetUIControl<ZKButton>(ID_MAIN_UnloadFilamentBtn)
				->setInvalid(s_filamentDialogTool->GetFilament().IsEmpty());
			UI::GetUIControl<ZKTextView>(ID_MAIN_FilamentControlHeading)
				->setTextTrf("tool_filament_control", s_filamentDialogTool->index);
			UI::WINDOW.OpenOverlay(UI::GetUIControl<ZKWindow>(ID_MAIN_FilamentLoadUnloadWindow));
		}
	}

	size_t GetFilamentListCount()
	{
		if (OM::FileSystem::GetCurrentDirPath().find("/filaments") == std::string::npos)
		{
			return 0;
		}
		return OM::FileSystem::GetItemCount();
	}

	void SetFilamentListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		OM::FileSystem::FileSystemItem* item = OM::FileSystem::GetItem(index);
		if (item == nullptr || s_filamentDialogTool == nullptr)
		{
			pListItem->setText("");
			return;
		}
		pListItem->setText(item->GetName());
		pListItem->setSelected(item->GetName() == s_filamentDialogTool->GetFilament().c_str());
		UI::GetUIControl<ZKButton>(ID_MAIN_UnloadFilamentBtn)
			->setInvalid(s_filamentDialogTool->GetFilament().IsEmpty());
	}

	void FilamentListItemCallback(const int index, const int id)
	{
		OM::FileSystem::FileSystemItem* item = OM::FileSystem::GetItem(index);
		if (item == nullptr || s_filamentDialogTool == nullptr)
		{
			return;
		}
		Comm::DUET.SendGcodef("T%d\n"
							  "M702\n"
							  "M701 S\"%s\"\n"
							  "M703",
							  s_filamentDialogTool->index,
							  item->GetName().c_str());
		UI::WINDOW.CloseOverlay();
	}

	size_t GetExtrusionDistanceCount()
	{
		return sizeof(s_extrusionFeedDistances) / sizeof(s_extrusionFeedDistances[0]);
	}

	void SetExtrusionDistanceListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		if (index < 0 || index >= (int)GetExtrusionDistanceCount())
		{
			return;
		}
		pListItem->setTextf("%d", s_extrusionFeedDistances[index]);
		pListItem->setSelected(s_defaultExtrusionFeedDistance == s_extrusionFeedDistances[index]);
	}

	void ExtrusionDistanceListItemCallback(const int index)
	{
		if (index < 0 || index >= (int)GetExtrusionDistanceCount())
		{
			return;
		}
		s_defaultExtrusionFeedDistance = s_extrusionFeedDistances[index];
		UI::GetUIControl<ZKListView>(ID_MAIN_ExtruderFeedrate)->refreshListView();
	}

	size_t GetExtrusionSpeedCount()
	{
		return sizeof(s_extrusionFeedRates) / sizeof(s_extrusionFeedRates[0]);
	}

	void SetExtrusionSpeedListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		if (index < 0 || index >= (int)GetExtrusionSpeedCount())
		{
			return;
		}
		pListItem->setTextf("%d", s_extrusionFeedRates[index]);
		pListItem->setSelected(s_defaultExtrusionFeedRate == s_extrusionFeedRates[index]);
	}

	void ExtrusionSpeedListItemCallback(const int index)
	{
		if (index < 0 || index >= (int)GetExtrusionSpeedCount())
		{
			return;
		}
		s_defaultExtrusionFeedRate = s_extrusionFeedRates[index];
		UI::GetUIControl<ZKListView>(ID_MAIN_ExtruderFeedrate)->refreshListView();
	}

	void ExtrudeFilament()
	{
		Comm::DUET.SendGcodef("G1 E%u F%u\n", s_defaultExtrusionFeedDistance, s_defaultExtrusionFeedRate * 60);
	}

	void RetractFilament()
	{
		Comm::DUET.SendGcodef("G1 E-%u F%u\n", s_defaultExtrusionFeedDistance, s_defaultExtrusionFeedRate * 60);
	}

	bool UnloadFilament()
	{
		if (s_filamentDialogTool == nullptr)
		{
			return false;
		}
		Comm::DUET.SendGcodef("T%d", s_filamentDialogTool->index);
		Comm::DUET.SendGcode("M702");
		return true;
	}
} // namespace UI::ExtrusionControl
