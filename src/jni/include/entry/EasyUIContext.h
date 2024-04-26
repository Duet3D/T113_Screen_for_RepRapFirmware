/*
 * EasyUIContext.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Aug 24, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _ENTRY_EASYUI_CONTEXT_H_
#define _ENTRY_EASYUI_CONTEXT_H_

#include <vector>
#include <stddef.h>

#include "control/Common.h"
#include "system/Mutex.h"
#include "ime/IMEContext.h"

class Intent;
class BaseApp;
class IMEBaseApp;

class EasyUIContext {
public:
  /**
   * @brief Global touch listener interface
   */
  class ITouchListener
  {
	public:
		virtual ~ITouchListener() { }
		virtual bool onTouchEvent(const MotionEvent &ev) = 0;
	};

	/**
	 * @brief Register global touch listener interface
	 */
	void registerGlobalTouchListener(ITouchListener *pListener);

	/**
	 * @brief Unregister global touch listener interface
	 */
	void unregisterGlobalTouchListener(ITouchListener *pListener);

	bool notifyGlobalTouchListener(const MotionEvent &ev);

	/**
	 * @brief Key listener interface
	 */
	class IKeyListener {
	public:
		virtual ~IKeyListener() { }
		virtual bool onKeyEvent(const KeyEvent &ke) = 0;
	};

	/**
	 * @brief Register key listener interface
	 */
	void registerKeyListener(IKeyListener *pListener);

	/**
	 * @brief Unregister key listener interface
	 */
	void unregisterKeyListener(IKeyListener *pListener);

	bool notifyKeyListener(const KeyEvent &ke);

public:
	static EasyUIContext* getInstance();

	/**
	 * @brief Open activity
	 * @param appName Activity name
	 * @param intentPtr Data transfer object between activities, default is NULL
	 * @attention The intentPtr passed in must be a heap-allocated object, and will be deleted by the framework
	 * internally
	 */
	void openActivity(const char *appName, Intent *intentPtr = NULL);

	/**
	 * @brief Close activity
	 * @param appName Activity name
	 */
	void closeActivity(const char *appName);

	/**
	 * @brief Go back to the previous activity
	 */
	void goBack();

	/**
	 * @brief Go back to the home activity
	 */
	void goHome();

	/**
	 * @brief Current activity name
	 */
	const char* currentAppName() const;

	/**
	 * @brief Show status bar
	 */
	void showStatusBar();

	/**
	 * @brief Hide status bar
	 */
	void hideStatusBar();

	/**
	 * @brief Check if status bar is shown
	 */
	bool isStatusBarShow() const;

	BaseApp* getStatusBar() const;

	/**
	 * @brief Show navigation bar
	 */
	void showNaviBar();

	/**
	 * @brief Hide navigation bar
	 */
	void hideNaviBar();

	/**
	 * @brief Check if navigation bar is shown
	 */
	bool isNaviBarShow() const;

	BaseApp* getNaviBar() const;

	void powerOff();
	void powerOn();
	bool isPowerOff() const;

	/**
	 * @brief Set screensaver timeout
	 * @param timeOut Timeout in seconds, -1 means no screensaver
	 */
	void setScreensaverTimeOut(int timeOut);

	/**
	 * @brief Get screensaver timeout
	 */
	int getScreensaverTimeOut() const;

	/**
	 * @brief Reset screensaver timeout
	 */
	void resetScreensaverTimeOut();

	void performResetScreensaverTimeOut();

	/**
	 * @brief Set whether screensaver is enabled
	 * @param isEnable true to enable screensaver, false to disable screensaver
	 * @note Use case: For example, the upgrade activity should not enter screensaver mode. You can disable screensaver
	 * detection in the upgrade application by calling setScreensaverEnable(false), and restore screensaver
	 * functionality by calling setScreensaverEnable(true) when exiting.
	 */
	void setScreensaverEnable(bool isEnable);

	/**
	 * @brief Check if screensaver is enabled
	 */
	bool isScreensaverEnable() const;

	/**
	 * @brief Turn on screensaver
	 */
	void screensaverOn();

	/**
	 * @brief Turn off screensaver
	 */
	void screensaverOff();

	/**
	 * @brief Check if screensaver is active
	 */
	bool isScreensaverOn() const;

	void performScreensaverOn();
	void performScreensaverOff();

	void showIME(IMEContext::SIMETextInfo *pInfo, IMEContext::IIMETextUpdateListener *pListener);
	void hideIME();
	void performHideIME();

	/**
	 * @brief Check if IME is shown
	 */
	bool isIMEShow() const;

	/**
	 * @brief Switches the language for localization
	 * @param code Language code
	 */
	void updateLocalesCode(const char *code);

	bool initEasyUI();
	void deinitEasyUI();
	void runEasyUI();
	void reqExit();

private:
	EasyUIContext();
	~EasyUIContext();

	bool initLib();
	void deinitLib();

	void loadStatusBar();
	void loadNaviBar();

	void unloadStatusBar();
	void unloadNaviBar();

	void performEasyUIInit();
	void performEasyUIDeinit();
	void performStartupApp();
	void performLoadStatusBar();
	void performLoadNaviBar();
	void performScreensaver();

private:
	std::vector<ITouchListener *> mGlobalTouchListenerList;
	mutable Mutex mTouchLock;

	std::vector<IKeyListener *> mKeyListenerList;
	mutable Mutex mKeyLock;

	BaseApp *mStatusBarPtr;
	BaseApp *mNaviBarPtr;
	BaseApp *mScreensaverPtr;

	IMEBaseApp *mIMEPtr;

	struct {
		void (*onEasyUIInit)(EasyUIContext*);
		void (*onEasyUIDeinit)(EasyUIContext*);
		const char* (*onStartupApp)(EasyUIContext*);
	} mLibFunInfo;

	void *mLibHandle;
	void *mInternalLibHandle;

	bool mIsUpgradeMode;

	int mScreensaverTimeOut;
	bool mIsScreensaverEnable;

	void *mReservePtr;
};

#define EASYUICONTEXT			EasyUIContext::getInstance()

#endif /* _ENTRY_EASYUI_CONTEXT_H_ */
