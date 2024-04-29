/*
 * PrintStatus.h
 *
 *  Created on: 24 April 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_UI_LOGIC_PRINTSTATUS_H_
#define JNI_UI_LOGIC_PRINTSTATUS_H_

#include "UI/UserInterface.h"

namespace UI::PrintStatus
{
    void Init();

	void BabyStepDown();
	void BabyStepUp();

	size_t GetFanListCount();
	void SetFanListItem(ZKListView::ZKListItem* pListItem, const int index);
	void FanListItemCallback(const int index);

	void PausePrint();
	void StopPrint();
	void ResumePrint();

	size_t GetAxisListCount();
	void SetAxisListItem(ZKListView::ZKListItem* pListItem, const int index);

	size_t GetExtruderListCount();
	void SetExtruderListItem(ZKListView::ZKListItem* pListItem, const int index);
	void ExtruderListItemCallback(const int index);

	void SpeedMultiplierCallback(const int progress);
	void OpenPrintSpeedFactorPopup();

	size_t GetToolsListCount();
	void SetToolsListItem(ZKListView::ZKListItem* pListItem, const int index);
	void ToolsListItemCallback(const int index, const int id);
} // namespace UI::PrintStatus

#endif /* JNI_UI_LOGIC_PRINTSTATUS_H_ */