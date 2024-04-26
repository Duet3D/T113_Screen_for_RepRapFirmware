/*
 * PrintStatus.cpp
 *
 *  Created on: 24 April 2024
 *      Author: Andy Everitt
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "PrintStatus.h"

#include "Hardware/Duet.h"
#include "ObjectModel/Fan.h"
#include "ObjectModel/Files.h"
#include <manager/LanguageManager.h>

namespace UI::PrintStatus
{
	static ToolsList s_toolsList;

	void Init()
	{
		info("Initialising PrintStatus UI...");

		ZKListView* toolList = UI::GetUIControl<ZKListView>(ID_MAIN_PrintTemperatureList);
		if (toolList == nullptr)
		{
			error("Failed to get tool list");
			return;
		}

		s_toolsList.Init(toolList);
	}

	void BabyStepDown()
	{
		Comm::DUET.SendGcode("M290 S-0.05");
	}

	void BabyStepUp()
	{
		Comm::DUET.SendGcode("M290 S0.05");
	}

	size_t GetFanListCount()
	{
		return OM::GetFanCount();
	}

	void SetFanListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		OM::Fan* fan = OM::GetFanBySlot(index);
		if (fan == nullptr)
		{
			return;
		}
		pListItem->setTextTrf("fan_status", fan->index, (int)(100 * fan->requestedValue));
	}

	void FanListItemCallback(const int index)
	{
		OM::Fan* fan = OM::GetFanBySlot(index);
		if (fan == nullptr)
		{
			return;
		}
		size_t fanIndex = fan->index;
		UI::OpenSliderNumPad(
			utils::format(LANGUAGEMANAGER->getValue("fan_header").c_str(), fan->index).c_str(),
			"",
			"",
			"%",
			0,
			100,
			(int)(fan->requestedValue * 100),
			[fanIndex](int percent) {
				OM::Fan* fan = OM::GetFan(fanIndex);
				if (fan == nullptr)
				{
					return;
				}
				int fanSpeed = (percent * 255) / 100;
				Comm::DUET.SendGcodef("M106 P%d S%d\n", fan->index, fanSpeed);
			},
			true);
	}

	void PausePrint()
	{
		OM::FileSystem::PausePrint();
	}

	void StopPrint()
	{
		OM::FileSystem::StopPrint();
		UI::WINDOW.Home();
	}

	void ResumePrint()
	{
		OM::FileSystem::ResumePrint();
	}

	size_t GetAxisListCount()
	{
		return OM::Move::GetAxisCount();
	}

	void SetAxisListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		ZKListView::ZKListSubItem* pMachinePosition =
			pListItem->findSubItemByID(ID_MAIN_PrintPositionListMachinePositionSubItem);
		ZKListView::ZKListSubItem* pUserPosition =
			pListItem->findSubItemByID(ID_MAIN_PrintPositionListUserPositionSubItem);
		OM::Move::Axis* axis = OM::Move::GetAxisBySlot(index);
		if (axis == nullptr)
			return;

		pListItem->setText(axis->letter);
		pUserPosition->setText(axis->userPosition);
		pMachinePosition->setTextf("(%.2f)", axis->machinePosition);
	}

	size_t GetExtruderListCount()
	{
		return OM::Move::GetExtruderAxisCount();
	}

	void SetExtruderListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		ZKListView::ZKListSubItem* pSubItem = pListItem->findSubItemByID(ID_MAIN_PrintExtruderPositionListSubItem1);
		OM::Move::ExtruderAxis* extruder = OM::Move::GetExtruderAxisBySlot(index);
		if (extruder == nullptr)
			return;

		pListItem->setText(extruder->index);
		pSubItem->setText(extruder->position);
	}

	void ExtruderListItemCallback(const int index)
	{
		OM::Move::ExtruderAxis* extruder = OM::Move::GetExtruderAxisBySlot(index);
		if (extruder == nullptr)
		{
			return;
		}
		size_t extruderIndex = extruder->index;
		UI::OpenSliderNumPad(
			utils::format(LANGUAGEMANAGER->getValue("extrusion_factor_header").c_str(), extruderIndex).c_str(),
			"",
			"",
			"%",
			1,
			200,
			(int)(extruder->factor * 100),
			[extruderIndex](int percent) {
				OM::Move::ExtruderAxis* extruder = OM::Move::GetExtruderAxis(extruderIndex);
				if (extruder == nullptr)
				{
					return;
				}
				Comm::DUET.SendGcodef("M221 D%d S%d\n", extruder->index, percent);
			});
	}

	void SpeedMultiplierCallback(const int progress)
	{
		Comm::DUET.SendGcodef("M220 S%d\n", progress);
	}

	size_t GetToolsListCount()
	{
		return s_toolsList.GetTotalHeaterCount(false);
	}

	void SetToolsListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		s_toolsList.ObtainListItemData(pListItem,
									   index,
									   ID_MAIN_PrintTemperatureListNameSubItem,
									   ID_MAIN_PrintTemperatureListStatusSubItem,
									   ID_MAIN_PrintTemperatureListCurrentSubItem,
									   ID_MAIN_PrintTemperatureListActiveSubItem,
									   ID_MAIN_PrintTemperatureListStandbySubItem);
	}

	void ToolsListItemCallback(const int index, const int id)
	{
		s_toolsList.OnListItemClick(index,
									id,
									NULL,
									NULL,
									ID_MAIN_PrintTemperatureListActiveSubItem,
									ID_MAIN_PrintTemperatureListStandbySubItem);
	}
} // namespace UI::PrintStatus
