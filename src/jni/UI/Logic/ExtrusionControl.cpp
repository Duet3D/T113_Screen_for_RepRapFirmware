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

#include "Configuration.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Tool.h"
#include "Storage.h"
#include <manager/LanguageManager.h>
#include <storage/StoragePreferences.h>

namespace UI::ExtrusionControl
{
	static uint32_t s_extrusionFeedRates[] = {50, 10, 5, 2, 1};
	static uint32_t s_extrusionFeedDistances[] = {100, 50, 20, 10, 5, 2, 1};
	static uint32_t s_selectedExtrusionFeedRateIndex = 2;
	static uint32_t s_selectedExtrusionFeedDistanceIndex = 2;

	static OM::Tool* s_filamentDialogTool = nullptr;
	static ToolsList s_toolsList;

	static void SetFeedDistanceAmount(const int index, int amount)
	{
		if (index < 0 || index >= (int)ARRAY_SIZE(s_extrusionFeedDistances))
		{
			warn("Invalid feedrate index %d", index);
			return;
		}
		if (amount <= 0 || amount > MAX_EXTRUDE_DISTANCE)
		{
			warn("Invalid distance amount: %d", amount);
			return;
		}
		info("Setting s_extrusionFeedDistances[%d] to %d", index, amount);
		StoragePreferences::putInt(utils::format(ID_EXTRUSION_DISTANCE, index), amount);
		s_extrusionFeedDistances[index] = amount;
	}

	static void SetFeedRateAmount(const int index, int amount)
	{
		if (index < 0 || index >= (int)ARRAY_SIZE(s_extrusionFeedRates))
		{
			warn("Invalid feedrate index %d", index);
			return;
		}
		if (amount <= 0 || amount > MAX_EXTRUDE_FEEDRATE)
		{
			warn("Invalid feedrate amount: %d", amount);
			return;
		}
		info("Setting s_extrusionFeedRates[%d] to %d", index, amount);
		StoragePreferences::putInt(utils::format(ID_EXTRUSION_FEEDRATE, index), amount);
		s_extrusionFeedRates[index] = amount;
	}

	class FeedDistanceLongClickListener : public ZKListView::IItemLongClickListener
	{
		virtual void onItemLongClick(ZKListView* pListView, int index, int itemID)
		{
			dbg("Long click on distance button");

			UI::NUMPAD_WINDOW.Open(
				LANGUAGEMANAGER->getValue("set_extrusion_distance").c_str(),
				1,
				MAX_EXTRUDE_DISTANCE,
				s_extrusionFeedDistances[index],
				[](int) {},
				[index](int value) { SetFeedDistanceAmount(index, value); });
		}
	};

	class FeedRateLongClickListener : public ZKListView::IItemLongClickListener
	{
		virtual void onItemLongClick(ZKListView* pListView, int index, int itemID)
		{
			dbg("Long click on feedRate button");

			UI::NUMPAD_WINDOW.Open(
				LANGUAGEMANAGER->getValue("set_extrusion_feedrate").c_str(),
				1,
				MAX_EXTRUDE_FEEDRATE,
				s_extrusionFeedRates[index],
				[](int) {},
				[index](int value) { SetFeedRateAmount(index, value); });
		}
	};

	void Init()
	{
		static FeedDistanceLongClickListener distanceListener;
		static FeedRateLongClickListener feedrateListener;
		info("Initialising ExtrusionControl UI...");

		for (size_t i = 0; i < ARRAY_SIZE(s_extrusionFeedDistances); i++)
		{
			s_extrusionFeedDistances[i] =
				StoragePreferences::getInt(utils::format(ID_EXTRUSION_DISTANCE, i), s_extrusionFeedDistances[i]);
		}

		for (size_t i = 0; i < ARRAY_SIZE(s_extrusionFeedRates); i++)
		{
			s_extrusionFeedRates[i] =
				StoragePreferences::getInt(utils::format(ID_EXTRUSION_FEEDRATE, i), s_extrusionFeedRates[i]);
		}

		s_selectedExtrusionFeedDistanceIndex =
			StoragePreferences::getInt(ID_EXTRUSION_SELECTED_DISTANCE, s_selectedExtrusionFeedDistanceIndex);
		s_selectedExtrusionFeedRateIndex =
			StoragePreferences::getInt(ID_EXTRUSION_SELECTED_FEEDRATE, s_selectedExtrusionFeedRateIndex);

		UI::GetUIControl<ZKListView>(ID_MAIN_ExtruderFeedDist)->setItemLongClickListener(&distanceListener);
		UI::GetUIControl<ZKListView>(ID_MAIN_ExtruderFeedrate)->setItemLongClickListener(&feedrateListener);

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
		pListItem->setSelected((int)s_selectedExtrusionFeedDistanceIndex == index);
	}

	void ExtrusionDistanceListItemCallback(const int index)
	{
		if (index < 0 || index >= (int)GetExtrusionDistanceCount())
		{
			return;
		}
		StoragePreferences::putInt(ID_EXTRUSION_SELECTED_DISTANCE, index);
		s_selectedExtrusionFeedDistanceIndex = index;
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
		pListItem->setSelected((int)s_selectedExtrusionFeedRateIndex == index);
	}

	void ExtrusionSpeedListItemCallback(const int index)
	{
		if (index < 0 || index >= (int)GetExtrusionSpeedCount())
		{
			return;
		}
		StoragePreferences::putInt(ID_EXTRUSION_SELECTED_FEEDRATE, index);
		s_selectedExtrusionFeedRateIndex = index;
		UI::GetUIControl<ZKListView>(ID_MAIN_ExtruderFeedrate)->refreshListView();
	}

	void ExtrudeFilament()
	{
		Comm::DUET.SendGcodef("G1 E%u F%u\n",
							  s_extrusionFeedDistances[s_selectedExtrusionFeedDistanceIndex],
							  s_extrusionFeedRates[s_selectedExtrusionFeedRateIndex] * 60);
	}

	void RetractFilament()
	{
		Comm::DUET.SendGcodef("G1 E-%u F%u\n",
							  s_extrusionFeedDistances[s_selectedExtrusionFeedDistanceIndex],
							  s_extrusionFeedRates[s_selectedExtrusionFeedRateIndex] * 60);
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
