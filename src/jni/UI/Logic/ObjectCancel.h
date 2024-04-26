/*
 * ObjectCancel.h
 *
 *  Created on: 19 Apr 2024
 *      Author: andy
 */

#ifndef JNI_UI_LOGIC_OBJECTCANCEL_H_
#define JNI_UI_LOGIC_OBJECTCANCEL_H_

#include "UI/UserInterface.h"

#include "ObjectModel/Job.h"

namespace UI::ObjectCancel
{
	class TouchListener : public ZKBase::ITouchListener
	{
	  public:
		virtual void onTouchEvent(ZKBase* pBase, const MotionEvent& ev);
	};

	void Init();

	void SetObjectCancelXAxisText(ZKListView* pListView, ZKListView::ZKListItem* pListItem, const int index);
	void SetObjectCancelYAxisText(ZKListView* pListView, ZKListView::ZKListItem* pListItem, const int index);

	inline size_t GetObjectCount()
	{
		return OM::GetJobObjectCount();
	}
	void SetObjectLabel(ZKListView::ZKListItem* pListItem, const int index);

	void CancelJobObject(const int index);
	void CancelCurrentJobObject();

	void RenderObjectMap();
	TouchListener& GetTouchListener();
} // namespace UI::ObjectCancel

#endif /* JNI_UI_LOGIC_OBJECTCANCEL_H_ */
