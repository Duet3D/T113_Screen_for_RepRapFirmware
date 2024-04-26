/*
 * ZKSeekBar.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Jun 26, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _CONTROL_ZKSEEKBAR_H_
#define _CONTROL_ZKSEEKBAR_H_

#include "ZKBase.h"

class ZKSeekBarPrivate;

/**
 * @brief Slider control
 */
class ZKSeekBar : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKSeekBar)

public:
	ZKSeekBar(ZKBase *pParent);
	virtual ~ZKSeekBar();

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
	class ISeekBarChangeListener {
	public:
		virtual ~ISeekBarChangeListener() { }
		virtual void onProgressChanged(ZKSeekBar *pSeekBar, int progress) = 0;
		virtual void onStartTrackingTouch(ZKSeekBar *pSeekBar) { }
		virtual void onStopTrackingTouch(ZKSeekBar *pSeekBar) { }
	};

	void setSeekBarChangeListener(ISeekBarChangeListener *pListener);

protected:
	ZKSeekBar(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_SEEKBAR; }

	virtual void onDraw(ZKCanvas *pCanvas);
	virtual bool onTouchEvent(const MotionEvent &ev);

private:
	void parseSeekBarAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKSEEKBAR_H_ */
