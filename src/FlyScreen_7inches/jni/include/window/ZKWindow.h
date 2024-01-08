/*
 * ZKWindow.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 * Created on: Jun 10, 2017
 * Author: zkswe@zkswe.com
 */

#ifndef _WINDOW_ZKWINDOW_H_
#define _WINDOW_ZKWINDOW_H_

#include <vector>
#include "control/ZKBase.h"

class ZKWindowPrivate;

/**
 * @brief Window control
 */
class ZKWindow : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKWindow)

public:
	ZKWindow(ZKBase *pParent);
	virtual ~ZKWindow();

	bool isModalWindow() const;

	virtual ZKBase* findControlByID(int id);
	virtual void getAllControls(std::vector<ZKBase*> &controlsList);

	/**
	 * @brief Show the window
	 */
	void showWnd();

	/**
	 * @brief Hide the window
	 */
	void hideWnd();

	/**
	 * @brief Check if the window is visible
	 */
	bool isWndShow() const;

	/**
	 * @brief Set the child control's z-order position (Non-thread-safe interface, only allowed to be called on the UI thread)
	 * @param pChild    Child control
	 * @param index     Position
	 */
	void setChildIndex(ZKBase *pChild, int index);

protected:
	ZKWindow(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual void onAfterCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_WINDOW; }

	virtual bool onInterceptMessage(const struct _message_t *pMsg);
	virtual bool onTouchEvent(const MotionEvent &ev);
	virtual void onTimer(int id);

	bool isWindowClass(const char *pClassName) const;
	bool checkHideModalWindow(int x, int y);

private:
	void parseWindowAttributeFromJson(const Json::Value &json);
	void parseControlsFromJson(const Json::Value &json);
};

#endif /* _WINDOW_ZKWINDOW_H_ */
