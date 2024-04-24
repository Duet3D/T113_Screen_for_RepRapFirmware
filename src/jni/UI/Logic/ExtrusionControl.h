/*
 * ExtrusionControl.h
 *
 *  Created on: 24 April 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_UI_LOGIC_EXTRUSIONCONTROL_H_
#define JNI_UI_LOGIC_EXTRUSIONCONTROL_H_

#include "UI/UserInterface.h"

namespace UI::ExtrusionControl
{
	void Init();

	size_t GetListCount();
	void SetExtrudeListItem(ZKListView::ZKListItem* pListItem, const int index);
	void ExtrudeListItemCallback(const int index, const int id);

	size_t GetFilamentListCount();
	void SetFilamentListItem(ZKListView::ZKListItem* pListItem, const int index);
	void FilamentListItemCallback(const int index, const int id);

	size_t GetExtrusionDistanceCount();
	void SetExtrusionDistanceListItem(ZKListView::ZKListItem* pListItem, const int index);
	void ExtrusionDistanceListItemCallback(const int index);
	size_t GetExtrusionSpeedCount();
	void SetExtrusionSpeedListItem(ZKListView::ZKListItem* pListItem, const int index);
	void ExtrusionSpeedListItemCallback(const int index);

	void ExtrudeFilament();
	void RetractFilament();

	bool UnloadFilament();
} // namespace UI::ExtrusionControl

#endif /* JNI_UI_LOGIC_EXTRUSIONCONTROL_H_ */