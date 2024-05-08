/*
 * Move.h
 *
 *  Created on: 24 April 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_UI_LOGIC_MOVE_H_
#define JNI_UI_LOGIC_MOVE_H_

#include "UI/UserInterface.h"

namespace UI::Move
{
	void Init();
	
	void HomeAll();
	void TrueLevel();
	void MeshLevel();
	void DisableMotors();
	void OpenHeightmap();

	size_t GetAxisListCount();
	void SetAxisListItem(ZKListView::ZKListItem* pListItem, const int index);
	void AxisListItemCallback(const int index, const int id);

	size_t GetFeedRateCount();
	void SetFeedRateListItem(ZKListView::ZKListItem* pListItem, const int index);
	void FeedRateListItemCallback(const int index);
} // namespace UI::Move

#endif /* JNI_UI_LOGIC_MOVE_H_ */