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

	std::string GetObjectCancelXAxisText(int index);
	std::string GetObjectCancelYAxisText(int index);
	void SetObjectLabel(ZKListView::ZKListItem* pListItem, const int index);

	void CancelJobObject(const int index);
	void CancelCurrentJobObject();

	void RenderObjectMap();
	TouchListener& GetTouchListener();
} // namespace UI::ObjectCancel

#endif /* JNI_UI_LOGIC_OBJECTCANCEL_H_ */
