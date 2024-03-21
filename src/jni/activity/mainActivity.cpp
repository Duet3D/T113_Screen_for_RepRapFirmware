/***********************************************
/gen auto by zuitools
***********************************************/
#include "mainActivity.h"

/*TAG:GlobalVariable全局变量*/
static ZKSeekBar* mPopupProgressPtr;
static ZKListView* mTempGraphYLabelsPtr;
static ZKListView* mTempGraphXLabelsPtr;
static ZKTextView* mGraphYLabelBottomPtr;
static ZKTextView* mGraphYLabelMidPtr;
static ZKTextView* mGraphYLabelTopPtr;
static ZKTextView* mGraphXLabelRightPtr;
static ZKTextView* mGraphXLabelMidPtr;
static ZKTextView* mGraphXLabelLeftPtr;
static ZKTextView* mTextView44Ptr;
static ZKTextView* mFileListInfoPtr;
static ZKButton* mOverlayModalZonePtr;
static ZKTextView* mTextView43Ptr;
static ZKListView* mDebugCommandListPtr;
static ZKWindow* mDebugWindowPtr;
static ZKTextView* mTextView42Ptr;
static ZKListView* mThemesListPtr;
static ZKWindow* mThemeSelectionWindowPtr;
static ZKTextView* mTextView41Ptr;
static ZKSlideWindow* mSlideWindow2Ptr;
static ZKListView* mListView1Ptr;
static ZKCheckBox* mCheckbox1Ptr;
static ZKDigitalClock* mDigitalClock2Ptr;
static ZKPointer* mPointer1Ptr;
static ZKCircleBar* mCircleBar1Ptr;
static ZKSeekBar* mSeekBar1Ptr;
static ZKEditText* mEditText2Ptr;
static ZKButton* mButton1Ptr;
static ZKTextView* mTextView40Ptr;
static ZKWindow* mWindow1Ptr;
static ZKTextView* mCommunicationTypePtr;
static ZKDigitalClock* mDigitalClock1Ptr;
static ZKButton* mEStopBtnPtr;
static ZKTextView* mTextView26Ptr;
static ZKWindow* mPageOverlayExampleWindowPtr;
static ZKTextView* mTextView24Ptr;
static ZKCheckBox* mShowSetupOnStartupPtr;
static ZKWindow* mSetupGuideWindowPtr;
static ZKTextView* mGuidePageNumPtr;
static ZKButton* mCloseGuideBtnPtr;
static ZKButton* mPreviousPageBtnPtr;
static ZKButton* mNextPageBtnPtr;
static ZKWindow* mGuidedSetupWindowPtr;
static ZKTextView* mPopupImagePtr;
static ZKListView* mPopupAxisAdjusmentPtr;
static ZKListView* mPopupAxisSelectionPtr;
static ZKTextView* mPopupWarningPtr;
static ZKTextView* mPopupMaxPtr;
static ZKTextView* mPopupMinPtr;
static ZKEditText* mPopupNumberInputPtr;
static ZKEditText* mPopupTextInputPtr;
static ZKListView* mPopupSelectionListPtr;
static ZKButton* mPopupOkBtnPtr;
static ZKButton* mPopupCancelBtnPtr;
static ZKTextView* mPopupTextPtr;
static ZKTextView* mPopupTitlePtr;
static ZKWindow* mPopupWindowPtr;
static ZKButton* mNumPadClearBtnPtr;
static ZKButton* mNumPadCloseBtnPtr;
static ZKTextView* mNumPadHeaderPtr;
static ZKTextView* mNumPadInputPtr;
static ZKButton* mNumPadConfirmPtr;
static ZKButton* mNumPad0Ptr;
static ZKButton* mNumPadDelPtr;
static ZKButton* mNumPad9Ptr;
static ZKButton* mNumPad8Ptr;
static ZKButton* mNumPad7Ptr;
static ZKButton* mNumPad6Ptr;
static ZKButton* mNumPad5Ptr;
static ZKButton* mNumPad4Ptr;
static ZKButton* mNumPad3Ptr;
static ZKButton* mNumPad2Ptr;
static ZKButton* mNumPad1Ptr;
static ZKWindow* mNumPadWindowPtr;
static ZKWindow* mNoTouchWindowPtr;
static ZKTextView* mSliderValuePtr;
static ZKButton* mSliderCloseBtnPtr;
static ZKSeekBar* mSliderPtr;
static ZKTextView* mSliderSuffixPtr;
static ZKTextView* mSliderPrefixPtr;
static ZKTextView* mSliderHeaderPtr;
static ZKWindow* mSliderWindowPtr;
static ZKTextView* mTextView13Ptr;
static ZKTextView* mTextView12Ptr;
static ZKTextView* mTextView11Ptr;
static ZKTextView* mTextView10Ptr;
static ZKTextView* mTextView9Ptr;
static ZKListView* mPrintTemperatureListPtr;
static ZKWindow* mPrintTemperatureWindowPtr;
static ZKTextView* mTextView15Ptr;
static ZKTextView* mPrintSpeedFactorPtr;
static ZKTextView* mPrintVolFlowPtr;
static ZKTextView* mPrintTopSpeedPtr;
static ZKTextView* mPrintRequestedSpeedPtr;
static ZKTextView* mTextView8Ptr;
static ZKSeekBar* mPrintSpeedMultiplierBarPtr;
static ZKTextView* mTextView7Ptr;
static ZKListView* mPrintExtruderPositionListPtr;
static ZKTextView* mTextView6Ptr;
static ZKListView* mPrintPositionListPtr;
static ZKWindow* mPrintPositionWindowPtr;
static ZKButton* mPrintResumeBtnPtr;
static ZKButton* mPrintCancelBtnPtr;
static ZKButton* mPrintPauseBtnPtr;
static ZKWindow* mPrintPauseWindowPtr;
static ZKTextView* mTextView14Ptr;
static ZKListView* mPrintFanListPtr;
static ZKWindow* mPrintFansWindowPtr;
static ZKButton* mPrintBabystepIncBtnPtr;
static ZKButton* mPrintBabystepDecBtnPtr;
static ZKTextView* mPrintBabystepCurrentOffsetPtr;
static ZKTextView* mTextView5Ptr;
static ZKWindow* mPrintBabystepWindowPtr;
static ZKTextView* mPrintEstimatedTimePtr;
static ZKTextView* mPrintElapsedTimePtr;
static ZKCircleBar* mPrintProgressBarPtr;
static ZKTextView* mPrintFileNamePtr;
static ZKWindow* mPrintJobInfoWindowPtr;
static ZKWindow* mPrintWindowPtr;
static ZKTextView* mTextView39Ptr;
static ZKTextView* mTextView38Ptr;
static ZKEditText* mScreensaverTimeoutInputPtr;
static ZKCheckBox* mScreensaverEnablePtr;
static ZKTextView* mTextView37Ptr;
static ZKWindow* mScreensaverSettingWindowPtr;
static ZKTextView* mTextView25Ptr;
static ZKListView* mGuidesListPtr;
static ZKWindow* mGuideSelectionWindowPtr;
static ZKTextView* mTextView23Ptr;
static ZKEditText* mInfoTimeoutInputPtr;
static ZKTextView* mTextView22Ptr;
static ZKEditText* mPollIntervalInputPtr;
static ZKTextView* mTextView20Ptr;
static ZKListView* mBaudRateListPtr;
static ZKWindow* mDuetUartCommSettingWindowPtr;
static ZKEditText* mPasswordInputPtr;
static ZKTextView* mTextView21Ptr;
static ZKEditText* mHostnameInputPtr;
static ZKTextView* mTextView19Ptr;
static ZKTextView* mTextView18Ptr;
static ZKEditText* mIpAddressInputPtr;
static ZKWindow* mDuetNetworkCommSettingWindowPtr;
static ZKListView* mDuetCommListPtr;
static ZKWindow* mDuetCommSettingWindowPtr;
static ZKSlideWindow* mSettingsSlideWindowPtr;
static ZKWindow* mSettingsWindowPtr;
static ZKWindow* mNetworkWindowPtr;
static ZKButton* mUsbFilesPtr;
static ZKTextView* mFolderIDPtr;
static ZKButton* mFileRefreshBtnPtr;
static ZKListView* mFileListViewPtr;
static ZKWindow* mFilesWindowPtr;
static ZKWindow* mFanWindowPtr;
static ZKWindow* mHeightMapWindowPtr;
static ZKButton* mConsoleMacroBtn3Ptr;
static ZKButton* mConsoleMacroBtn2Ptr;
static ZKButton* mConsoleMacroBtn1Ptr;
static ZKButton* mConsoleClearBtnPtr;
static ZKButton* mSendBtnPtr;
static ZKTextView* mTextView4Ptr;
static ZKTextView* mTextView3Ptr;
static ZKEditText* mEditText1Ptr;
static ZKListView* mGcodeListViewPtr;
static ZKListView* mConsoleListViewPtr;
static ZKWindow* mConsoleWindowPtr;
static ZKTextView* mFilamentControlHeadingPtr;
static ZKButton* mUnloadFilamentBtnPtr;
static ZKTextView* mTextView36Ptr;
static ZKListView* mFilamentListPtr;
static ZKWindow* mFilamentLoadUnloadWindowPtr;
static ZKButton* mExtrudeBtnPtr;
static ZKButton* mRetractBtnPtr;
static ZKTextView* mTextView33Ptr;
static ZKTextView* mTextView32Ptr;
static ZKTextView* mTextView31Ptr;
static ZKTextView* mTextView34Ptr;
static ZKTextView* mTextView35Ptr;
static ZKTextView* mTextView30Ptr;
static ZKListView* mExtrudeToolListPtr;
static ZKTextView* mTextView29Ptr;
static ZKTextView* mTextView28Ptr;
static ZKTextView* mTextView27Ptr;
static ZKListView* mExtruderFeedratePtr;
static ZKListView* mExtruderFeedDistPtr;
static ZKWindow* mExtrudeWindowPtr;
static ZKButton* mDisableMotorsBtnPtr;
static ZKTextView* mTextView17Ptr;
static ZKTextView* mTextView16Ptr;
static ZKButton* mFeedrateBtn5Ptr;
static ZKButton* mFeedrateBtn4Ptr;
static ZKButton* mFeedrateBtn3Ptr;
static ZKButton* mFeedrateBtn2Ptr;
static ZKTextView* mTextView2Ptr;
static ZKButton* mHeightmapBtnPtr;
static ZKButton* mMeshLevelBtnPtr;
static ZKButton* mTrueLevelBtnPtr;
static ZKButton* mFeedrateBtn1Ptr;
static ZKListView* mAxisControlListViewPtr;
static ZKButton* mHomeAllBtnPtr;
static ZKWindow* mMoveWindowPtr;
static ZKSlideWindow* mSlideWindow1Ptr;
static ZKWindow* mWindowSelectWindowPtr;
static ZKListView* mTemperatureGraphLegendPtr;
static ZKDiagram* mTempGraphPtr;
static ZKWindow* mTemperatureGraphWindowPtr;
static ZKTextView* mToolListHeadingStandbyPtr;
static ZKTextView* mToolListHeadingActivePtr;
static ZKTextView* mToolListHeadingCurrentPtr;
static ZKTextView* mToolListHeadingStatusPtr;
static ZKTextView* mToolListHeadingNamePtr;
static ZKListView* mToolListViewPtr;
static ZKWindow* mToolListWindowPtr;
static ZKWindow* mMainWindowPtr;
static ZKTextView* mStatusTextPtr;
static ZKButton* mConsoleBtnPtr;
static ZKButton* mMacroBtnPtr;
static ZKButton* mBackBtnPtr;
static ZKButton* mHomeBtnPtr;
static ZKWindow* mSideBarWindowPtr;
static ZKWindow* mRootWindowPtr;
static mainActivity* mActivityPtr;

/*register activity*/
REGISTER_ACTIVITY(mainActivity);

typedef struct {
	int id; // 定时器ID ， 不能重复
	int time; // 定时器  时间间隔  单位 毫秒
}S_ACTIVITY_TIMEER;

#include "logic/mainLogic.cc"

/***********/
typedef struct {
    int id;
    const char *pApp;
} SAppInfo;

/**
 *点击跳转window
 */
static SAppInfo sAppInfoTab[] = {
//  { ID_MAIN_TEXT, "TextViewActivity" },
};

/***************/
typedef bool (*ButtonCallback)(ZKButton *pButton);
/**
 * button onclick表
 */
typedef struct {
    int id;
    ButtonCallback callback;
}S_ButtonCallback;

/*TAG:ButtonCallbackTab按键映射表*/
static S_ButtonCallback sButtonCallbackTab[] = {
    ID_MAIN_OverlayModalZone, onButtonClick_OverlayModalZone,
    ID_MAIN_Button1, onButtonClick_Button1,
    ID_MAIN_EStopBtn, onButtonClick_EStopBtn,
    ID_MAIN_CloseGuideBtn, onButtonClick_CloseGuideBtn,
    ID_MAIN_PreviousPageBtn, onButtonClick_PreviousPageBtn,
    ID_MAIN_NextPageBtn, onButtonClick_NextPageBtn,
    ID_MAIN_PopupOkBtn, onButtonClick_PopupOkBtn,
    ID_MAIN_PopupCancelBtn, onButtonClick_PopupCancelBtn,
    ID_MAIN_NumPadClearBtn, onButtonClick_NumPadClearBtn,
    ID_MAIN_NumPadCloseBtn, onButtonClick_NumPadCloseBtn,
    ID_MAIN_NumPadConfirm, onButtonClick_NumPadConfirm,
    ID_MAIN_NumPad0, onButtonClick_NumPad0,
    ID_MAIN_NumPadDel, onButtonClick_NumPadDel,
    ID_MAIN_NumPad9, onButtonClick_NumPad9,
    ID_MAIN_NumPad8, onButtonClick_NumPad8,
    ID_MAIN_NumPad7, onButtonClick_NumPad7,
    ID_MAIN_NumPad6, onButtonClick_NumPad6,
    ID_MAIN_NumPad5, onButtonClick_NumPad5,
    ID_MAIN_NumPad4, onButtonClick_NumPad4,
    ID_MAIN_NumPad3, onButtonClick_NumPad3,
    ID_MAIN_NumPad2, onButtonClick_NumPad2,
    ID_MAIN_NumPad1, onButtonClick_NumPad1,
    ID_MAIN_SliderCloseBtn, onButtonClick_SliderCloseBtn,
    ID_MAIN_PrintResumeBtn, onButtonClick_PrintResumeBtn,
    ID_MAIN_PrintCancelBtn, onButtonClick_PrintCancelBtn,
    ID_MAIN_PrintPauseBtn, onButtonClick_PrintPauseBtn,
    ID_MAIN_PrintBabystepIncBtn, onButtonClick_PrintBabystepIncBtn,
    ID_MAIN_PrintBabystepDecBtn, onButtonClick_PrintBabystepDecBtn,
    ID_MAIN_UsbFiles, onButtonClick_UsbFiles,
    ID_MAIN_FileRefreshBtn, onButtonClick_FileRefreshBtn,
    ID_MAIN_ConsoleMacroBtn3, onButtonClick_ConsoleMacroBtn3,
    ID_MAIN_ConsoleMacroBtn2, onButtonClick_ConsoleMacroBtn2,
    ID_MAIN_ConsoleMacroBtn1, onButtonClick_ConsoleMacroBtn1,
    ID_MAIN_ConsoleClearBtn, onButtonClick_ConsoleClearBtn,
    ID_MAIN_SendBtn, onButtonClick_SendBtn,
    ID_MAIN_UnloadFilamentBtn, onButtonClick_UnloadFilamentBtn,
    ID_MAIN_ExtrudeBtn, onButtonClick_ExtrudeBtn,
    ID_MAIN_RetractBtn, onButtonClick_RetractBtn,
    ID_MAIN_DisableMotorsBtn, onButtonClick_DisableMotorsBtn,
    ID_MAIN_FeedrateBtn5, onButtonClick_FeedrateBtn5,
    ID_MAIN_FeedrateBtn4, onButtonClick_FeedrateBtn4,
    ID_MAIN_FeedrateBtn3, onButtonClick_FeedrateBtn3,
    ID_MAIN_FeedrateBtn2, onButtonClick_FeedrateBtn2,
    ID_MAIN_HeightmapBtn, onButtonClick_HeightmapBtn,
    ID_MAIN_MeshLevelBtn, onButtonClick_MeshLevelBtn,
    ID_MAIN_TrueLevelBtn, onButtonClick_TrueLevelBtn,
    ID_MAIN_FeedrateBtn1, onButtonClick_FeedrateBtn1,
    ID_MAIN_HomeAllBtn, onButtonClick_HomeAllBtn,
    ID_MAIN_ConsoleBtn, onButtonClick_ConsoleBtn,
    ID_MAIN_MacroBtn, onButtonClick_MacroBtn,
    ID_MAIN_BackBtn, onButtonClick_BackBtn,
    ID_MAIN_HomeBtn, onButtonClick_HomeBtn,
};
/***************/


typedef void (*SeekBarCallback)(ZKSeekBar *pSeekBar, int progress);
typedef struct {
    int id;
    SeekBarCallback callback;
}S_ZKSeekBarCallback;
/*TAG:SeekBarCallbackTab*/
static S_ZKSeekBarCallback SZKSeekBarCallbackTab[] = {
    ID_MAIN_PopupProgress, onProgressChanged_PopupProgress,
    ID_MAIN_SeekBar1, onProgressChanged_SeekBar1,
    ID_MAIN_Slider, onProgressChanged_Slider,
    ID_MAIN_PrintSpeedMultiplierBar, onProgressChanged_PrintSpeedMultiplierBar,
};


typedef int (*ListViewGetItemCountCallback)(const ZKListView *pListView);
typedef void (*ListViewobtainListItemDataCallback)(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index);
typedef void (*ListViewonItemClickCallback)(ZKListView *pListView, int index, int id);
typedef struct {
    int id;
    ListViewGetItemCountCallback getListItemCountCallback;
    ListViewobtainListItemDataCallback obtainListItemDataCallback;
    ListViewonItemClickCallback onItemClickCallback;
}S_ListViewFunctionsCallback;
/*TAG:ListViewFunctionsCallback*/
static S_ListViewFunctionsCallback SListViewFunctionsCallbackTab[] = {
    ID_MAIN_ListView1, getListItemCount_ListView1, obtainListItemData_ListView1, onListItemClick_ListView1,
    ID_MAIN_PopupAxisAdjusment, getListItemCount_PopupAxisAdjusment, obtainListItemData_PopupAxisAdjusment, onListItemClick_PopupAxisAdjusment,
    ID_MAIN_PopupAxisSelection, getListItemCount_PopupAxisSelection, obtainListItemData_PopupAxisSelection, onListItemClick_PopupAxisSelection,
    ID_MAIN_PopupSelectionList, getListItemCount_PopupSelectionList, obtainListItemData_PopupSelectionList, onListItemClick_PopupSelectionList,
    ID_MAIN_ThemesList, getListItemCount_ThemesList, obtainListItemData_ThemesList, onListItemClick_ThemesList,
    ID_MAIN_GuidesList, getListItemCount_GuidesList, obtainListItemData_GuidesList, onListItemClick_GuidesList,
    ID_MAIN_BaudRateList, getListItemCount_BaudRateList, obtainListItemData_BaudRateList, onListItemClick_BaudRateList,
    ID_MAIN_DuetCommList, getListItemCount_DuetCommList, obtainListItemData_DuetCommList, onListItemClick_DuetCommList,
    ID_MAIN_PrintTemperatureList, getListItemCount_PrintTemperatureList, obtainListItemData_PrintTemperatureList, onListItemClick_PrintTemperatureList,
    ID_MAIN_PrintExtruderPositionList, getListItemCount_PrintExtruderPositionList, obtainListItemData_PrintExtruderPositionList, onListItemClick_PrintExtruderPositionList,
    ID_MAIN_PrintPositionList, getListItemCount_PrintPositionList, obtainListItemData_PrintPositionList, onListItemClick_PrintPositionList,
    ID_MAIN_PrintFanList, getListItemCount_PrintFanList, obtainListItemData_PrintFanList, onListItemClick_PrintFanList,
    ID_MAIN_FileListView, getListItemCount_FileListView, obtainListItemData_FileListView, onListItemClick_FileListView,
    ID_MAIN_DebugCommandList, getListItemCount_DebugCommandList, obtainListItemData_DebugCommandList, onListItemClick_DebugCommandList,
    ID_MAIN_GcodeListView, getListItemCount_GcodeListView, obtainListItemData_GcodeListView, onListItemClick_GcodeListView,
    ID_MAIN_ConsoleListView, getListItemCount_ConsoleListView, obtainListItemData_ConsoleListView, onListItemClick_ConsoleListView,
    ID_MAIN_FilamentList, getListItemCount_FilamentList, obtainListItemData_FilamentList, onListItemClick_FilamentList,
    ID_MAIN_ExtrudeToolList, getListItemCount_ExtrudeToolList, obtainListItemData_ExtrudeToolList, onListItemClick_ExtrudeToolList,
    ID_MAIN_ExtruderFeedrate, getListItemCount_ExtruderFeedrate, obtainListItemData_ExtruderFeedrate, onListItemClick_ExtruderFeedrate,
    ID_MAIN_ExtruderFeedDist, getListItemCount_ExtruderFeedDist, obtainListItemData_ExtruderFeedDist, onListItemClick_ExtruderFeedDist,
    ID_MAIN_AxisControlListView, getListItemCount_AxisControlListView, obtainListItemData_AxisControlListView, onListItemClick_AxisControlListView,
    ID_MAIN_TempGraphYLabels, getListItemCount_TempGraphYLabels, obtainListItemData_TempGraphYLabels, onListItemClick_TempGraphYLabels,
    ID_MAIN_TempGraphXLabels, getListItemCount_TempGraphXLabels, obtainListItemData_TempGraphXLabels, onListItemClick_TempGraphXLabels,
    ID_MAIN_TemperatureGraphLegend, getListItemCount_TemperatureGraphLegend, obtainListItemData_TemperatureGraphLegend, onListItemClick_TemperatureGraphLegend,
    ID_MAIN_ToolListView, getListItemCount_ToolListView, obtainListItemData_ToolListView, onListItemClick_ToolListView,
	ID_MAIN_TempGraphYLabels,
	getListItemCount_TempGraphYLabels,
	obtainListItemData_TempGraphYLabels,
	onListItemClick_TempGraphYLabels,
	ID_MAIN_TempGraphXLabels,
	getListItemCount_TempGraphXLabels,
	obtainListItemData_TempGraphXLabels,
	onListItemClick_TempGraphXLabels,
	ID_MAIN_DebugCommandList,
	getListItemCount_DebugCommandList,
	obtainListItemData_DebugCommandList,
	onListItemClick_DebugCommandList,
	ID_MAIN_ThemesList,
	getListItemCount_ThemesList,
	obtainListItemData_ThemesList,
	onListItemClick_ThemesList,
	ID_MAIN_ListView1,
	getListItemCount_ListView1,
	obtainListItemData_ListView1,
	onListItemClick_ListView1,
	ID_MAIN_PopupAxisAdjusment,
	getListItemCount_PopupAxisAdjusment,
	obtainListItemData_PopupAxisAdjusment,
	onListItemClick_PopupAxisAdjusment,
	ID_MAIN_PopupAxisSelection,
	getListItemCount_PopupAxisSelection,
	obtainListItemData_PopupAxisSelection,
	onListItemClick_PopupAxisSelection,
	ID_MAIN_PopupSelectionList,
	getListItemCount_PopupSelectionList,
	obtainListItemData_PopupSelectionList,
	onListItemClick_PopupSelectionList,
	ID_MAIN_PrintTemperatureList,
	getListItemCount_PrintTemperatureList,
	obtainListItemData_PrintTemperatureList,
	onListItemClick_PrintTemperatureList,
	ID_MAIN_PrintExtruderPositionList,
	getListItemCount_PrintExtruderPositionList,
	obtainListItemData_PrintExtruderPositionList,
	onListItemClick_PrintExtruderPositionList,
	ID_MAIN_PrintPositionList,
	getListItemCount_PrintPositionList,
	obtainListItemData_PrintPositionList,
	onListItemClick_PrintPositionList,
	ID_MAIN_PrintFanList,
	getListItemCount_PrintFanList,
	obtainListItemData_PrintFanList,
	onListItemClick_PrintFanList,
	ID_MAIN_GuidesList,
	getListItemCount_GuidesList,
	obtainListItemData_GuidesList,
	onListItemClick_GuidesList,
	ID_MAIN_BaudRateList,
	getListItemCount_BaudRateList,
	obtainListItemData_BaudRateList,
	onListItemClick_BaudRateList,
	ID_MAIN_DuetCommList,
	getListItemCount_DuetCommList,
	obtainListItemData_DuetCommList,
	onListItemClick_DuetCommList,
	ID_MAIN_FileListView,
	getListItemCount_FileListView,
	obtainListItemData_FileListView,
	onListItemClick_FileListView,
	ID_MAIN_GcodeListView,
	getListItemCount_GcodeListView,
	obtainListItemData_GcodeListView,
	onListItemClick_GcodeListView,
	ID_MAIN_ConsoleListView,
	getListItemCount_ConsoleListView,
	obtainListItemData_ConsoleListView,
	onListItemClick_ConsoleListView,
	ID_MAIN_FilamentList,
	getListItemCount_FilamentList,
	obtainListItemData_FilamentList,
	onListItemClick_FilamentList,
	ID_MAIN_ExtrudeToolList,
	getListItemCount_ExtrudeToolList,
	obtainListItemData_ExtrudeToolList,
	onListItemClick_ExtrudeToolList,
	ID_MAIN_ExtruderFeedrate,
	getListItemCount_ExtruderFeedrate,
	obtainListItemData_ExtruderFeedrate,
	onListItemClick_ExtruderFeedrate,
	ID_MAIN_ExtruderFeedDist,
	getListItemCount_ExtruderFeedDist,
	obtainListItemData_ExtruderFeedDist,
	onListItemClick_ExtruderFeedDist,
	ID_MAIN_AxisControlListView,
	getListItemCount_AxisControlListView,
	obtainListItemData_AxisControlListView,
	onListItemClick_AxisControlListView,
	ID_MAIN_TemperatureGraphLegend,
	getListItemCount_TemperatureGraphLegend,
	obtainListItemData_TemperatureGraphLegend,
	onListItemClick_TemperatureGraphLegend,
	ID_MAIN_ToolListView,
	getListItemCount_ToolListView,
	obtainListItemData_ToolListView,
	onListItemClick_ToolListView,
};

typedef void (*SlideWindowItemClickCallback)(ZKSlideWindow *pSlideWindow, int index);
typedef struct {
    int id;
    SlideWindowItemClickCallback onSlideItemClickCallback;
}S_SlideWindowItemClickCallback;
/*TAG:SlideWindowFunctionsCallbackTab*/
static S_SlideWindowItemClickCallback SSlideWindowItemClickCallbackTab[] = {
    ID_MAIN_SlideWindow2, onSlideItemClick_SlideWindow2,
    ID_MAIN_SettingsSlideWindow, onSlideItemClick_SettingsSlideWindow,
    ID_MAIN_SlideWindow1, onSlideItemClick_SlideWindow1,
};


typedef void (*EditTextInputCallback)(const std::string &text);
typedef struct {
    int id;
    EditTextInputCallback onEditTextChangedCallback;
}S_EditTextInputCallback;
/*TAG:EditTextInputCallback*/
static S_EditTextInputCallback SEditTextInputCallbackTab[] = {
    ID_MAIN_EditText2, onEditTextChanged_EditText2,
    ID_MAIN_PopupNumberInput, onEditTextChanged_PopupNumberInput,
    ID_MAIN_PopupTextInput, onEditTextChanged_PopupTextInput,
    ID_MAIN_ScreensaverTimeoutInput, onEditTextChanged_ScreensaverTimeoutInput,
    ID_MAIN_InfoTimeoutInput, onEditTextChanged_InfoTimeoutInput,
    ID_MAIN_PollIntervalInput, onEditTextChanged_PollIntervalInput,
    ID_MAIN_PasswordInput, onEditTextChanged_PasswordInput,
    ID_MAIN_HostnameInput, onEditTextChanged_HostnameInput,
    ID_MAIN_IpAddressInput, onEditTextChanged_IpAddressInput,
    ID_MAIN_EditText1, onEditTextChanged_EditText1,
};

typedef void (*VideoViewCallback)(ZKVideoView *pVideoView, int msg);
typedef struct {
    int id; //VideoView ID
    bool loop; // 是否是轮播类型
    int defaultvolume;//轮播类型时,默认视频音量
    VideoViewCallback onVideoViewCallback;
}S_VideoViewCallback;
/*TAG:VideoViewCallback*/
static S_VideoViewCallback SVideoViewCallbackTab[] = {
};


typedef void (*CheckboxCallback)(ZKCheckBox*, bool);
typedef struct {
  int id;
  CheckboxCallback onCheckedChanged;
}S_CheckboxCallback;
/*TAG:CheckboxCallbackTab*/
static S_CheckboxCallback SCheckboxCallbackTab[] = {
    ID_MAIN_Checkbox1, onCheckedChanged_Checkbox1,
    ID_MAIN_ShowSetupOnStartup, onCheckedChanged_ShowSetupOnStartup,
    ID_MAIN_ScreensaverEnable, onCheckedChanged_ScreensaverEnable,
};

mainActivity::mainActivity() {
	//todo add init code here
	mVideoLoopIndex = -1;
	mVideoLoopErrorCount = 0;
}

mainActivity::~mainActivity() {
  //todo add init file here
  // 退出应用时需要反注册
    EASYUICONTEXT->unregisterGlobalTouchListener(this);
    unregisterProtocolDataUpdateListener(onProtocolDataUpdate);
    onUI_quit();
    mActivityPtr = NULL;
    mPopupProgressPtr = NULL;
	mTempGraphYLabelsPtr = NULL;
	mTempGraphXLabelsPtr = NULL;
	mGraphYLabelBottomPtr = NULL;
	mGraphYLabelMidPtr = NULL;
	mGraphYLabelTopPtr = NULL;
	mGraphXLabelRightPtr = NULL;
	mGraphXLabelMidPtr = NULL;
	mGraphXLabelLeftPtr = NULL;
	mTextView44Ptr = NULL;
	mFileListInfoPtr = NULL;
    mOverlayModalZonePtr = NULL;
    mTextView43Ptr = NULL;
    mDebugCommandListPtr = NULL;
    mDebugWindowPtr = NULL;
    mTextView42Ptr = NULL;
    mThemesListPtr = NULL;
    mThemeSelectionWindowPtr = NULL;
    mTextView41Ptr = NULL;
    mSlideWindow2Ptr = NULL;
    mListView1Ptr = NULL;
    mCheckbox1Ptr = NULL;
    mDigitalClock2Ptr = NULL;
    mPointer1Ptr = NULL;
    mCircleBar1Ptr = NULL;
    mSeekBar1Ptr = NULL;
    mEditText2Ptr = NULL;
    mButton1Ptr = NULL;
    mTextView40Ptr = NULL;
    mWindow1Ptr = NULL;
    mCommunicationTypePtr = NULL;
    mDigitalClock1Ptr = NULL;
    mEStopBtnPtr = NULL;
    mTextView26Ptr = NULL;
    mPageOverlayExampleWindowPtr = NULL;
    mTextView24Ptr = NULL;
    mShowSetupOnStartupPtr = NULL;
    mSetupGuideWindowPtr = NULL;
    mGuidePageNumPtr = NULL;
    mCloseGuideBtnPtr = NULL;
    mPreviousPageBtnPtr = NULL;
    mNextPageBtnPtr = NULL;
    mGuidedSetupWindowPtr = NULL;
    mPopupImagePtr = NULL;
    mPopupAxisAdjusmentPtr = NULL;
    mPopupAxisSelectionPtr = NULL;
    mPopupWarningPtr = NULL;
    mPopupMaxPtr = NULL;
    mPopupMinPtr = NULL;
    mPopupNumberInputPtr = NULL;
    mPopupTextInputPtr = NULL;
    mPopupSelectionListPtr = NULL;
    mPopupOkBtnPtr = NULL;
    mPopupCancelBtnPtr = NULL;
    mPopupTextPtr = NULL;
    mPopupTitlePtr = NULL;
    mPopupWindowPtr = NULL;
    mNumPadClearBtnPtr = NULL;
    mNumPadCloseBtnPtr = NULL;
    mNumPadHeaderPtr = NULL;
    mNumPadInputPtr = NULL;
    mNumPadConfirmPtr = NULL;
    mNumPad0Ptr = NULL;
    mNumPadDelPtr = NULL;
    mNumPad9Ptr = NULL;
    mNumPad8Ptr = NULL;
    mNumPad7Ptr = NULL;
    mNumPad6Ptr = NULL;
    mNumPad5Ptr = NULL;
    mNumPad4Ptr = NULL;
    mNumPad3Ptr = NULL;
    mNumPad2Ptr = NULL;
    mNumPad1Ptr = NULL;
    mNumPadWindowPtr = NULL;
    mNoTouchWindowPtr = NULL;
    mSliderValuePtr = NULL;
    mSliderCloseBtnPtr = NULL;
    mSliderPtr = NULL;
    mSliderSuffixPtr = NULL;
    mSliderPrefixPtr = NULL;
    mSliderHeaderPtr = NULL;
    mSliderWindowPtr = NULL;
    mTextView13Ptr = NULL;
    mTextView12Ptr = NULL;
    mTextView11Ptr = NULL;
    mTextView10Ptr = NULL;
    mTextView9Ptr = NULL;
    mPrintTemperatureListPtr = NULL;
    mPrintTemperatureWindowPtr = NULL;
    mTextView15Ptr = NULL;
    mPrintSpeedFactorPtr = NULL;
    mPrintVolFlowPtr = NULL;
    mPrintTopSpeedPtr = NULL;
    mPrintRequestedSpeedPtr = NULL;
    mTextView8Ptr = NULL;
    mPrintSpeedMultiplierBarPtr = NULL;
    mTextView7Ptr = NULL;
    mPrintExtruderPositionListPtr = NULL;
    mTextView6Ptr = NULL;
    mPrintPositionListPtr = NULL;
    mPrintPositionWindowPtr = NULL;
    mPrintResumeBtnPtr = NULL;
    mPrintCancelBtnPtr = NULL;
    mPrintPauseBtnPtr = NULL;
    mPrintPauseWindowPtr = NULL;
    mTextView14Ptr = NULL;
    mPrintFanListPtr = NULL;
    mPrintFansWindowPtr = NULL;
    mPrintBabystepIncBtnPtr = NULL;
    mPrintBabystepDecBtnPtr = NULL;
    mPrintBabystepCurrentOffsetPtr = NULL;
    mTextView5Ptr = NULL;
    mPrintBabystepWindowPtr = NULL;
    mPrintEstimatedTimePtr = NULL;
    mPrintElapsedTimePtr = NULL;
    mPrintProgressBarPtr = NULL;
    mPrintFileNamePtr = NULL;
    mPrintJobInfoWindowPtr = NULL;
    mPrintWindowPtr = NULL;
    mTextView39Ptr = NULL;
    mTextView38Ptr = NULL;
    mScreensaverTimeoutInputPtr = NULL;
    mScreensaverEnablePtr = NULL;
    mTextView37Ptr = NULL;
    mScreensaverSettingWindowPtr = NULL;
    mTextView25Ptr = NULL;
    mGuidesListPtr = NULL;
    mGuideSelectionWindowPtr = NULL;
    mTextView23Ptr = NULL;
    mInfoTimeoutInputPtr = NULL;
    mTextView22Ptr = NULL;
    mPollIntervalInputPtr = NULL;
    mTextView20Ptr = NULL;
    mBaudRateListPtr = NULL;
    mDuetUartCommSettingWindowPtr = NULL;
    mPasswordInputPtr = NULL;
    mTextView21Ptr = NULL;
    mHostnameInputPtr = NULL;
    mTextView19Ptr = NULL;
    mTextView18Ptr = NULL;
    mIpAddressInputPtr = NULL;
    mDuetNetworkCommSettingWindowPtr = NULL;
    mDuetCommListPtr = NULL;
    mDuetCommSettingWindowPtr = NULL;
    mSettingsSlideWindowPtr = NULL;
    mSettingsWindowPtr = NULL;
    mNetworkWindowPtr = NULL;
    mUsbFilesPtr = NULL;
    mFolderIDPtr = NULL;
    mFileRefreshBtnPtr = NULL;
    mFileListViewPtr = NULL;
    mFilesWindowPtr = NULL;
    mFanWindowPtr = NULL;
    mHeightMapWindowPtr = NULL;
    mConsoleMacroBtn3Ptr = NULL;
    mConsoleMacroBtn2Ptr = NULL;
    mConsoleMacroBtn1Ptr = NULL;
    mConsoleClearBtnPtr = NULL;
    mSendBtnPtr = NULL;
    mTextView4Ptr = NULL;
    mTextView3Ptr = NULL;
    mEditText1Ptr = NULL;
    mGcodeListViewPtr = NULL;
    mConsoleListViewPtr = NULL;
    mConsoleWindowPtr = NULL;
    mFilamentControlHeadingPtr = NULL;
    mUnloadFilamentBtnPtr = NULL;
    mTextView36Ptr = NULL;
    mFilamentListPtr = NULL;
    mFilamentLoadUnloadWindowPtr = NULL;
    mExtrudeBtnPtr = NULL;
    mRetractBtnPtr = NULL;
    mTextView33Ptr = NULL;
    mTextView32Ptr = NULL;
    mTextView31Ptr = NULL;
    mTextView34Ptr = NULL;
    mTextView35Ptr = NULL;
    mTextView30Ptr = NULL;
    mExtrudeToolListPtr = NULL;
    mTextView29Ptr = NULL;
    mTextView28Ptr = NULL;
    mTextView27Ptr = NULL;
    mExtruderFeedratePtr = NULL;
    mExtruderFeedDistPtr = NULL;
    mExtrudeWindowPtr = NULL;
    mDisableMotorsBtnPtr = NULL;
    mTextView17Ptr = NULL;
    mTextView16Ptr = NULL;
    mFeedrateBtn5Ptr = NULL;
    mFeedrateBtn4Ptr = NULL;
    mFeedrateBtn3Ptr = NULL;
    mFeedrateBtn2Ptr = NULL;
    mTextView2Ptr = NULL;
    mHeightmapBtnPtr = NULL;
    mMeshLevelBtnPtr = NULL;
    mTrueLevelBtnPtr = NULL;
    mFeedrateBtn1Ptr = NULL;
    mAxisControlListViewPtr = NULL;
    mHomeAllBtnPtr = NULL;
    mMoveWindowPtr = NULL;
    mSlideWindow1Ptr = NULL;
    mWindowSelectWindowPtr = NULL;
	mTemperatureGraphLegendPtr = NULL;
	mTempGraphPtr = NULL;
    mTemperatureGraphWindowPtr = NULL;
    mToolListHeadingStandbyPtr = NULL;
    mToolListHeadingActivePtr = NULL;
    mToolListHeadingCurrentPtr = NULL;
    mToolListHeadingStatusPtr = NULL;
    mToolListHeadingNamePtr = NULL;
    mToolListViewPtr = NULL;
    mToolListWindowPtr = NULL;
    mMainWindowPtr = NULL;
    mStatusTextPtr = NULL;
    mConsoleBtnPtr = NULL;
    mMacroBtnPtr = NULL;
    mBackBtnPtr = NULL;
    mHomeBtnPtr = NULL;
    mSideBarWindowPtr = NULL;
    mRootWindowPtr = NULL;
}

const char* mainActivity::getAppName() const{
	return "main.ftu";
}

//TAG:onCreate
void mainActivity::onCreate() {
	Activity::onCreate();
    mPopupProgressPtr = (ZKSeekBar*)findControlByID(ID_MAIN_PopupProgress);if(mPopupProgressPtr!= NULL){mPopupProgressPtr->setSeekBarChangeListener(this);}
    mTempGraphYLabelsPtr = (ZKListView*)findControlByID(ID_MAIN_TempGraphYLabels);if(mTempGraphYLabelsPtr!= NULL){mTempGraphYLabelsPtr->setListAdapter(this);mTempGraphYLabelsPtr->setItemClickListener(this);}
    mTempGraphXLabelsPtr = (ZKListView*)findControlByID(ID_MAIN_TempGraphXLabels);if(mTempGraphXLabelsPtr!= NULL){mTempGraphXLabelsPtr->setListAdapter(this);mTempGraphXLabelsPtr->setItemClickListener(this);}
    mTemperatureGraphLegendPtr = (ZKListView*)findControlByID(ID_MAIN_TemperatureGraphLegend);if(mTemperatureGraphLegendPtr!= NULL){mTemperatureGraphLegendPtr->setListAdapter(this);mTemperatureGraphLegendPtr->setItemClickListener(this);}
	mTempGraphYLabelsPtr = (ZKListView*)findControlByID(ID_MAIN_TempGraphYLabels);
	if (mTempGraphYLabelsPtr != NULL)
	{
		mTempGraphYLabelsPtr->setListAdapter(this);
		mTempGraphYLabelsPtr->setItemClickListener(this);
	}
	mTempGraphXLabelsPtr = (ZKListView*)findControlByID(ID_MAIN_TempGraphXLabels);
	if (mTempGraphXLabelsPtr != NULL)
	{
		mTempGraphXLabelsPtr->setListAdapter(this);
		mTempGraphXLabelsPtr->setItemClickListener(this);
	}
	mGraphYLabelBottomPtr = (ZKTextView*)findControlByID(ID_MAIN_GraphYLabelBottom);
	mGraphYLabelMidPtr = (ZKTextView*)findControlByID(ID_MAIN_GraphYLabelMid);
	mGraphYLabelTopPtr = (ZKTextView*)findControlByID(ID_MAIN_GraphYLabelTop);
	mGraphXLabelRightPtr = (ZKTextView*)findControlByID(ID_MAIN_GraphXLabelRight);
	mGraphXLabelMidPtr = (ZKTextView*)findControlByID(ID_MAIN_GraphXLabelMid);
	mGraphXLabelLeftPtr = (ZKTextView*)findControlByID(ID_MAIN_GraphXLabelLeft);
	mTextView44Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView44);
	mFileListInfoPtr = (ZKTextView*)findControlByID(ID_MAIN_FileListInfo);
    mOverlayModalZonePtr = (ZKButton*)findControlByID(ID_MAIN_OverlayModalZone);
    mTextView43Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView43);
    mDebugCommandListPtr = (ZKListView*)findControlByID(ID_MAIN_DebugCommandList);if(mDebugCommandListPtr!= NULL){mDebugCommandListPtr->setListAdapter(this);mDebugCommandListPtr->setItemClickListener(this);}
    mDebugWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_DebugWindow);
    mTextView42Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView42);
    mThemesListPtr = (ZKListView*)findControlByID(ID_MAIN_ThemesList);if(mThemesListPtr!= NULL){mThemesListPtr->setListAdapter(this);mThemesListPtr->setItemClickListener(this);}
    mThemeSelectionWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_ThemeSelectionWindow);
    mTextView41Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView41);
    mSlideWindow2Ptr = (ZKSlideWindow*)findControlByID(ID_MAIN_SlideWindow2);if(mSlideWindow2Ptr!= NULL){mSlideWindow2Ptr->setSlideItemClickListener(this);}
    mListView1Ptr = (ZKListView*)findControlByID(ID_MAIN_ListView1);if(mListView1Ptr!= NULL){mListView1Ptr->setListAdapter(this);mListView1Ptr->setItemClickListener(this);}
    mCheckbox1Ptr = (ZKCheckBox*)findControlByID(ID_MAIN_Checkbox1);if(mCheckbox1Ptr!= NULL){mCheckbox1Ptr->setCheckedChangeListener(this);}
    mDigitalClock2Ptr = (ZKDigitalClock*)findControlByID(ID_MAIN_DigitalClock2);
    mPointer1Ptr = (ZKPointer*)findControlByID(ID_MAIN_Pointer1);
    mCircleBar1Ptr = (ZKCircleBar*)findControlByID(ID_MAIN_CircleBar1);
    mSeekBar1Ptr = (ZKSeekBar*)findControlByID(ID_MAIN_SeekBar1);if(mSeekBar1Ptr!= NULL){mSeekBar1Ptr->setSeekBarChangeListener(this);}
    mEditText2Ptr = (ZKEditText*)findControlByID(ID_MAIN_EditText2);if(mEditText2Ptr!= NULL){mEditText2Ptr->setTextChangeListener(this);}
    mButton1Ptr = (ZKButton*)findControlByID(ID_MAIN_Button1);
    mTextView40Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView40);
    mWindow1Ptr = (ZKWindow*)findControlByID(ID_MAIN_Window1);
    mCommunicationTypePtr = (ZKTextView*)findControlByID(ID_MAIN_CommunicationType);
    mDigitalClock1Ptr = (ZKDigitalClock*)findControlByID(ID_MAIN_DigitalClock1);
    mEStopBtnPtr = (ZKButton*)findControlByID(ID_MAIN_EStopBtn);
    mTextView26Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView26);
    mPageOverlayExampleWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_PageOverlayExampleWindow);
    mTextView24Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView24);
    mShowSetupOnStartupPtr = (ZKCheckBox*)findControlByID(ID_MAIN_ShowSetupOnStartup);if(mShowSetupOnStartupPtr!= NULL){mShowSetupOnStartupPtr->setCheckedChangeListener(this);}
    mSetupGuideWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_SetupGuideWindow);
    mGuidePageNumPtr = (ZKTextView*)findControlByID(ID_MAIN_GuidePageNum);
    mCloseGuideBtnPtr = (ZKButton*)findControlByID(ID_MAIN_CloseGuideBtn);
    mPreviousPageBtnPtr = (ZKButton*)findControlByID(ID_MAIN_PreviousPageBtn);
    mNextPageBtnPtr = (ZKButton*)findControlByID(ID_MAIN_NextPageBtn);
    mGuidedSetupWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_GuidedSetupWindow);
    mPopupImagePtr = (ZKTextView*)findControlByID(ID_MAIN_PopupImage);
    mPopupAxisAdjusmentPtr = (ZKListView*)findControlByID(ID_MAIN_PopupAxisAdjusment);if(mPopupAxisAdjusmentPtr!= NULL){mPopupAxisAdjusmentPtr->setListAdapter(this);mPopupAxisAdjusmentPtr->setItemClickListener(this);}
    mPopupAxisSelectionPtr = (ZKListView*)findControlByID(ID_MAIN_PopupAxisSelection);if(mPopupAxisSelectionPtr!= NULL){mPopupAxisSelectionPtr->setListAdapter(this);mPopupAxisSelectionPtr->setItemClickListener(this);}
    mPopupWarningPtr = (ZKTextView*)findControlByID(ID_MAIN_PopupWarning);
    mPopupMaxPtr = (ZKTextView*)findControlByID(ID_MAIN_PopupMax);
    mPopupMinPtr = (ZKTextView*)findControlByID(ID_MAIN_PopupMin);
    mPopupNumberInputPtr = (ZKEditText*)findControlByID(ID_MAIN_PopupNumberInput);if(mPopupNumberInputPtr!= NULL){mPopupNumberInputPtr->setTextChangeListener(this);}
    mPopupTextInputPtr = (ZKEditText*)findControlByID(ID_MAIN_PopupTextInput);if(mPopupTextInputPtr!= NULL){mPopupTextInputPtr->setTextChangeListener(this);}
    mPopupSelectionListPtr = (ZKListView*)findControlByID(ID_MAIN_PopupSelectionList);if(mPopupSelectionListPtr!= NULL){mPopupSelectionListPtr->setListAdapter(this);mPopupSelectionListPtr->setItemClickListener(this);}
    mPopupOkBtnPtr = (ZKButton*)findControlByID(ID_MAIN_PopupOkBtn);
    mPopupCancelBtnPtr = (ZKButton*)findControlByID(ID_MAIN_PopupCancelBtn);
    mPopupTextPtr = (ZKTextView*)findControlByID(ID_MAIN_PopupText);
    mPopupTitlePtr = (ZKTextView*)findControlByID(ID_MAIN_PopupTitle);
    mPopupWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_PopupWindow);
    mNumPadClearBtnPtr = (ZKButton*)findControlByID(ID_MAIN_NumPadClearBtn);
    mNumPadCloseBtnPtr = (ZKButton*)findControlByID(ID_MAIN_NumPadCloseBtn);
    mNumPadHeaderPtr = (ZKTextView*)findControlByID(ID_MAIN_NumPadHeader);
    mNumPadInputPtr = (ZKTextView*)findControlByID(ID_MAIN_NumPadInput);
    mNumPadConfirmPtr = (ZKButton*)findControlByID(ID_MAIN_NumPadConfirm);
    mNumPad0Ptr = (ZKButton*)findControlByID(ID_MAIN_NumPad0);
    mNumPadDelPtr = (ZKButton*)findControlByID(ID_MAIN_NumPadDel);
    mNumPad9Ptr = (ZKButton*)findControlByID(ID_MAIN_NumPad9);
    mNumPad8Ptr = (ZKButton*)findControlByID(ID_MAIN_NumPad8);
    mNumPad7Ptr = (ZKButton*)findControlByID(ID_MAIN_NumPad7);
    mNumPad6Ptr = (ZKButton*)findControlByID(ID_MAIN_NumPad6);
    mNumPad5Ptr = (ZKButton*)findControlByID(ID_MAIN_NumPad5);
    mNumPad4Ptr = (ZKButton*)findControlByID(ID_MAIN_NumPad4);
    mNumPad3Ptr = (ZKButton*)findControlByID(ID_MAIN_NumPad3);
    mNumPad2Ptr = (ZKButton*)findControlByID(ID_MAIN_NumPad2);
    mNumPad1Ptr = (ZKButton*)findControlByID(ID_MAIN_NumPad1);
    mNumPadWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_NumPadWindow);
    mNoTouchWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_NoTouchWindow);
    mSliderValuePtr = (ZKTextView*)findControlByID(ID_MAIN_SliderValue);
    mSliderCloseBtnPtr = (ZKButton*)findControlByID(ID_MAIN_SliderCloseBtn);
    mSliderPtr = (ZKSeekBar*)findControlByID(ID_MAIN_Slider);if(mSliderPtr!= NULL){mSliderPtr->setSeekBarChangeListener(this);}
    mSliderSuffixPtr = (ZKTextView*)findControlByID(ID_MAIN_SliderSuffix);
    mSliderPrefixPtr = (ZKTextView*)findControlByID(ID_MAIN_SliderPrefix);
    mSliderHeaderPtr = (ZKTextView*)findControlByID(ID_MAIN_SliderHeader);
    mSliderWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_SliderWindow);
    mTextView13Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView13);
    mTextView12Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView12);
    mTextView11Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView11);
    mTextView10Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView10);
    mTextView9Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView9);
    mPrintTemperatureListPtr = (ZKListView*)findControlByID(ID_MAIN_PrintTemperatureList);if(mPrintTemperatureListPtr!= NULL){mPrintTemperatureListPtr->setListAdapter(this);mPrintTemperatureListPtr->setItemClickListener(this);}
    mPrintTemperatureWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_PrintTemperatureWindow);
    mTextView15Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView15);
    mPrintSpeedFactorPtr = (ZKTextView*)findControlByID(ID_MAIN_PrintSpeedFactor);
    mPrintVolFlowPtr = (ZKTextView*)findControlByID(ID_MAIN_PrintVolFlow);
    mPrintTopSpeedPtr = (ZKTextView*)findControlByID(ID_MAIN_PrintTopSpeed);
    mPrintRequestedSpeedPtr = (ZKTextView*)findControlByID(ID_MAIN_PrintRequestedSpeed);
    mTextView8Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView8);
    mPrintSpeedMultiplierBarPtr = (ZKSeekBar*)findControlByID(ID_MAIN_PrintSpeedMultiplierBar);if(mPrintSpeedMultiplierBarPtr!= NULL){mPrintSpeedMultiplierBarPtr->setSeekBarChangeListener(this);}
    mTextView7Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView7);
    mPrintExtruderPositionListPtr = (ZKListView*)findControlByID(ID_MAIN_PrintExtruderPositionList);if(mPrintExtruderPositionListPtr!= NULL){mPrintExtruderPositionListPtr->setListAdapter(this);mPrintExtruderPositionListPtr->setItemClickListener(this);}
    mTextView6Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView6);
    mPrintPositionListPtr = (ZKListView*)findControlByID(ID_MAIN_PrintPositionList);if(mPrintPositionListPtr!= NULL){mPrintPositionListPtr->setListAdapter(this);mPrintPositionListPtr->setItemClickListener(this);}
    mPrintPositionWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_PrintPositionWindow);
    mPrintResumeBtnPtr = (ZKButton*)findControlByID(ID_MAIN_PrintResumeBtn);
    mPrintCancelBtnPtr = (ZKButton*)findControlByID(ID_MAIN_PrintCancelBtn);
    mPrintPauseBtnPtr = (ZKButton*)findControlByID(ID_MAIN_PrintPauseBtn);
    mPrintPauseWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_PrintPauseWindow);
    mTextView14Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView14);
    mPrintFanListPtr = (ZKListView*)findControlByID(ID_MAIN_PrintFanList);if(mPrintFanListPtr!= NULL){mPrintFanListPtr->setListAdapter(this);mPrintFanListPtr->setItemClickListener(this);}
    mPrintFansWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_PrintFansWindow);
    mPrintBabystepIncBtnPtr = (ZKButton*)findControlByID(ID_MAIN_PrintBabystepIncBtn);
    mPrintBabystepDecBtnPtr = (ZKButton*)findControlByID(ID_MAIN_PrintBabystepDecBtn);
    mPrintBabystepCurrentOffsetPtr = (ZKTextView*)findControlByID(ID_MAIN_PrintBabystepCurrentOffset);
    mTextView5Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView5);
    mPrintBabystepWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_PrintBabystepWindow);
    mPrintEstimatedTimePtr = (ZKTextView*)findControlByID(ID_MAIN_PrintEstimatedTime);
    mPrintElapsedTimePtr = (ZKTextView*)findControlByID(ID_MAIN_PrintElapsedTime);
    mPrintProgressBarPtr = (ZKCircleBar*)findControlByID(ID_MAIN_PrintProgressBar);
    mPrintFileNamePtr = (ZKTextView*)findControlByID(ID_MAIN_PrintFileName);
    mPrintJobInfoWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_PrintJobInfoWindow);
    mPrintWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_PrintWindow);
    mTextView39Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView39);
    mTextView38Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView38);
    mScreensaverTimeoutInputPtr = (ZKEditText*)findControlByID(ID_MAIN_ScreensaverTimeoutInput);if(mScreensaverTimeoutInputPtr!= NULL){mScreensaverTimeoutInputPtr->setTextChangeListener(this);}
    mScreensaverEnablePtr = (ZKCheckBox*)findControlByID(ID_MAIN_ScreensaverEnable);if(mScreensaverEnablePtr!= NULL){mScreensaverEnablePtr->setCheckedChangeListener(this);}
    mTextView37Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView37);
    mScreensaverSettingWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_ScreensaverSettingWindow);
    mTextView25Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView25);
    mGuidesListPtr = (ZKListView*)findControlByID(ID_MAIN_GuidesList);if(mGuidesListPtr!= NULL){mGuidesListPtr->setListAdapter(this);mGuidesListPtr->setItemClickListener(this);}
    mGuideSelectionWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_GuideSelectionWindow);
    mTextView23Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView23);
    mInfoTimeoutInputPtr = (ZKEditText*)findControlByID(ID_MAIN_InfoTimeoutInput);if(mInfoTimeoutInputPtr!= NULL){mInfoTimeoutInputPtr->setTextChangeListener(this);}
    mTextView22Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView22);
    mPollIntervalInputPtr = (ZKEditText*)findControlByID(ID_MAIN_PollIntervalInput);if(mPollIntervalInputPtr!= NULL){mPollIntervalInputPtr->setTextChangeListener(this);}
    mTextView20Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView20);
    mBaudRateListPtr = (ZKListView*)findControlByID(ID_MAIN_BaudRateList);if(mBaudRateListPtr!= NULL){mBaudRateListPtr->setListAdapter(this);mBaudRateListPtr->setItemClickListener(this);}
    mDuetUartCommSettingWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_DuetUartCommSettingWindow);
    mPasswordInputPtr = (ZKEditText*)findControlByID(ID_MAIN_PasswordInput);if(mPasswordInputPtr!= NULL){mPasswordInputPtr->setTextChangeListener(this);}
    mTextView21Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView21);
    mHostnameInputPtr = (ZKEditText*)findControlByID(ID_MAIN_HostnameInput);if(mHostnameInputPtr!= NULL){mHostnameInputPtr->setTextChangeListener(this);}
    mTextView19Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView19);
    mTextView18Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView18);
    mIpAddressInputPtr = (ZKEditText*)findControlByID(ID_MAIN_IpAddressInput);if(mIpAddressInputPtr!= NULL){mIpAddressInputPtr->setTextChangeListener(this);}
    mDuetNetworkCommSettingWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_DuetNetworkCommSettingWindow);
    mDuetCommListPtr = (ZKListView*)findControlByID(ID_MAIN_DuetCommList);if(mDuetCommListPtr!= NULL){mDuetCommListPtr->setListAdapter(this);mDuetCommListPtr->setItemClickListener(this);}
    mDuetCommSettingWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_DuetCommSettingWindow);
    mSettingsSlideWindowPtr = (ZKSlideWindow*)findControlByID(ID_MAIN_SettingsSlideWindow);if(mSettingsSlideWindowPtr!= NULL){mSettingsSlideWindowPtr->setSlideItemClickListener(this);}
    mSettingsWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_SettingsWindow);
    mNetworkWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_NetworkWindow);
    mUsbFilesPtr = (ZKButton*)findControlByID(ID_MAIN_UsbFiles);
    mFolderIDPtr = (ZKTextView*)findControlByID(ID_MAIN_FolderID);
    mFileRefreshBtnPtr = (ZKButton*)findControlByID(ID_MAIN_FileRefreshBtn);
    mFileListViewPtr = (ZKListView*)findControlByID(ID_MAIN_FileListView);if(mFileListViewPtr!= NULL){mFileListViewPtr->setListAdapter(this);mFileListViewPtr->setItemClickListener(this);}
    mFilesWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_FilesWindow);
    mFanWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_FanWindow);
    mHeightMapWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_HeightMapWindow);
    mConsoleMacroBtn3Ptr = (ZKButton*)findControlByID(ID_MAIN_ConsoleMacroBtn3);
    mConsoleMacroBtn2Ptr = (ZKButton*)findControlByID(ID_MAIN_ConsoleMacroBtn2);
    mConsoleMacroBtn1Ptr = (ZKButton*)findControlByID(ID_MAIN_ConsoleMacroBtn1);
    mConsoleClearBtnPtr = (ZKButton*)findControlByID(ID_MAIN_ConsoleClearBtn);
    mSendBtnPtr = (ZKButton*)findControlByID(ID_MAIN_SendBtn);
    mTextView4Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView4);
    mTextView3Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView3);
    mEditText1Ptr = (ZKEditText*)findControlByID(ID_MAIN_EditText1);if(mEditText1Ptr!= NULL){mEditText1Ptr->setTextChangeListener(this);}
    mGcodeListViewPtr = (ZKListView*)findControlByID(ID_MAIN_GcodeListView);if(mGcodeListViewPtr!= NULL){mGcodeListViewPtr->setListAdapter(this);mGcodeListViewPtr->setItemClickListener(this);}
    mConsoleListViewPtr = (ZKListView*)findControlByID(ID_MAIN_ConsoleListView);if(mConsoleListViewPtr!= NULL){mConsoleListViewPtr->setListAdapter(this);mConsoleListViewPtr->setItemClickListener(this);}
    mConsoleWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_ConsoleWindow);
    mFilamentControlHeadingPtr = (ZKTextView*)findControlByID(ID_MAIN_FilamentControlHeading);
    mUnloadFilamentBtnPtr = (ZKButton*)findControlByID(ID_MAIN_UnloadFilamentBtn);
    mTextView36Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView36);
    mFilamentListPtr = (ZKListView*)findControlByID(ID_MAIN_FilamentList);if(mFilamentListPtr!= NULL){mFilamentListPtr->setListAdapter(this);mFilamentListPtr->setItemClickListener(this);}
    mFilamentLoadUnloadWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_FilamentLoadUnloadWindow);
    mExtrudeBtnPtr = (ZKButton*)findControlByID(ID_MAIN_ExtrudeBtn);
    mRetractBtnPtr = (ZKButton*)findControlByID(ID_MAIN_RetractBtn);
    mTextView33Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView33);
    mTextView32Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView32);
    mTextView31Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView31);
    mTextView34Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView34);
    mTextView35Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView35);
    mTextView30Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView30);
    mExtrudeToolListPtr = (ZKListView*)findControlByID(ID_MAIN_ExtrudeToolList);if(mExtrudeToolListPtr!= NULL){mExtrudeToolListPtr->setListAdapter(this);mExtrudeToolListPtr->setItemClickListener(this);}
    mTextView29Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView29);
    mTextView28Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView28);
    mTextView27Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView27);
    mExtruderFeedratePtr = (ZKListView*)findControlByID(ID_MAIN_ExtruderFeedrate);if(mExtruderFeedratePtr!= NULL){mExtruderFeedratePtr->setListAdapter(this);mExtruderFeedratePtr->setItemClickListener(this);}
    mExtruderFeedDistPtr = (ZKListView*)findControlByID(ID_MAIN_ExtruderFeedDist);if(mExtruderFeedDistPtr!= NULL){mExtruderFeedDistPtr->setListAdapter(this);mExtruderFeedDistPtr->setItemClickListener(this);}
    mExtrudeWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_ExtrudeWindow);
    mDisableMotorsBtnPtr = (ZKButton*)findControlByID(ID_MAIN_DisableMotorsBtn);
    mTextView17Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView17);
    mTextView16Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView16);
    mFeedrateBtn5Ptr = (ZKButton*)findControlByID(ID_MAIN_FeedrateBtn5);
    mFeedrateBtn4Ptr = (ZKButton*)findControlByID(ID_MAIN_FeedrateBtn4);
    mFeedrateBtn3Ptr = (ZKButton*)findControlByID(ID_MAIN_FeedrateBtn3);
    mFeedrateBtn2Ptr = (ZKButton*)findControlByID(ID_MAIN_FeedrateBtn2);
    mTextView2Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView2);
    mHeightmapBtnPtr = (ZKButton*)findControlByID(ID_MAIN_HeightmapBtn);
    mMeshLevelBtnPtr = (ZKButton*)findControlByID(ID_MAIN_MeshLevelBtn);
    mTrueLevelBtnPtr = (ZKButton*)findControlByID(ID_MAIN_TrueLevelBtn);
    mFeedrateBtn1Ptr = (ZKButton*)findControlByID(ID_MAIN_FeedrateBtn1);
    mAxisControlListViewPtr = (ZKListView*)findControlByID(ID_MAIN_AxisControlListView);if(mAxisControlListViewPtr!= NULL){mAxisControlListViewPtr->setListAdapter(this);mAxisControlListViewPtr->setItemClickListener(this);}
    mHomeAllBtnPtr = (ZKButton*)findControlByID(ID_MAIN_HomeAllBtn);
    mMoveWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_MoveWindow);
    mSlideWindow1Ptr = (ZKSlideWindow*)findControlByID(ID_MAIN_SlideWindow1);if(mSlideWindow1Ptr!= NULL){mSlideWindow1Ptr->setSlideItemClickListener(this);}
    mWindowSelectWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_WindowSelectWindow);
	mTemperatureGraphLegendPtr = (ZKListView*)findControlByID(ID_MAIN_TemperatureGraphLegend);
	if (mTemperatureGraphLegendPtr != NULL)
	{
		mTemperatureGraphLegendPtr->setListAdapter(this);
		mTemperatureGraphLegendPtr->setItemClickListener(this);
	}
	mTempGraphPtr = (ZKDiagram*)findControlByID(ID_MAIN_TempGraph);
    mTemperatureGraphWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_TemperatureGraphWindow);
    mToolListHeadingStandbyPtr = (ZKTextView*)findControlByID(ID_MAIN_ToolListHeadingStandby);
    mToolListHeadingActivePtr = (ZKTextView*)findControlByID(ID_MAIN_ToolListHeadingActive);
    mToolListHeadingCurrentPtr = (ZKTextView*)findControlByID(ID_MAIN_ToolListHeadingCurrent);
    mToolListHeadingStatusPtr = (ZKTextView*)findControlByID(ID_MAIN_ToolListHeadingStatus);
    mToolListHeadingNamePtr = (ZKTextView*)findControlByID(ID_MAIN_ToolListHeadingName);
    mToolListViewPtr = (ZKListView*)findControlByID(ID_MAIN_ToolListView);if(mToolListViewPtr!= NULL){mToolListViewPtr->setListAdapter(this);mToolListViewPtr->setItemClickListener(this);}
    mToolListWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_ToolListWindow);
    mMainWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_MainWindow);
    mStatusTextPtr = (ZKTextView*)findControlByID(ID_MAIN_StatusText);
    mConsoleBtnPtr = (ZKButton*)findControlByID(ID_MAIN_ConsoleBtn);
    mMacroBtnPtr = (ZKButton*)findControlByID(ID_MAIN_MacroBtn);
    mBackBtnPtr = (ZKButton*)findControlByID(ID_MAIN_BackBtn);
    mHomeBtnPtr = (ZKButton*)findControlByID(ID_MAIN_HomeBtn);
    mSideBarWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_SideBarWindow);
    mRootWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_RootWindow);
	mActivityPtr = this;
	onUI_init();
  registerProtocolDataUpdateListener(onProtocolDataUpdate);
  rigesterActivityTimer();
}

void mainActivity::onClick(ZKBase *pBase) {
	//TODO: add widget onClik code 
    int buttonTablen = sizeof(sButtonCallbackTab) / sizeof(S_ButtonCallback);
    for (int i = 0; i < buttonTablen; ++i) {
        if (sButtonCallbackTab[i].id == pBase->getID()) {
            if (sButtonCallbackTab[i].callback((ZKButton*)pBase)) {
            	return;
            }
            break;
        }
    }


    int len = sizeof(sAppInfoTab) / sizeof(sAppInfoTab[0]);
    for (int i = 0; i < len; ++i) {
        if (sAppInfoTab[i].id == pBase->getID()) {
            EASYUICONTEXT->openActivity(sAppInfoTab[i].pApp);
            return;
        }
    }

	Activity::onClick(pBase);
}

void mainActivity::onResume() {
	Activity::onResume();
	EASYUICONTEXT->registerGlobalTouchListener(this);
	startVideoLoopPlayback();
	onUI_show();
}

void mainActivity::onPause() {
	Activity::onPause();
	EASYUICONTEXT->unregisterGlobalTouchListener(this);
	stopVideoLoopPlayback();
	onUI_hide();
}

void mainActivity::onIntent(const Intent *intentPtr) {
	Activity::onIntent(intentPtr);
	onUI_intent(intentPtr);
}

bool mainActivity::onTimer(int id) {
	return onUI_Timer(id);
}

void mainActivity::onProgressChanged(ZKSeekBar *pSeekBar, int progress){

    int seekBarTablen = sizeof(SZKSeekBarCallbackTab) / sizeof(S_ZKSeekBarCallback);
    for (int i = 0; i < seekBarTablen; ++i) {
        if (SZKSeekBarCallbackTab[i].id == pSeekBar->getID()) {
            SZKSeekBarCallbackTab[i].callback(pSeekBar, progress);
            break;
        }
    }
}

int mainActivity::getListItemCount(const ZKListView *pListView) const{
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            return SListViewFunctionsCallbackTab[i].getListItemCountCallback(pListView);
            break;
        }
    }
    return 0;
}

void mainActivity::obtainListItemData(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index){
	UI::Theme::ThemeListItem(pListView, pListItem, index);
	int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
	for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            SListViewFunctionsCallbackTab[i].obtainListItemDataCallback(pListView, pListItem, index);
            break;
        }
    }
}

void mainActivity::onItemClick(ZKListView *pListView, int index, int id){
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            SListViewFunctionsCallbackTab[i].onItemClickCallback(pListView, index, id);
            break;
        }
    }
}

void mainActivity::onSlideItemClick(ZKSlideWindow *pSlideWindow, int index) {
    int tablen = sizeof(SSlideWindowItemClickCallbackTab) / sizeof(S_SlideWindowItemClickCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SSlideWindowItemClickCallbackTab[i].id == pSlideWindow->getID()) {
            SSlideWindowItemClickCallbackTab[i].onSlideItemClickCallback(pSlideWindow, index);
            break;
        }
    }
}

bool mainActivity::onTouchEvent(const MotionEvent &ev) {
    return onmainActivityTouchEvent(ev);
}

void mainActivity::onTextChanged(ZKTextView *pTextView, const std::string &text) {
    int tablen = sizeof(SEditTextInputCallbackTab) / sizeof(S_EditTextInputCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SEditTextInputCallbackTab[i].id == pTextView->getID()) {
            SEditTextInputCallbackTab[i].onEditTextChangedCallback(text);
            break;
        }
    }
}

void mainActivity::rigesterActivityTimer() {
    int tablen = sizeof(REGISTER_ACTIVITY_TIMER_TAB) / sizeof(S_ACTIVITY_TIMEER);
    for (int i = 0; i < tablen; ++i) {
        S_ACTIVITY_TIMEER temp = REGISTER_ACTIVITY_TIMER_TAB[i];
        registerTimer(temp.id, temp.time);
    }
}


void mainActivity::onVideoPlayerMessage(ZKVideoView *pVideoView, int msg) {
    int tablen = sizeof(SVideoViewCallbackTab) / sizeof(S_VideoViewCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SVideoViewCallbackTab[i].id == pVideoView->getID()) {
        	if (SVideoViewCallbackTab[i].loop) {
                //循环播放
        		videoLoopPlayback(pVideoView, msg, i);
        	} else if (SVideoViewCallbackTab[i].onVideoViewCallback != NULL){
        	    SVideoViewCallbackTab[i].onVideoViewCallback(pVideoView, msg);
        	}
            break;
        }
    }
}

void mainActivity::videoLoopPlayback(ZKVideoView *pVideoView, int msg, size_t callbackTabIndex) {

	switch (msg) {
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_STARTED:
		LOGD("ZKVideoView::E_MSGTYPE_VIDEO_PLAY_STARTED\n");
    if (callbackTabIndex >= (sizeof(SVideoViewCallbackTab)/sizeof(S_VideoViewCallback))) {
      break;
    }
		pVideoView->setVolume(SVideoViewCallbackTab[callbackTabIndex].defaultvolume / 10.0);
		mVideoLoopErrorCount = 0;
		break;
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_ERROR:
		/**错误处理 */
		++mVideoLoopErrorCount;
		if (mVideoLoopErrorCount > 100) {
			LOGD("video loop error counts > 100, quit loop playback !");
            break;
		} //不用break, 继续尝试播放下一个
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED:
		LOGD("ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED\n");
        std::vector<std::string> videolist;
        std::string fileName(getAppName());
        if (fileName.size() < 4) {
             LOGD("getAppName size < 4, ignore!");
             break;
        }
        fileName = fileName.substr(0, fileName.length() - 4) + "_video_list.txt";
        fileName = "/mnt/extsd/" + fileName;
        if (!parseVideoFileList(fileName.c_str(), videolist)) {
            LOGD("parseVideoFileList failed !");
		    break;
        }
		if (pVideoView && !videolist.empty()) {
			mVideoLoopIndex = (mVideoLoopIndex + 1) % videolist.size();
			pVideoView->play(videolist[mVideoLoopIndex].c_str());
		}
		break;
	}
}

void mainActivity::startVideoLoopPlayback() {
    int tablen = sizeof(SVideoViewCallbackTab) / sizeof(S_VideoViewCallback);
    for (int i = 0; i < tablen; ++i) {
    	if (SVideoViewCallbackTab[i].loop) {
    		ZKVideoView* videoView = (ZKVideoView*)findControlByID(SVideoViewCallbackTab[i].id);
    		if (!videoView) {
    			return;
    		}
    		//循环播放
    		videoLoopPlayback(videoView, ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED, i);
    		return;
    	}
    }
}

void mainActivity::stopVideoLoopPlayback() {
    int tablen = sizeof(SVideoViewCallbackTab) / sizeof(S_VideoViewCallback);
    for (int i = 0; i < tablen; ++i) {
    	if (SVideoViewCallbackTab[i].loop) {
    		ZKVideoView* videoView = (ZKVideoView*)findControlByID(SVideoViewCallbackTab[i].id);
    		if (!videoView) {
    			return;
    		}
    		if (videoView->isPlaying()) {
    		    videoView->stop();
    		}
    		return;
    	}
    }
}

bool mainActivity::parseVideoFileList(const char *pFileListPath, std::vector<string>& mediaFileList) {
	mediaFileList.clear();
	if (NULL == pFileListPath || 0 == strlen(pFileListPath)) {
        LOGD("video file list is null!");
		return false;
	}

	ifstream is(pFileListPath, ios_base::in);
	if (!is.is_open()) {
		LOGD("cann't open file %s \n", pFileListPath);
		return false;
	}
	char tmp[1024] = {0};
	while (is.getline(tmp, sizeof(tmp))) {
		string str = tmp;
		removeCharFromString(str, '\"');
		removeCharFromString(str, '\r');
		removeCharFromString(str, '\n');
		if (str.size() > 1) {
     		mediaFileList.push_back(str.c_str());
		}
	}
  LOGD("(f:%s, l:%d) parse fileList[%s], get [%d]files", __FUNCTION__,
      __LINE__, pFileListPath, int(mediaFileList.size()));
  for (std::vector<string>::size_type i = 0; i < mediaFileList.size(); i++) {
    LOGD("file[%u]:[%s]", ::size_t(i), mediaFileList[i].c_str());
  }
	is.close();

	return true;
}

int mainActivity::removeCharFromString(string& nString, char c) {
    string::size_type   pos;
    while(1) {
        pos = nString.find(c);
        if(pos != string::npos) {
            nString.erase(pos, 1);
        } else {
            break;
        }
    }
    return (int)nString.size();
}

void mainActivity::registerUserTimer(int id, int time) {
	registerTimer(id, time);
}

void mainActivity::unregisterUserTimer(int id) {
	unregisterTimer(id);
}

void mainActivity::resetUserTimer(int id, int time) {
	resetTimer(id, time);
}
void mainActivity::onCheckedChanged(ZKCheckBox* pCheckBox, bool isChecked) {
    int tablen = sizeof(SCheckboxCallbackTab) / sizeof(S_CheckboxCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SCheckboxCallbackTab[i].id == pCheckBox->getID()) {
        	SCheckboxCallbackTab[i].onCheckedChanged(pCheckBox, isChecked);
            break;
        }
    }
}