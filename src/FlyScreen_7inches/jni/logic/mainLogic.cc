#pragma once
#include <vector>
#include "json/json.h"
#include "Communication.hpp"
#include "Hardware/SerialIo.hpp"
//#include "UI/UserInterface.cc"
#include "UI/UserInterface.hpp"
#include "UI/Display.cc"
#include "UI/DisplayCustom.cc"

#define DEBUG (1)
#include "Debug.hpp"
/*
 * This file is generated by the GUI tool
 * File function: logic corresponding code for processing users
 * Function Description:
 *========================onButtonClick_XXXX
 When the button on the page is pressed, the system will call the corresponding function.
 XXX represents the [ID value] name in the GUI tool, such as Button1.
 When the return value is false, the system will no longer process this button,
 and when it returns true, the system will continue to process this button. Such as SYS_BACK.
 *========================onSlideWindowItemClick_XXXX(int index) 
 When there is a sliding window on the page and the user clicks the icon of the sliding window, the system will call this function.
 XXX represents the [ID value] name in the GUI tool, for example: slideWindow1.
 index represents the offset value of the pressed icon
 *========================onSeekBarChange_XXXX(int progress) 
 When there is a slider on the page and the user changes the progress, the system will call this function.
 XXX represents the [ID value] name in the GUI tool, such as SeekBar1.
 progress represents the current progress value
 *========================ogetListItemCount_XXXX()
 When there is a sliding list on the page, the system will call this interface to obtain the total number of lists when updating.
 XXX represents the [ID value] name in the GUI tool, such as List1.
 The return value is the total number of items in the current list
 *========================oobtainListItemData_XXXX(ZKListView::ZKListItem *pListItem, int index)
 When there is a sliding list on the page, the system will call this interface
 to obtain the content information under the current item of the list when updating,
 XXX represents the [ID value] name in the GUI tool. For example, List1.
 pListItem is a single item object in the map and index is the offset of the list's total purpose.
 See function description for details
 *========================Commonly used interface===============
 * LOGD(...)  interface to print debugging information
 * mTextXXXPtr->setText("****") Display text on the control TextXXX
 * mButton1Ptr->setSelected(true); Set the control mButton1 to the selected mode, the picture will be switched to the selected picture, and the button text will be switched to the selected color
 * mSeekBarPtr->setProgress(12) Adjust the progress to 12 on the control mSeekBar
 * mListView1Ptr->refreshListView() Make mListView1 re-refresh, call when the list data changes
 * mDashbroadView1Ptr->setTargetAngle(120) Adjust the pointer display angle on the control mDashbroadView1 to 120 degrees
 *
 * In Eclipse IDE, use the "alt + /" shortcut key to open the smart prompt
 */

#define TIMER_UPDATE_DATA 0  // Id of the timer used to update values from data received from serial


/**
 * Register timer
 * Fill the array to register the timer
 * Note: id cannot be repeated
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	{TIMER_UPDATE_DATA,  500}, // Timer id=0, interval of 500ms
//	{1,  100},
};

/**
 * Triggered when the interface is constructed
 */
static void onUI_init(){
    // Tips : Add the display code for UI initialization here, such as: mText1Ptr->setText("123");
	srand(0);

	UI::Element *elem = UI::Element::head;
	while (elem != nullptr)
	{
		elem->Init();
		elem = elem->next;
	}
}

/**
 * Triggered when switching to this interface
 */
static void onUI_intent(const Intent *intentPtr) {
    if (intentPtr != NULL) {
        //TODO
    }
}

/*
 * Triggered when the interface is displayed
 */
static void onUI_show() {

}

/*
 * Triggered when the interface is hidden
 */
static void onUI_hide() {

}

/*
 * Triggered when the interface completely exits
 */
static void onUI_quit() {

}

/**
 * Serial data callback interface
 */
static void onProtocolDataUpdate(const SProtocolData &rxData) {
	SerialIo::CheckInput(rxData.data, rxData.len);
}

/**
 * Timer trigger function
 * It is not recommended to write time-consuming operations in this function, otherwise it will affect UI refresh
 * Parameters:
 * 	- id: The id of the currently triggered timer is the same as the id at registration
 * Return value:
 * 	- True: Continue to run the current timer
 * 	- False: Stop running the current timer
 */
static bool onUI_Timer(int id){
	switch (id) {

		case TIMER_UPDATE_DATA:
		{
			Comm::sendNext();
		}
			break;

		case 1:
		{
			float val = (float)(rand()%100);
			dbg("rand %f", val);
			mTempGraphPtr->addData(0, val);

			int size = UI::elementMap.GetElements("key1").size();
			dbg("%d elements registered against key \"%s\"", size, "key1");

			size = UI::elementMap.GetElements("key2").size();
			dbg("%d elements registered against key \"%s\"", size, "key2");
		}
			break;
		default:
			break;
	}
	//LOGD("controlTabAxisPos %s", UI::controlTabAxisPos[0]->GetValue());

//	mAxisSlot1_labelPtr->setText(UI::controlTabAxisPos[0]->GetLabel());

    return true;
}

/**
 * Triggered when there is a new touch event
 * Parameters:
 * 	- ev: New touch event
 * Return value:
 * 	- True: Indicates that the touch event is intercepted here, and the system will no longer pass this touch event to the control
 * 	- False: Touch events will continue to be passed to the control
 */
static bool onmainActivityTouchEvent(const MotionEvent &ev) {
    switch (ev.mActionStatus) {
		case MotionEvent::E_ACTION_DOWN: // touch pressed
			//LOGD("Time = %ld Pos x = %d, y = %d", ev.mEventTime, ev.mX, ev.mY);
			break;
		case MotionEvent::E_ACTION_MOVE: // touch slide
			break;
		case MotionEvent::E_ACTION_UP: // touch released
			break;
		default:
			break;
	}
	return false;
}


static bool onButtonClick_TriggerBtn(ZKButton *pButton) {
    Comm::sendNext();

    return false;
}
static void onCheckedChanged_RadioGroup1(ZKRadioGroup* pRadioGroup, int checkedID) {
    LOGD(" RadioGroup RadioGroup1 checked %d", checkedID);
}

static int getListItemCount_ListView1(const ZKListView *pListView) {
    //LOGD("getListItemCount_ListView1 !\n");
    return 2;
}

static void obtainListItemData_ListView1(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ ListView1  !!!\n");
}

static void onListItemClick_ListView1(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ ListView1  !!!\n");
}
