#ifndef _SYSAPP_SCREENSAVER_H_
#define _SYSAPP_SCREENSAVER_H_

#include "entry/EasyUIContext.h"
#include "app/SysAppFactory.h"

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
#include "window/ZKSlideWindow.h"
/*TAG:Macro宏ID*/
#define ID_SCREENSAVER_ScreensaverText    50001
/*TAG:Macro宏ID END*/
class screensaver : public BaseApp,
                  public ZKSeekBar::ISeekBarChangeListener, 
                  public ZKListView::IItemClickListener,
                  public ZKListView::AbsListAdapter,
                  public ZKSlideWindow::ISlideItemClickListener,
                  public EasyUIContext::ITouchListener,
                  public ZKEditText::ITextChangeListener,
                  public ZKVideoView::IVideoPlayerMessageListener
{
public:
	screensaver();
	virtual ~screensaver();
	virtual void onCreate();
	virtual void onClick(ZKBase *pBase);
	virtual bool onTimer(int id);

    virtual void onProgressChanged(ZKSeekBar *pSeekBar, int progress);

    virtual int getListItemCount(const ZKListView *pListView) const;
    virtual void obtainListItemData(ZKListView *pListView, ZKListView::ZKListItem *pListItem, int index);
    virtual void onItemClick(ZKListView *pListView, int index, int subItemIndex);

    virtual void onSlideItemClick(ZKSlideWindow *pSlideWindow, int index);

    virtual bool onTouchEvent(const MotionEvent &ev);

    virtual void onTextChanged(ZKTextView *pTextView, const string &text);

    void rigesterActivityTimer();

    virtual void onVideoPlayerMessage(ZKVideoView *pVideoView, int msg);
    void videoLoopPlayback(ZKVideoView *pVideoView, int msg, size_t callbackTabIndex);
    void startVideoLoopPlayback();
    void stopVideoLoopPlayback();
    bool parseVideoFileList(const char *pFileListPath, std::vector<string>& mediaFileList);
    int removeCharFromString(string& nString, char c);

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


private:
    /*TAG:PRIVATE_VARIABLE*/
    int mVideoLoopIndex;
    int mVideoLoopErrorCount;
};

#endif