/*
 * Move.cpp
 *
 *  Created on: 24 April 2024
 *      Author: Andy Everitt
 */

#include "Debug.h"

#include "Move.h"

#include "Configuration.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Axis.h"
#include "Storage.h"
#include "UI/Logic/Heightmap.h"
#include <storage/StoragePreferences.h>

namespace UI::Move
{
	static float s_moveDistances[] = {-50, -10, -1, -0.1, 0.1, 1, 10, 50}; // mm
	static uint32_t s_moveFeedRates[] = {300, 100, 50, 20, 10, 5};		   // mm/s : can be overridden by user
	static uint32_t s_selectedFeedRateIndex = 2;

	static void SetFeedRateAmount(const int index, int amount)
	{
		if (index < 0 || index >= (int)ARRAY_SIZE(s_moveFeedRates))
		{
			warn("Invalid feedrate index %d", index);
			return;
		}
		if (amount <= 0 || amount > MAX_MOVE_FEEDRATE)
		{
			warn("Invalid feedrate amount: %d", amount);
			return;
		}
		info("Setting s_moveFeedRates[%d] to %d", index, amount);
		StoragePreferences::putInt(utils::format(ID_MOVE_FEEDRATE, index), amount);
		s_moveFeedRates[index] = amount;
	}

	class FeedRateLongClickListener : public ZKListView::IItemLongClickListener
	{
		virtual void onItemLongClick(ZKListView* pListView, int index, int itemID)
		{
			dbg("Long click on feedRate button");

			UI::NUMPAD_WINDOW.Open(
				LANGUAGEMANAGER->getValue("set_move_feedrate").c_str(),
				1,
				MAX_MOVE_FEEDRATE,
				s_moveFeedRates[index],
				[](int) {},
				[index](int value) { SetFeedRateAmount(index, value); });
		}
	};

	void Init()
	{
		static FeedRateLongClickListener s_feedRateListener;

		for (size_t i = 0; i < ARRAY_SIZE(s_moveFeedRates); i++)
		{
			s_moveFeedRates[i] = StoragePreferences::getInt(utils::format(ID_MOVE_FEEDRATE, i), s_moveFeedRates[i]);
		}
		s_selectedFeedRateIndex = StoragePreferences::getInt(ID_MOVE_SELECTED_FEEDRATE, s_selectedFeedRateIndex);

		UI::GetUIControl<ZKListView>(ID_MAIN_MoveFeedrate)->setItemLongClickListener(&s_feedRateListener);
	}

	void HomeAll()
	{
		Comm::DUET.SendGcode("G28\n");
	}

	void TrueLevel()
	{
		Comm::DUET.SendGcode("G32\n");
	}

	void MeshLevel()
	{
		Comm::DUET.SendGcode("G29\n");
	}

	void DisableMotors()
	{
		Comm::DUET.SendGcode("M18\n");
	}

	void OpenHeightmap()
	{
		UI::Heightmap::Open();
	}

	size_t GetAxisListCount()
	{
		return OM::Move::GetAxisCount();
	}

	void SetAxisListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		verbose("%d", index);
		ZKListView::ZKListSubItem* pHome = pListItem->findSubItemByID(ID_MAIN_AxisControlHomeSubItem);
		ZKListView::ZKListSubItem* pMachinePosition =
			pListItem->findSubItemByID(ID_MAIN_AxisControlMachinePositionSubItem);
		ZKListView::ZKListSubItem* pUserPosition = pListItem->findSubItemByID(ID_MAIN_AxisControlUserPositionSubItem);
		OM::Move::Axis* axis = OM::Move::GetAxisBySlot(index);
		if (axis == nullptr)
			return;

		pHome->setTextTrf("axis_control_home", axis->letter);
		pHome->setSelected(axis->homed);
		pUserPosition->setText(axis->userPosition);
		pMachinePosition->setTextf("(%.2f)", axis->machinePosition);

		pListItem->findSubItemByID(ID_MAIN_AxisControlSubItem1)->setTextf("%.1f", s_moveDistances[0]);
		pListItem->findSubItemByID(ID_MAIN_AxisControlSubItem2)->setTextf("%.1f", s_moveDistances[1]);
		pListItem->findSubItemByID(ID_MAIN_AxisControlSubItem3)->setTextf("%.1f", s_moveDistances[2]);
		pListItem->findSubItemByID(ID_MAIN_AxisControlSubItem4)->setTextf("%.1f", s_moveDistances[3]);
		pListItem->findSubItemByID(ID_MAIN_AxisControlSubItem5)->setTextf("%.1f", s_moveDistances[4]);
		pListItem->findSubItemByID(ID_MAIN_AxisControlSubItem6)->setTextf("%.1f", s_moveDistances[5]);
		pListItem->findSubItemByID(ID_MAIN_AxisControlSubItem7)->setTextf("%.1f", s_moveDistances[6]);
		pListItem->findSubItemByID(ID_MAIN_AxisControlSubItem8)->setTextf("%.1f", s_moveDistances[7]);
	}

	void AxisListItemCallback(const int index, const int id)
	{
		OM::Move::Axis* axis = OM::Move::GetAxisBySlot(index);
		if (axis == nullptr)
			return;

		float distance = 0;
		switch (id)
		{
		case ID_MAIN_AxisControlHomeSubItem:
			Comm::DUET.SendGcodef("G28 %s\n", axis->letter);
			return;
		case ID_MAIN_AxisControlSubItem1:
			distance = s_moveDistances[0];
			break;
		case ID_MAIN_AxisControlSubItem2:
			distance = s_moveDistances[1];
			break;
		case ID_MAIN_AxisControlSubItem3:
			distance = s_moveDistances[2];
			break;
		case ID_MAIN_AxisControlSubItem4:
			distance = s_moveDistances[3];
			break;
		case ID_MAIN_AxisControlSubItem5:
			distance = s_moveDistances[4];
			break;
		case ID_MAIN_AxisControlSubItem6:
			distance = s_moveDistances[5];
			break;
		case ID_MAIN_AxisControlSubItem7:
			distance = s_moveDistances[6];
			break;
		case ID_MAIN_AxisControlSubItem8:
			distance = s_moveDistances[7];
			break;
		}
		Comm::DUET.SendGcodef(
			"G91\nG1 %s%.3f F%d\nG90\n", axis->letter, distance, s_moveFeedRates[s_selectedFeedRateIndex] * 60);
	}

	size_t GetFeedRateCount()
	{
		return ARRAY_SIZE(s_moveFeedRates);
	}

	void SetFeedRateListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		verbose("%d", index);
		if (index < 0 || index >= (int)ARRAY_SIZE(s_moveFeedRates))
		{
			return;
		}
		pListItem->setTextf("%d", s_moveFeedRates[index]);
		pListItem->setSelected((int)s_selectedFeedRateIndex == index);
	}

	void FeedRateListItemCallback(const int index)
	{
		if (index < 0 || index >= (int)ARRAY_SIZE(s_moveFeedRates))
		{
			return;
		}
		StoragePreferences::putInt(ID_MOVE_SELECTED_FEEDRATE, index);
		s_selectedFeedRateIndex = index;
	}

} // namespace UI::Move
