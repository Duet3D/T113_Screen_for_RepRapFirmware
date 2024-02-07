#pragma once
#define DEBUG_LEVEL 3

#include "Communication.h"
#include "Debug.h"
#include "timer.h"
#include "Hardware/Duet.h"
#include "Hardware/SerialIo.h"
#include "ObjectModel/Alert.h"
#include "ObjectModel/Fan.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Utils.h"
#include "UI/Colors.h"
#include "UI/Gcodes.h"
#include "UI/Observers/FanObservers.h"
#include "UI/Observers/FileObservers.h"
#include "UI/Observers/HeatObservers.h"
#include "UI/Observers/JobObservers.h"
#include "UI/Observers/MoveObservers.h"
#include "UI/Observers/ResponseObservers.h"
#include "UI/Observers/StateObservers.h"
#include "UI/Observers/ToolObservers.h"
#include "UI/OmObserver.h"
#include "UI/UserInterface.h"
#include "UI/UserInterfaceConstants.h"
#include "storage/StoragePreferences.h"
#include "utils/TimeHelper.h"
#include <string>
#include <vector>

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

static int sFeedRate = 6000;

/**
 * Register timer
 * Fill the array to register the timer
 * Note: id cannot be repeated
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	{TIMER_DELAYED_TASK, 50},
};

/**
 * Triggered when the interface is constructed
 */
static void onUI_init()
{
	// Tips : Add the display code for UI initialization here, such as: mText1Ptr->setText("123");
	srand(0);

	initTimer(mActivityPtr);
	registerUserTimer(
		TIMER_UPDATE_DATA,
		(int)Comm::defaultPrinterPollInterval); // Register here so it can be reset with stored poll interval

	Comm::duet.Init();
	OM::FileSystem::Init(mFolderIDPtr, mFileListViewPtr);
	UI::WINDOW->AddHome(mMainWindowPtr);
	UI::ToolsList::Create("home")->Init(mToolListViewPtr, mTemperatureInputWindowPtr, mNumPadInputPtr);
	UI::ToolsList::Create("print")->Init(mPrintTemperatureListPtr, mTemperatureInputWindowPtr, mNumPadInputPtr);
	UI::CONSOLE->Init(mConsoleListViewPtr, mEditText1Ptr);
	UI::POPUP_WINDOW->Init(mPopupWindowPtr,
						   mNoTouchWindowPtr,
						   mPopupOkBtnPtr,
						   mPopupCancelBtnPtr,
						   mPopupTitlePtr,
						   mPopupTextPtr,
						   mPopupWarningPtr,
						   mPopupMinPtr,
						   mPopupMaxPtr,
						   mPopupSelectionListPtr,
						   mPopupTextInputPtr,
						   mPopupNumberInputPtr,
						   mPopupAxisSelectionPtr,
						   mPopupAxisAdjusmentPtr);
	UI::SLIDER_WINDOW->Init(
		mSliderWindowPtr, mSliderPtr, mSliderHeaderPtr, mSliderValuePtr, mSliderPrefixPtr, mSliderSuffixPtr);

	// Duet communication settings
	mCommunicationTypePtr->setText(Comm::duetCommunicationTypeNames[(int)Comm::duet.GetCommunicationType()]);
	mIpAddressInputPtr->setText(Comm::duet.GetIPAddress());
	mHostnameInputPtr->setText(Comm::duet.GetHostname());
	mPasswordInputPtr->setText(Comm::duet.GetPassword());
	mPollIntervalInputPtr->setText((int)Comm::duet.GetPollInterval());
	mInfoTimeoutInputPtr->setText((int)UI::POPUP_WINDOW->GetTimeout());

	// Hide clock here so that it is visible when editing the GUI
	mDigitalClock1Ptr->setVisible(false);

	/* Initialise UI observer updaters that run on each field value that is received from the OM */
	auto *observer = UI::omFieldObserverHead;
	while (observer != nullptr)
	{
		observer->Init(UI::observerMap);
		observer = observer->next;
	}

	/* Initialise UI observer updaters that run after an array has been received */
	auto *observerArrayEnd = UI::omArrayEndObserverHead;
	while (observerArrayEnd != nullptr)
	{
		observerArrayEnd->Init(UI::observerMapArrayEnd);
		observerArrayEnd = observerArrayEnd->next;
	}

	mFeedrateBtn1Ptr->setSelected(true);
}

/**
 * Triggered when switching to this interface
 */
static void onUI_intent(const Intent *intentPtr)
{
	if (intentPtr != NULL)
	{
		//TODO
	}
}

/*
 * Triggered when the interface is displayed
 */
static void onUI_show()
{

}

/*
 * Triggered when the interface is hidden
 */
static void onUI_hide()
{

}

/*
 * Triggered when the interface completely exits
 */
static void onUI_quit()
{

}

/**
 * Serial data callback interface
 */
static void onProtocolDataUpdate(const SProtocolData &rxData)
{
//	dbg("data %s", rxData.data);
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
static bool onUI_Timer(int id)
{
	switch (id)
	{

	case TIMER_UPDATE_DATA:
	{
		static OM::PrinterStatus status = OM::PrinterStatus::unknown;
		if (status != OM::GetStatus())
		{
			status = OM::GetStatus();
			mStatusTextPtr->setTextTr(OM::GetStatusText());
		}
		Comm::sendNext();
		break;
	}
	case TIMER_DELAYED_TASK: {
		runDelayedCallbacks();
	}
	default:
		break;
	}

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
static bool onmainActivityTouchEvent(const MotionEvent &ev)
{
	switch (ev.mActionStatus)
	{
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
static bool onButtonClick_HomeBtn(ZKButton *pButton)
{
	dbg(" ButtonClick HomeBtn !!!\n");
	UI::WINDOW->Home();
	return false;
}
static bool onButtonClick_BackBtn(ZKButton *pButton)
{
	dbg("Back btn pressed, returning to previous window");
	UI::WINDOW->Back();
	return false;
}

static bool onButtonClick_MacroBtn(ZKButton *pButton)
{
	OM::FileSystem::RequestFiles("0:/macros");
	UI::WINDOW->CloseLastWindow();
	UI::WINDOW->OpenWindow(mFilesWindowPtr);
	return false;
}

static bool onButtonClick_ConsoleBtn(ZKButton* pButton)
{
	UI::WINDOW->CloseLastWindow();
	UI::WINDOW->OpenWindow(mConsoleWindowPtr);
    return false;
}

static bool onButtonClick_EStopBtn(ZKButton *pButton)
{
	UI::POPUP_WINDOW->Close();
	Comm::duet.SendGcode("M112\n");
	Thread::sleep(1000);
	Comm::duet.SendGcode("M999\n");
	return false;
}

static int getListItemCount_ToolListView(const ZKListView *pListView)
{
	size_t count = UI::ToolsList::Get("home")->GetTotalHeaterCount(false);
	return count;
}

static void obtainListItemData_ToolListView(ZKListView *pListView, ZKListView::ZKListItem *pListItem, int index)
{
	ZKListView::ZKListSubItem *ptoolName = pListItem->findSubItemByID(ID_MAIN_ToolNameSubItem);
	ZKListView::ZKListSubItem *pcurrentTemperature = pListItem->findSubItemByID(ID_MAIN_ToolCurrentTemperatureSubItem);
	ZKListView::ZKListSubItem *pactiveTemperature = pListItem->findSubItemByID(ID_MAIN_ToolActiveTemperatureSubItem);
	ZKListView::ZKListSubItem *pstandbyTemperature = pListItem->findSubItemByID(ID_MAIN_ToolStandbyTemperatureSubItem);
	ZKListView::ZKListSubItem *pstatus = pListItem->findSubItemByID(ID_MAIN_ToolStatusSubItem);

	UI::ToolsList::Get("home")->ObtainListItemData(pListItem, index, ptoolName, pcurrentTemperature, pactiveTemperature, pstandbyTemperature, pstatus);
}

static void onListItemClick_ToolListView(ZKListView *pListView, int index, int id)
{
	UI::ToolsList::Get("home")->OnListItemClick(
			index, id,
			ID_MAIN_ToolNameSubItem, ID_MAIN_ToolStatusSubItem,
			ID_MAIN_ToolActiveTemperatureSubItem, ID_MAIN_ToolStandbyTemperatureSubItem);
}

static void onSlideItemClick_SlideWindow1(ZKSlideWindow *pSlideWindow, int index)
{
	dbg(" onSlideItemClick_ SlideWindow1 %d !!!\n", index);
	UI::WINDOW->CloseWindow(mMainWindowPtr);
	switch (index)
	{
	case (int)UI::SlideWindowIndex::move:
		UI::WINDOW->OpenWindow(mMoveWindowPtr);
		break;
	case (int)UI::SlideWindowIndex::extrude:
		UI::WINDOW->OpenWindow(mExtrudeWindowPtr);
		break;
	case (int)UI::SlideWindowIndex::status:
		UI::WINDOW->OpenWindow(mPrintWindowPtr);
		break;
	case (int)UI::SlideWindowIndex::heightmap:
		UI::WINDOW->OpenWindow(mHeightMapWindowPtr);
		break;
	case (int)UI::SlideWindowIndex::fans:
		UI::WINDOW->OpenWindow(mFanWindowPtr);
		break;
	case (int)UI::SlideWindowIndex::print:
			OM::FileSystem::RequestFiles("0:/gcodes");
		UI::WINDOW->OpenWindow(mFilesWindowPtr);
		break;
	case (int)UI::SlideWindowIndex::network:
		UI::WINDOW->Home();
		// UI::WINDOW->OpenWindow(mNetworkWindowPtr);
		EASYUICONTEXT->openActivity("WifiSettingActivity");
		break;
	case (int)UI::SlideWindowIndex::settings:
		UI::WINDOW->OpenWindow(mSettingsWindowPtr);
		break;
	}
}
static int getListItemCount_TemperatureGraphLegend(const ZKListView *pListView)
{
	//LOGD("getListItemCount_TemperatureGraphLegend !\n");
	return 5;
}

static void obtainListItemData_TemperatureGraphLegend(ZKListView *pListView, ZKListView::ZKListItem *pListItem, int index)
{
	//LOGD(" obtainListItemData_ TemperatureGraphLegend  !!!\n");
}

static void onListItemClick_TemperatureGraphLegend(ZKListView *pListView, int index, int id)
{
	//LOGD(" onListItemClick_ TemperatureGraphLegend  !!!\n");
}

static UI::ToolsList* getVisibleToolsList()
{
	if (mPrintWindowPtr->isVisible())
		return UI::ToolsList::Get("print");
	return UI::ToolsList::Get("home");
}
static bool onButtonClick_NumPad1(ZKButton *pButton) {
	getVisibleToolsList()->NumPadAddOneChar('1');
	return false;
}

static bool onButtonClick_NumPad2(ZKButton *pButton) {
	getVisibleToolsList()->NumPadAddOneChar('2');
	return false;
}

static bool onButtonClick_NumPad3(ZKButton *pButton) {
	getVisibleToolsList()->NumPadAddOneChar('3');
	return false;
}

static bool onButtonClick_NumPad4(ZKButton *pButton) {
	getVisibleToolsList()->NumPadAddOneChar('4');
	return false;
}

static bool onButtonClick_NumPad5(ZKButton *pButton) {
	getVisibleToolsList()->NumPadAddOneChar('5');
	return false;
}

static bool onButtonClick_NumPad6(ZKButton *pButton) {
	getVisibleToolsList()->NumPadAddOneChar('6');
	return false;
}

static bool onButtonClick_NumPad7(ZKButton *pButton) {
	getVisibleToolsList()->NumPadAddOneChar('7');
	return false;
}

static bool onButtonClick_NumPad8(ZKButton *pButton) {
	getVisibleToolsList()->NumPadAddOneChar('8');
	return false;
}

static bool onButtonClick_NumPad9(ZKButton *pButton) {
	getVisibleToolsList()->NumPadAddOneChar('9');
	return false;
}

static bool onButtonClick_NumPadDel(ZKButton *pButton) {
	getVisibleToolsList()->NumPadDelOneChar();
	return false;
}

static bool onButtonClick_NumPad0(ZKButton *pButton) {
	getVisibleToolsList()->NumPadAddOneChar('0');
	return false;
}

static bool onButtonClick_NumPadConfirm(ZKButton *pButton) {
	getVisibleToolsList()->SendTempTarget();
	getVisibleToolsList()->CloseNumPad();
	return false;
}
static bool onButtonClick_HomeAllBtn(ZKButton *pButton) {
	Comm::duet.SendGcode("G28\n");
	return false;
}

static bool onButtonClick_TrueLevelBtn(ZKButton* pButton)
{
	Comm::duet.SendGcode("G32\n");
	return false;
}

static bool onButtonClick_MeshLevelBtn(ZKButton* pButton)
{
	Comm::duet.SendGcode("G29\n");
	return false;
}

static bool onButtonClick_DisableMotorsBtn(ZKButton* pButton)
{
	Comm::duet.SendGcode("M18\n");
	return false;
}

static bool onButtonClick_HeightmapBtn(ZKButton* pButton)
{
	UI::WINDOW->CloseWindow(mMoveWindowPtr);
	UI::WINDOW->OpenWindow(mHeightMapWindowPtr);
	return false;
}

static int getListItemCount_AxisControlListView(const ZKListView* pListView)
{
	return OM::Move::GetAxisCount();
}

static void obtainListItemData_AxisControlListView(ZKListView* pListView, ZKListView::ZKListItem* pListItem, int index)
{
	ZKListView::ZKListSubItem* pHome = pListItem->findSubItemByID(ID_MAIN_AxisControlHomeSubItem);
	ZKListView::ZKListSubItem* pMachinePosition = pListItem->findSubItemByID(ID_MAIN_AxisControlMachinePositionSubItem);
	ZKListView::ZKListSubItem* pUserPosition = pListItem->findSubItemByID(ID_MAIN_AxisControlUserPositionSubItem);
	OM::Move::Axis* axis = OM::Move::GetAxisBySlot(index);
	if (axis == nullptr) return;

	pHome->setTextTrf("axis_control_home", axis->letter);
	pHome->setSelected(axis->homed);
	pUserPosition->setText(axis->userPosition);
	pMachinePosition->setTextf("(%.2f)", axis->machinePosition);
}

static void onListItemClick_AxisControlListView(ZKListView* pListView, int index, int id)
{
	OM::Move::Axis* axis = OM::Move::GetAxisBySlot(index);
	if (axis == nullptr) return;

	int distance = 0;
	switch (id)
	{
	case ID_MAIN_AxisControlHomeSubItem:
		Comm::duet.SendGcodef("G28 %s\n", axis->letter);
		return;
	case ID_MAIN_AxisControlSubItem1:
		distance = -50;
		break;
	case ID_MAIN_AxisControlSubItem2:
		distance = -10;
		break;
	case ID_MAIN_AxisControlSubItem3:
		distance = -1;
		break;
	case ID_MAIN_AxisControlSubItem4:
		distance = -0.1;
		break;
	case ID_MAIN_AxisControlSubItem5:
		distance = 0.1;
		break;
	case ID_MAIN_AxisControlSubItem6:
		distance = 1;
		break;
	case ID_MAIN_AxisControlSubItem7:
		distance = 10;
		break;
	case ID_MAIN_AxisControlSubItem8:
		distance = 50;
		break;
	}
	Comm::duet.SendGcodef("G91\nG1 %s%d F%d\nG90\n", axis->letter, distance, sFeedRate);
}

static void selectFeedRateBtn(ZKButton* pButton)
{
	mFeedrateBtn1Ptr->setSelected(false);
	mFeedrateBtn2Ptr->setSelected(false);
	mFeedrateBtn3Ptr->setSelected(false);
	mFeedrateBtn4Ptr->setSelected(false);
	mFeedrateBtn5Ptr->setSelected(false);
	pButton->setSelected(true);
}

static bool onButtonClick_FeedrateBtn1(ZKButton* pButton)
{
	sFeedRate = 100 * 60;
	selectFeedRateBtn(pButton);
	return false;
}

static bool onButtonClick_FeedrateBtn2(ZKButton *pButton) {
	sFeedRate = 50 * 60;
	selectFeedRateBtn(pButton);
	return false;
}

static bool onButtonClick_FeedrateBtn3(ZKButton *pButton) {
	sFeedRate = 20 * 60;
	selectFeedRateBtn(pButton);
	return false;
}

static bool onButtonClick_FeedrateBtn4(ZKButton *pButton) {
	sFeedRate = 10 * 60;
	selectFeedRateBtn(pButton);
	return false;
}

static bool onButtonClick_FeedrateBtn5(ZKButton *pButton) {
	sFeedRate = 1 * 60;
	selectFeedRateBtn(pButton);
	return false;
}
static int getListItemCount_ConsoleListView(const ZKListView *pListView) {
    //LOGD("getListItemCount_ConsoleListView !\n");
    return MaxResponseLines;
}

static void obtainListItemData_ConsoleListView(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ ConsoleListView  !!!\n");
	pListItem->setText(UI::CONSOLE->GetItem(index).c_str());
}

static void onListItemClick_ConsoleListView(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ ConsoleListView  !!!\n");
}

static int getListItemCount_GcodeListView(const ZKListView *pListView) {
	return sizeof(Gcode) / sizeof(gcode);
}

static void obtainListItemData_GcodeListView(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
	pListItem->setTextTr(Gcode[index].displayText);
}

static void onListItemClick_GcodeListView(ZKListView *pListView, int index, int id) {
}

static void onEditTextChanged_EditText1(const std::string& text)
{
	UI::CONSOLE->AddCommand(text);
	Comm::duet.SendGcode(text.c_str());
}

static bool onButtonClick_SendBtn(ZKButton* pButton)
{
	UI::CONSOLE->AddCommand(mEditText1Ptr->getText());
	Comm::duet.SendGcode(mEditText1Ptr->getText().c_str());
	return true;
}
static bool onButtonClick_ConsoleClearBtn(ZKButton *pButton) {
    UI::CONSOLE->Clear();
    return false;
}

static bool onButtonClick_FileRefreshBtn(ZKButton *pButton) {
	OM::FileSystem::ClearFileSystem();
	OM::FileSystem::RequestFiles(OM::FileSystem::GetCurrentDirPath());
    return false;
}
static int getListItemCount_FileListView(const ZKListView *pListView) {
    //LOGD("getListItemCount_FileListView !\n");
    return OM::FileSystem::GetItemCount();
}

static void obtainListItemData_FileListView(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ FileListView  !!!\n");
	ZKListView::ZKListSubItem *pFileType = pListItem->findSubItemByID(ID_MAIN_FileTypeSubItem);
	ZKListView::ZKListSubItem *pFileSize = pListItem->findSubItemByID(ID_MAIN_FileSizeSubItem);
	ZKListView::ZKListSubItem *pFileDate = pListItem->findSubItemByID(ID_MAIN_FileDateSubItem);
//	ZKListView::ZKListSubItem *pFileThumbnail = pListItem->findSubItemByID(ID_MAIN_FileThumbnailSubItem);

	OM::FileSystem::FileSystemItem* item = OM::FileSystem::GetItem(index);
	if (item == nullptr)
		return;
//	dbg("Files: settings list item %d name to %s", index, item->GetName().c_str());
	pListItem->setText(item->GetName());
	switch (item->GetType())
	{
	case OM::FileSystem::FileSystemItemType::file:
		pListItem->setSelected(false);
		pFileType->setTextTr("file");
		break;
	case OM::FileSystem::FileSystemItemType::folder:
		pListItem->setSelected(true);
		pFileType->setTextTr("folder");
		break;
	}
	pFileSize->setTextTrf("file_size", item->GetSize());
	pFileDate->setTextTrf("file_date", item->GetDate().c_str());
}

static void onListItemClick_FileListView(ZKListView *pListView, int index, int id) {
	OM::FileSystem::FileSystemItem* item = OM::FileSystem::GetItem(index);
	if (UI::POPUP_WINDOW->IsBlocking())
		return;
	UI::POPUP_WINDOW->Cancel();
	switch (item->GetType())
	{
	case OM::FileSystem::FileSystemItemType::file:
		UI::SetSelectedFile((OM::FileSystem::File*)item);
		if (OM::FileSystem::IsMacroFolder())
		{
			UI::POPUP_WINDOW->Open([]() { UI::RunSelectedFile(); });
			UI::POPUP_WINDOW->SetTextf(LANGUAGEMANAGER->getValue("run_macro").c_str(), item->GetName().c_str());
		}
		else if (OM::FileSystem::IsUsbFolder())
		{
			UI::POPUP_WINDOW->Open([]() { OM::FileSystem::UploadFile(UI::GetSelectedFile()); });
			UI::POPUP_WINDOW->SetTextf(LANGUAGEMANAGER->getValue("upload_file").c_str(), item->GetName().c_str());
		}
		else
		{
			UI::POPUP_WINDOW->Open([]() {
				UI::RunSelectedFile();
				UI::WINDOW->CloseLastWindow();
				UI::WINDOW->OpenWindow(mPrintWindowPtr);
			});
			UI::POPUP_WINDOW->SetTextf(LANGUAGEMANAGER->getValue("start_print").c_str(), item->GetName().c_str());
		}
		break;
	case OM::FileSystem::FileSystemItemType::folder:
		if (OM::FileSystem::IsUsbFolder())
		{
			OM::FileSystem::RequestUsbFiles(item->GetPath());
			break;
		}
		OM::FileSystem::RequestFiles(item->GetPath());
		break;
	}
}
static bool onButtonClick_PrintBabystepDecBtn(ZKButton *pButton) {
	Comm::duet.SendGcode("M290 S-0.05");
	return false;
}

static bool onButtonClick_PrintBabystepIncBtn(ZKButton *pButton) {
	Comm::duet.SendGcode("M290 S0.05");
	return false;
}

static int getListItemCount_PrintFanList(const ZKListView *pListView) {
    //LOGD("getListItemCount_PrintFanList !\n");
    return OM::GetFanCount();
}

static void obtainListItemData_PrintFanList(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    OM::Fan* fan = OM::GetFanBySlot(index);
    if (fan == nullptr)
    {
    	return;
    }
	pListItem->setTextTrf("fan_status", fan->index, (int)(100 * fan->requestedValue));
}

static void onListItemClick_PrintFanList(ZKListView* pListView, int index, int id)
{
	OM::Fan* fan = OM::GetFanBySlot(index);
	if (fan == nullptr) { return; }
	char header[32];
	SafeSnprintf(header, sizeof(header), LANGUAGEMANAGER->getValue("fan_header").c_str(), fan->index);
	size_t fanIndex = fan->index;
	UI::SLIDER_WINDOW->Open(
		header, "", "", "%", 0, 100, (int)(fan->requestedValue * 255),
		[fanIndex](int percent) {
			OM::Fan* fan = OM::GetFan(fanIndex);
			if (fan == nullptr) { return; }
			int fanSpeed = (percent * 255) / 100;
			Comm::duet.SendGcodef("M106 P%d S%d\n", fan->index, fanSpeed);
		},
		true);
}

static bool onButtonClick_PrintPauseBtn(ZKButton *pButton) {
    OM::FileSystem::PausePrint();
    return false;
}

static bool onButtonClick_PrintCancelBtn(ZKButton *pButton) {
    OM::FileSystem::StopPrint();
    UI::WINDOW->Home();
    return false;
}

static bool onButtonClick_PrintResumeBtn(ZKButton *pButton) {
	OM::FileSystem::ResumePrint();
    return false;
}

static int getListItemCount_PrintPositionList(const ZKListView *pListView) {
    return OM::Move::GetAxisCount();
}

static void obtainListItemData_PrintPositionList(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    ZKListView::ZKListSubItem* pMachinePosition = pListItem->findSubItemByID(ID_MAIN_PrintPositionListMachinePositionSubItem);
    ZKListView::ZKListSubItem* pUserPosition = pListItem->findSubItemByID(ID_MAIN_PrintPositionListUserPositionSubItem);
	OM::Move::Axis* axis = OM::Move::GetAxisBySlot(index);
	if (axis == nullptr) return;

	pListItem->setText(axis->letter);
	pUserPosition->setText(axis->userPosition);
	pMachinePosition->setTextf("(%.2f)", axis->machinePosition);
}

static void onListItemClick_PrintPositionList(ZKListView* pListView, int index, int id) {}

static int getListItemCount_PrintExtruderPositionList(const ZKListView *pListView) {
    //LOGD("getListItemCount_PrintExtruderPositionList !\n");
    return OM::Move::GetExtruderAxisCount();
}

static void obtainListItemData_PrintExtruderPositionList(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    ZKListView::ZKListSubItem* pSubItem = pListItem->findSubItemByID(ID_MAIN_PrintExtruderPositionListSubItem1);
	OM::Move::ExtruderAxis* extruder = OM::Move::GetExtruderAxisBySlot(index);
	if (extruder == nullptr) return;

	pListItem->setText(extruder->index);
	pSubItem->setText(extruder->position);
}

static void onListItemClick_PrintExtruderPositionList(ZKListView *pListView, int index, int id) {
	OM::Move::ExtruderAxis* extruder = OM::Move::GetExtruderAxisBySlot(index);
	if (extruder == nullptr) { return; }
	char header[32];
	size_t extruderIndex = extruder->index;
	SafeSnprintf(header, sizeof(header), LANGUAGEMANAGER->getValue("extrusion_factor_header").c_str(), extruderIndex);
	UI::SLIDER_WINDOW->Open(header, "", "", "%", 0, 200, (int)(extruder->factor * 100), [extruderIndex](int percent) {
		OM::Move::ExtruderAxis* extruder = OM::Move::GetExtruderAxis(extruderIndex);
		if (extruder == nullptr) { return; }
		Comm::duet.SendGcodef("M221 D%d S%d\n", extruder->index, percent);
	});
}

static void onProgressChanged_PrintSpeedMultiplierBar(ZKSeekBar *pSeekBar, int progress) {
	Comm::duet.SendGcodef("M220 S%d\n", progress);
}

static int getListItemCount_PrintTemperatureList(const ZKListView *pListView) {
	return UI::ToolsList::Get("home")->GetTotalHeaterCount(false);
}

static void obtainListItemData_PrintTemperatureList(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
	ZKListView::ZKListSubItem *ptoolName = pListItem->findSubItemByID(ID_MAIN_PrintTemperatureListNameSubItem);
	ZKListView::ZKListSubItem *pcurrentTemperature = pListItem->findSubItemByID(ID_MAIN_PrintTemperatureListCurrentSubItem);
	ZKListView::ZKListSubItem *pactiveTemperature = pListItem->findSubItemByID(ID_MAIN_PrintTemperatureListActiveSubItem);
	ZKListView::ZKListSubItem *pstandbyTemperature = pListItem->findSubItemByID(ID_MAIN_PrintTemperatureListStandbySubItem);
	ZKListView::ZKListSubItem *pstatus = pListItem->findSubItemByID(ID_MAIN_PrintTemperatureListStatusSubItem);

	UI::ToolsList::Get("print")->ObtainListItemData(pListItem, index, ptoolName, pcurrentTemperature, pactiveTemperature, pstandbyTemperature, pstatus);
}

static void onListItemClick_PrintTemperatureList(ZKListView *pListView, int index, int id) {
	UI::ToolsList::Get("print")->OnListItemClick(
			index, id,
			NULL, NULL,
			ID_MAIN_PrintTemperatureListActiveSubItem, ID_MAIN_PrintTemperatureListStandbySubItem);
}

static void onProgressChanged_Slider(ZKSeekBar *pSeekBar, int progress) {
	UI::SLIDER_WINDOW->Callback();
}

static bool onButtonClick_SliderCloseBtn(ZKButton *pButton) {
	UI::WINDOW->CloseOverlay();
	return false;
}
static bool onButtonClick_PopupCancelBtn(ZKButton* pButton)
{
	UI::POPUP_WINDOW->Cancel();
	return false;
}

static bool onButtonClick_PopupOkBtn(ZKButton* pButton)
{
	UI::POPUP_WINDOW->Ok();
	return false;
}

static void onSlideItemClick_SettingsSlideWindow(ZKSlideWindow* pSlideWindow, int index)
{
	switch (index)
	{
	case (int)UI::SettingsSlideWindowIndex::language:
		EASYUICONTEXT->openActivity("LanguageSettingActivity");
		break;
	case (int)UI::SettingsSlideWindowIndex::duet:
		mDuetUartCommSettingWindowPtr->setVisible(Comm::duet.GetCommunicationType() ==
												  Comm::Duet::CommunicationType::uart);
		mDuetNetworkCommSettingWindowPtr->setVisible(Comm::duet.GetCommunicationType() ==
													 Comm::Duet::CommunicationType::network);
		UI::WINDOW->OpenOverlay(mDuetCommSettingWindowPtr);
		break;
	case (int)UI::SettingsSlideWindowIndex::update:
		EASYUICONTEXT->openActivity("UpgradeActivity");
		break;
	case (int)UI::SettingsSlideWindowIndex::dev:
		EASYUICONTEXT->openActivity("DeveloperSettingActivity");
		break;
	case (int)UI::SettingsSlideWindowIndex::power_off:
		EASYUICONTEXT->openActivity("PowerOffActivity");
		break;
	case (int)UI::SettingsSlideWindowIndex::zk_setting:
		EASYUICONTEXT->openActivity("ZKSettingActivity");
		break;
	case (int)UI::SettingsSlideWindowIndex::touch_calibration:
		EASYUICONTEXT->openActivity("TouchCalibrationActivity");
		break;
	default:
		break;
	}
}

static int getListItemCount_BaudRateList(const ZKListView* pListView)
{
	return ARRAY_SIZE(Comm::baudRates);
}

static void obtainListItemData_BaudRateList(ZKListView* pListView, ZKListView::ZKListItem* pListItem, int index)
{
	pListItem->setSelected(Comm::baudRates[index].rate == Comm::duet.GetBaudRate().rate);
	pListItem->setText((int)Comm::baudRates[index].rate);
}

static void onListItemClick_BaudRateList(ZKListView* pListView, int index, int id)
{
	Comm::duet.SetBaudRate(Comm::baudRates[index]);
}
static int getListItemCount_PopupSelectionList(const ZKListView* pListView)
{
	// LOGD("getListItemCount_PopupSelectionList !\n");
	return OM::currentAlert.choices_count;
}

static void obtainListItemData_PopupSelectionList(ZKListView* pListView, ZKListView::ZKListItem* pListItem, int index)
{
	pListItem->setText(OM::currentAlert.choices[index].c_str());
}

static void onListItemClick_PopupSelectionList(ZKListView* pListView, int index, int id)
{
	Comm::duet.SendGcodef("M292 R{%lu} S%lu\n", index, OM::currentAlert.seq);
	// LOGD(" onListItemClick_ PopupSelectionList  !!!\n");
}

static void onEditTextChanged_PopupTextInput(const std::string& text)
{
	if (UI::POPUP_WINDOW->GetMode() != OM::Alert::Mode::Text)
		return;
	UI::POPUP_WINDOW->ValidateTextInput(text.c_str());
}

static void onEditTextChanged_PopupNumberInput(const std::string& text)
{
	switch (UI::POPUP_WINDOW->GetMode())
	{
	case OM::Alert::Mode::NumberInt: {
		UI::POPUP_WINDOW->ValidateIntegerInput(text.c_str());
		break;
	}
	case OM::Alert::Mode::NumberFloat: {
		UI::POPUP_WINDOW->ValidateFloatInput(text.c_str());
		break;
	}
	default:
		break;
	}
}

static int getListItemCount_PopupAxisSelection(const ZKListView* pListView)
{
	return UI::POPUP_WINDOW->GetJogAxisCount();
}

static void obtainListItemData_PopupAxisSelection(ZKListView* pListView, ZKListView::ZKListItem* pListItem, int index)
{
	pListItem->setSelected(index == UI::POPUP_WINDOW->selectedAxis);
	pListItem->setText(UI::POPUP_WINDOW->GetJogAxis(index)->letter);
}

static void onListItemClick_PopupAxisSelection(ZKListView* pListView, int index, int id)
{
	dbg("Popup axis selection %d", index);
	UI::POPUP_WINDOW->selectedAxis = index;
}

static int getListItemCount_PopupAxisAdjusment(const ZKListView* pListView)
{
	// LOGD("getListItemCount_PopupAxisAdjusment !\n");
	return ARRAY_SIZE(UI::POPUP_WINDOW->jogAmounts);
}

static void obtainListItemData_PopupAxisAdjusment(ZKListView* pListView, ZKListView::ZKListItem* pListItem, int index)
{
	pListItem->setText(UI::POPUP_WINDOW->jogAmounts[index]);
}

static void onListItemClick_PopupAxisAdjusment(ZKListView* pListView, int index, int id)
{
	Comm::duet.SendGcode("M120\n"); // Push
	Comm::duet.SendGcode("G91\n");	// Relative move
	Comm::duet.SendGcodef("G1 %s%.3f F%d\n",
						 UI::POPUP_WINDOW->GetJogAxis(UI::POPUP_WINDOW->selectedAxis)->letter,
						 UI::POPUP_WINDOW->jogAmounts[index],
						 300);
	Comm::duet.SendGcode("M121\n"); // Pop
}

static int getListItemCount_DuetCommList(const ZKListView* pListView)
{
	// LOGD("getListItemCount_DuetCommList !\n");
	return (int)Comm::Duet::CommunicationType::COUNT;
}

static void obtainListItemData_DuetCommList(ZKListView* pListView, ZKListView::ZKListItem* pListItem, int index)
{
	pListItem->setText(Comm::duetCommunicationTypeNames[index]);
	pListItem->setSelected(index == (int)Comm::duet.GetCommunicationType());
}

static void onListItemClick_DuetCommList(ZKListView* pListView, int index, int id)
{
	Comm::duet.SetCommunicationType((Comm::Duet::CommunicationType)index);
	mDuetUartCommSettingWindowPtr->setVisible(Comm::duet.GetCommunicationType() == Comm::Duet::CommunicationType::uart);
	mDuetNetworkCommSettingWindowPtr->setVisible(Comm::duet.GetCommunicationType() ==
												 Comm::Duet::CommunicationType::network);
	mCommunicationTypePtr->setText(Comm::duetCommunicationTypeNames[index]);
}

static void onEditTextChanged_PollIntervalInput(const std::string& text)
{
	if (text.empty() || atoi(text.c_str()) < (int)Comm::minPrinterPollInterval)
	{
		mPollIntervalInputPtr->setText((int)Comm::minPrinterPollInterval);
		return;
	}
	Comm::duet.SetPollInterval(atoi(text.c_str()));
}

static void onEditTextChanged_IpAddressInput(const std::string& text)
{
	Comm::duet.SetIPAddress(text);
}

static void onEditTextChanged_HostnameInput(const std::string& text)
{
	Comm::duet.SetHostname(text);
}

static void onEditTextChanged_PasswordInput(const std::string& text)
{
	Comm::duet.SetPassword(text);
}

static void onEditTextChanged_InfoTimeoutInput(const std::string& text)
{
	int32_t timeout = -1;
	if (text.empty() || !Comm::GetInteger(text.c_str(), timeout) || timeout < 0)
	{
		mInfoTimeoutInputPtr->setText((int)UI::POPUP_WINDOW->GetTimeout());
		return;
	}
	UI::POPUP_WINDOW->SetTimeout((uint32_t)timeout);
}

static bool onButtonClick_UsbFiles(ZKButton* pButton)
{
	dbg(" ButtonClick UsbFiles !!!\n");
	OM::FileSystem::RequestUsbFiles("");
	return false;
}
