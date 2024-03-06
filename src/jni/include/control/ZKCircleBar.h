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
 * @brief 圆形进度条控件
 */
class ZKCircleBar : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKCircleBar)

public:
	ZKCircleBar(ZKBase *pParent);
	virtual ~ZKCircleBar();

	/**
	 * @brief 设置最大进度值
	 */
	void setMax(int max);

	/**
	 * @brief 获取最大进度值
	 */
	int getMax() const;

	/**
	 * @brief 设置当前进度值
	 */
	void setProgress(int progress);

	/**
	 * @brief 获取当前进度值
	 */
	int getProgress() const;

	/**
	 * @brief 设置起始角度
	 */
	void setStartAngle(int angle);

	/**
	 * @brief 设置文本颜色
	 * @param color 颜色值为0x ARGB
	 */
	void setTextColor(int color);

	/**
	 * @brief 设置进度图片
	 */
	void setProgressPic(const char *pPicPath);

	/**
	 * @brief 设置滑块图片
	 * @param status   状态
	 *    正常状态： ZK_CONTROL_STATUS_NORMAL
	 *    按下状态： ZK_CONTROL_STATUS_PRESSED
	 * @param pPicPath 图片路径
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
