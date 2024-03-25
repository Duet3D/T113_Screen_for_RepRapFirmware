/***********************************************
/gen auto by zuitools
***********************************************/
#ifndef __MAINACTIVITY_H__
#define __MAINACTIVITY_H__


#include "app/Activity.h"
#include "entry/EasyUIContext.h"

#include "uart/ProtocolData.h"
#include "uart/ProtocolParser.h"

#include "utils/Log.h"
#include "control/ZKDigitalClock.h"
#include "control/ZKButton.h"
#include "control/ZKCircleBar.h"
#include "control/ZKDiagram.h"
#include "control/ZKListView.h"
#include "control/ZKPointer.h"
#include "control/ZKQRCode.h"
#include "control/ZKTextView.h"
#include "control/ZKSeekBar.h"
#include "control/ZKEditText.h"
#include "control/ZKVideoView.h"
#include "control/ZKCheckbox.h"
#include "window/ZKSlideWindow.h"

/*TAG:Macro宏ID*/
#define ID_MAIN_PrinterName    50081
#define ID_MAIN_PopupProgress    91004
#define ID_MAIN_TempGraphYLabels    80025
#define ID_MAIN_TempGraphXLabels    80024
#define ID_MAIN_GraphYLabelBottom    50080
#define ID_MAIN_GraphYLabelMid    50079
#define ID_MAIN_GraphYLabelTop    50078
#define ID_MAIN_GraphXLabelRight    50076
#define ID_MAIN_GraphXLabelMid    50077
#define ID_MAIN_GraphXLabelLeft    50003
#define ID_MAIN_TextView44    50075
#define ID_MAIN_FileListInfo    50074
#define ID_MAIN_OverlayModalZone    20052
#define ID_MAIN_TextView43    50073
#define ID_MAIN_DebugCommandList    80023
#define ID_MAIN_DebugWindow    110038
#define ID_MAIN_FileNameSubItem    24036
#define ID_MAIN_TextView42    50072
#define ID_MAIN_ThemesList    80022
#define ID_MAIN_ThemeSelectionWindow    110037
#define ID_MAIN_TextView41    50071
#define ID_MAIN_SlideWindow2    30003
#define ID_MAIN_SubItem1    24035
#define ID_MAIN_ListView1    80021
#define ID_MAIN_Checkbox1    21003
#define ID_MAIN_DigitalClock2    93002
#define ID_MAIN_Pointer1    90001
#define ID_MAIN_CircleBar1    130002
#define ID_MAIN_SeekBar1    91003
#define ID_MAIN_EditText2    51010
#define ID_MAIN_Button1    20050
#define ID_MAIN_TextView40    50070
#define ID_MAIN_Window1    110032
#define ID_MAIN_CommunicationType    50043
#define ID_MAIN_DigitalClock1    93001
#define ID_MAIN_EStopBtn    20005
#define ID_MAIN_TextView26    50055
#define ID_MAIN_PageOverlayExampleWindow    110033
#define ID_MAIN_TextView24    50054
#define ID_MAIN_ShowSetupOnStartup    21001
#define ID_MAIN_SetupGuideWindow    110031
#define ID_MAIN_GuidePageNum    50051
#define ID_MAIN_CloseGuideBtn    20047
#define ID_MAIN_PreviousPageBtn    20046
#define ID_MAIN_NextPageBtn    20045
#define ID_MAIN_GuidedSetupWindow    110029
#define ID_MAIN_PopupImage    50053
#define ID_MAIN_PopupAxisAdjusment    80013
#define ID_MAIN_PopupAxisSelection    80014
#define ID_MAIN_PopupWarning    50042
#define ID_MAIN_PopupMax    50041
#define ID_MAIN_PopupMin    50040
#define ID_MAIN_PopupNumberInput    51003
#define ID_MAIN_PopupTextInput    51002
#define ID_MAIN_PopupSelectionList    80012
#define ID_MAIN_PopupOkBtn    20037
#define ID_MAIN_PopupCancelBtn    20036
#define ID_MAIN_PopupText    50034
#define ID_MAIN_PopupTitle    50039
#define ID_MAIN_PopupWindow    110024
#define ID_MAIN_NumPadClearBtn    20041
#define ID_MAIN_NumPadCloseBtn    20040
#define ID_MAIN_NumPadHeader    50050
#define ID_MAIN_NumPadInput    50006
#define ID_MAIN_NumPadConfirm    20016
#define ID_MAIN_NumPad0    20015
#define ID_MAIN_NumPadDel    20014
#define ID_MAIN_NumPad9    20013
#define ID_MAIN_NumPad8    20012
#define ID_MAIN_NumPad7    20011
#define ID_MAIN_NumPad6    20010
#define ID_MAIN_NumPad5    20009
#define ID_MAIN_NumPad4    20007
#define ID_MAIN_NumPad3    20008
#define ID_MAIN_NumPad2    20006
#define ID_MAIN_NumPad1    20002
#define ID_MAIN_NumPadWindow    110006
#define ID_MAIN_NoTouchWindow    110025
#define ID_MAIN_SliderValue    50036
#define ID_MAIN_SliderCloseBtn    20034
#define ID_MAIN_Slider    91002
#define ID_MAIN_SliderSuffix    50026
#define ID_MAIN_SliderPrefix    50025
#define ID_MAIN_SliderHeader    50024
#define ID_MAIN_SliderWindow    110023
#define ID_MAIN_TextView13    50031
#define ID_MAIN_TextView12    50030
#define ID_MAIN_TextView11    50029
#define ID_MAIN_TextView10    50028
#define ID_MAIN_TextView9    50027
#define ID_MAIN_PrintTemperatureListNameSubItem    24025
#define ID_MAIN_PrintTemperatureListStandbySubItem    24024
#define ID_MAIN_PrintTemperatureListStatusSubItem    24023
#define ID_MAIN_PrintTemperatureListCurrentSubItem    24022
#define ID_MAIN_PrintTemperatureListActiveSubItem    24021
#define ID_MAIN_PrintTemperatureList    80010
#define ID_MAIN_PrintTemperatureWindow    110022
#define ID_MAIN_TextView15    50033
#define ID_MAIN_PrintSpeedFactor    50023
#define ID_MAIN_PrintVolFlow    50022
#define ID_MAIN_PrintTopSpeed    50021
#define ID_MAIN_PrintRequestedSpeed    50020
#define ID_MAIN_TextView8    50019
#define ID_MAIN_PrintSpeedMultiplierBar    91001
#define ID_MAIN_TextView7    50018
#define ID_MAIN_PrintExtruderPositionListSubItem1    24020
#define ID_MAIN_PrintExtruderPositionList    80008
#define ID_MAIN_TextView6    50017
#define ID_MAIN_PrintPositionListMachinePositionSubItem    24026
#define ID_MAIN_PrintPositionListUserPositionSubItem    24019
#define ID_MAIN_PrintPositionList    80007
#define ID_MAIN_PrintPositionWindow    110021
#define ID_MAIN_PrintResumeBtn    20033
#define ID_MAIN_PrintCancelBtn    20032
#define ID_MAIN_PrintPauseBtn    20031
#define ID_MAIN_PrintPauseWindow    110020
#define ID_MAIN_TextView14    50032
#define ID_MAIN_PrintFanList    80009
#define ID_MAIN_PrintFansWindow    110019
#define ID_MAIN_PrintBabystepIncBtn    20030
#define ID_MAIN_PrintBabystepDecBtn    20029
#define ID_MAIN_PrintBabystepCurrentOffset    50016
#define ID_MAIN_TextView5    50015
#define ID_MAIN_PrintBabystepWindow    110018
#define ID_MAIN_PrintEstimatedTime    50014
#define ID_MAIN_PrintElapsedTime    50013
#define ID_MAIN_PrintProgressBar    130001
#define ID_MAIN_PrintFileName    50012
#define ID_MAIN_PrintJobInfoWindow    110017
#define ID_MAIN_PrintWindow    110016
#define ID_MAIN_TextView39    50069
#define ID_MAIN_TextView38    50068
#define ID_MAIN_ScreensaverTimeoutInput    51009
#define ID_MAIN_ScreensaverEnable    21002
#define ID_MAIN_TextView37    50067
#define ID_MAIN_ScreensaverSettingWindow    110035
#define ID_MAIN_TextView25    50052
#define ID_MAIN_GuidesList    80016
#define ID_MAIN_GuideSelectionWindow    110030
#define ID_MAIN_TextView23    50049
#define ID_MAIN_InfoTimeoutInput    51008
#define ID_MAIN_TextView22    50048
#define ID_MAIN_PollIntervalInput    51007
#define ID_MAIN_TextView20    50046
#define ID_MAIN_BaudRateList    80011
#define ID_MAIN_DuetUartCommSettingWindow    110028
#define ID_MAIN_PasswordInput    51006
#define ID_MAIN_TextView21    50047
#define ID_MAIN_HostnameInput    51005
#define ID_MAIN_TextView19    50045
#define ID_MAIN_DuetNetworkCommSettingWindow    110027
#define ID_MAIN_DuetCommList    80015
#define ID_MAIN_DuetCommSettingWindow    110026
#define ID_MAIN_SettingsSlideWindow    30002
#define ID_MAIN_SettingsWindow    110009
#define ID_MAIN_NetworkWindow    110012
#define ID_MAIN_UsbFiles    20039
#define ID_MAIN_FolderID    50011
#define ID_MAIN_FileRefreshBtn    20028
#define ID_MAIN_FileTypeSubItem    24018
#define ID_MAIN_FileDateSubItem    24017
#define ID_MAIN_FileSizeSubItem    24016
#define ID_MAIN_FileThumbnailSubItem    24015
#define ID_MAIN_FileListView    80006
#define ID_MAIN_FilesWindow    110011
#define ID_MAIN_FanWindow    110015
#define ID_MAIN_HeightMapWindow    110014
#define ID_MAIN_ConsoleMacroBtn3    20044
#define ID_MAIN_ConsoleMacroBtn2    20043
#define ID_MAIN_ConsoleMacroBtn1    20042
#define ID_MAIN_ConsoleClearBtn    20027
#define ID_MAIN_SendBtn    20026
#define ID_MAIN_TextView4    50010
#define ID_MAIN_TextView3    50009
#define ID_MAIN_EditText1    51001
#define ID_MAIN_GcodeListView    80005
#define ID_MAIN_ConsoleListView    80004
#define ID_MAIN_ConsoleWindow    110008
#define ID_MAIN_FilamentControlHeading    50066
#define ID_MAIN_UnloadFilamentBtn    20051
#define ID_MAIN_TextView36    50065
#define ID_MAIN_FilamentList    80020
#define ID_MAIN_FilamentLoadUnloadWindow    110034
#define ID_MAIN_ExtrudeBtn    20049
#define ID_MAIN_RetractBtn    20048
#define ID_MAIN_TextView33    50062
#define ID_MAIN_TextView32    50061
#define ID_MAIN_TextView31    50060
#define ID_MAIN_TextView34    50063
#define ID_MAIN_TextView35    50064
#define ID_MAIN_TextView30    50059
#define ID_MAIN_ExtrudeToolNameSubItem    24034
#define ID_MAIN_ExtrudeToolStatusSubItem    24033
#define ID_MAIN_ExtrudeToolStandbyTemperatureSubItem    24032
#define ID_MAIN_ExtrudeToolActiveTemperatureSubItem    24031
#define ID_MAIN_ExtrudeToolCurrentTemperatureSubItem    24030
#define ID_MAIN_ExtrudeToolFilamentSubItem    24029
#define ID_MAIN_ExtrudeToolList    80019
#define ID_MAIN_TextView29    50058
#define ID_MAIN_TextView28    50057
#define ID_MAIN_TextView27    50056
#define ID_MAIN_ExtruderFeedrate    80018
#define ID_MAIN_ExtruderFeedDist    80017
#define ID_MAIN_ExtrudeWindow    110010
#define ID_MAIN_DisableMotorsBtn    20038
#define ID_MAIN_TextView17    50038
#define ID_MAIN_TextView16    50037
#define ID_MAIN_FeedrateBtn5    20025
#define ID_MAIN_FeedrateBtn4    20024
#define ID_MAIN_FeedrateBtn3    20023
#define ID_MAIN_FeedrateBtn2    20022
#define ID_MAIN_TextView2    50008
#define ID_MAIN_HeightmapBtn    20021
#define ID_MAIN_MeshLevelBtn    20020
#define ID_MAIN_TrueLevelBtn    20019
#define ID_MAIN_FeedrateBtn1    20018
#define ID_MAIN_AxisControlMachinePositionSubItem    24028
#define ID_MAIN_AxisControlUserPositionSubItem    24027
#define ID_MAIN_AxisControlSubItem8    24014
#define ID_MAIN_AxisControlSubItem7    24013
#define ID_MAIN_AxisControlSubItem6    24012
#define ID_MAIN_AxisControlSubItem5    24011
#define ID_MAIN_AxisControlSubItem4    24010
#define ID_MAIN_AxisControlSubItem3    24009
#define ID_MAIN_AxisControlSubItem2    24008
#define ID_MAIN_AxisControlSubItem1    24007
#define ID_MAIN_AxisControlHomeSubItem    24006
#define ID_MAIN_AxisControlListView    80003
#define ID_MAIN_HomeAllBtn    20017
#define ID_MAIN_MoveWindow    110007
#define ID_MAIN_SlideWindow1    30001
#define ID_MAIN_WindowSelectWindow    110005
#define ID_MAIN_TemperatureGraphLegend    80002
#define ID_MAIN_TempGraph    120001
#define ID_MAIN_TemperatureGraphWindow    110001
#define ID_MAIN_ToolListHeadingStandby    50005
#define ID_MAIN_ToolListHeadingActive    50004
#define ID_MAIN_ToolListHeadingCurrent    50002
#define ID_MAIN_ToolListHeadingStatus    50007
#define ID_MAIN_ToolListHeadingName    50001
#define ID_MAIN_ToolStatusSubItem    24005
#define ID_MAIN_ToolStandbyTemperatureSubItem    24004
#define ID_MAIN_ToolActiveTemperatureSubItem    24003
#define ID_MAIN_ToolCurrentTemperatureSubItem    24002
#define ID_MAIN_ToolNameSubItem    24001
#define ID_MAIN_ToolListView    80001
#define ID_MAIN_ToolListWindow    110004
#define ID_MAIN_MainWindow    110002
#define ID_MAIN_StatusText    50035
#define ID_MAIN_ConsoleBtn    20035
#define ID_MAIN_MacroBtn    20004
#define ID_MAIN_BackBtn    20003
#define ID_MAIN_HomeBtn    20001
#define ID_MAIN_SideBarWindow    110003
#define ID_MAIN_RootWindow    110036
/*TAG:Macro宏ID END*/

class mainActivity : public Activity, 
                     public ZKSeekBar::ISeekBarChangeListener, 
                     public ZKListView::IItemClickListener,
                     public ZKListView::AbsListAdapter,
                     public ZKSlideWindow::ISlideItemClickListener,
                     public EasyUIContext::ITouchListener,
                     public ZKCheckBox::ICheckedChangeListener,
                     public ZKEditText::ITextChangeListener,
                     public ZKVideoView::IVideoPlayerMessageListener
{
public:
    mainActivity();
    virtual ~mainActivity();

    /**
     * 注册定时器
     */
	void registerUserTimer(int id, int time);
	/**
	 * 取消定时器
	 */
	void unregisterUserTimer(int id);
	/**
	 * 重置定时器
	 */
	void resetUserTimer(int id, int time);

protected:
    /*TAG:PROTECTED_FUNCTION*/
    virtual const char* getAppName() const;
    virtual void onCreate();
    virtual void onClick(ZKBase *pBase);
    virtual void onResume();
    virtual void onPause();
    virtual void onIntent(const Intent *intentPtr);
    virtual bool onTimer(int id);

    virtual void onProgressChanged(ZKSeekBar *pSeekBar, int progress);

    virtual int getListItemCount(const ZKListView *pListView) const;
    virtual void obtainListItemData(ZKListView *pListView, ZKListView::ZKListItem *pListItem, int index);
    virtual void onItemClick(ZKListView *pListView, int index, int subItemIndex);

    virtual void onSlideItemClick(ZKSlideWindow *pSlideWindow, int index);

    virtual bool onTouchEvent(const MotionEvent &ev);
    virtual void onCheckedChanged(ZKCheckBox* pCheckBox, bool isChecked);

    virtual void onTextChanged(ZKTextView *pTextView, const string &text);

    void rigesterActivityTimer();

    virtual void onVideoPlayerMessage(ZKVideoView *pVideoView, int msg);
    void videoLoopPlayback(ZKVideoView *pVideoView, int msg, size_t callbackTabIndex);
    void startVideoLoopPlayback();
    void stopVideoLoopPlayback();
    bool parseVideoFileList(const char *pFileListPath, std::vector<string>& mediaFileList);
    int removeCharFromString(string& nString, char c);


private:
    /*TAG:PRIVATE_VARIABLE*/
    int mVideoLoopIndex;
    int mVideoLoopErrorCount;

};

#endif