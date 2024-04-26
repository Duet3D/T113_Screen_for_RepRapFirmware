/*
 * Move.cpp
 *
 *  Created on: 24 April 2024
 *      Author: Andy Everitt
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "Move.h"

#include "Hardware/Duet.h"
#include "ObjectModel/Axis.h"
#include "UI/Logic/Heightmap.h"

namespace UI::Move
{
	static float s_moveDistances[] = {-50, -10, -1, -0.1, 0.1, 1, 10, 50};
	static uint32_t s_moveFeedRates[] = {300, 100, 50, 20, 10, 5};
	static uint32_t s_defaultMoveFeedRate = 50;

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
		Comm::DUET.SendGcodef("G91\nG1 %s%.3f F%d\nG90\n", axis->letter, distance, s_defaultMoveFeedRate * 60);
	}

	size_t GetFeedRateCount()
	{
		return ARRAY_SIZE(s_moveFeedRates);
	}

	void SetFeedRateListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		if (index < 0 || index >= (int)ARRAY_SIZE(s_moveFeedRates))
		{
			return;
		}
		pListItem->setTextf("%d", s_moveFeedRates[index]);
		pListItem->setSelected(s_defaultMoveFeedRate == s_moveFeedRates[index]);
	}

	void FeedRateListItemCallback(const int index)
	{
		if (index < 0 || index >= (int)ARRAY_SIZE(s_moveFeedRates))
		{
			return;
		}
		s_defaultMoveFeedRate = s_moveFeedRates[index];
	}

} // namespace UI::Move
