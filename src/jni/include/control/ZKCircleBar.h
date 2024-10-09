/*
 * ZKCircleBar.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Sep 5, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _CONTROL_ZKCIRCLEBAR_H_
#define _CONTROL_ZKCIRCLEBAR_H_

#include "ZKBase.h"

class ZKCircleBarPrivate;

/**
 * @brief Circular progress bar control
 */
class ZKCircleBar : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKCircleBar)

public:
	ZKCircleBar(ZKBase *pParent);
	virtual ~ZKCircleBar();

	/**
	 * @brief Set the maximum progress value
	 */
	void setMax(int max);

	/**
	 * @brief Get the maximum progress value
	 */
	int getMax() const;

	/**
	 * @brief Set the current progress value
	 */
	void setProgress(int progress);

	/**
	 * @brief Get the current progress value
	 */
	int getProgress() const;

	/**
	 * @brief Set the starting angle
	 */
	void setStartAngle(int angle);

	/**
	 * @brief Set the text color
	 * @param color Color value in the format 0xARGB
	 */
	void setTextColor(int color);

	/**
	 * @brief Set the progress image
	 */
	void setProgressPic(const char *pPicPath);

	/**
	 * @brief Set the thumb image
	 * @param status   Status
	 *    Normal state: ZK_CONTROL_STATUS_NORMAL
	 *    Pressed state: ZK_CONTROL_STATUS_PRESSED
	 * @param pPicPath Image path
	 */
	void setThumbPic(int status, const char *pPicPath);

public:
	class ICircleBarChangeListener {
	public:
		virtual ~ICircleBarChangeListener() { }
		virtual void onProgressChanged(ZKCircleBar *pCircleBar, int progress) = 0;
		virtual void onStartTrackingTouch(ZKCircleBar *pCircleBar) { }
		virtual void onStopTrackingTouch(ZKCircleBar *pCircleBar) { }
	};

	void setCircleBarChangeListener(ICircleBarChangeListener *pListener);

protected:
	ZKCircleBar(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_CIRCLEBAR; }

	virtual void onDraw(ZKCanvas *pCanvas);
	virtual bool onTouchEvent(const MotionEvent &ev);

private:
	void parseCircleBarAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKCIRCLEBAR_H_ */
