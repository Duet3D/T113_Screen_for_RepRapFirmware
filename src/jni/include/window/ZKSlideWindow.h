/*
 * ZKSlideWindow.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Jul 3, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _WINDOW_ZKSLIDEWINDOW_H_
#define _WINDOW_ZKSLIDEWINDOW_H_

#include "ZKWindow.h"

class ZKSlideWindowPrivate;

/**
 * @brief Slide window control
 */
class ZKSlideWindow : public ZKWindow {
	ZK_DECLARE_PRIVATE(ZKSlideWindow)

public:
	ZKSlideWindow(ZKBase *pParent);
	virtual ~ZKSlideWindow();

public:
  /**
   * @brief Slide item click listener interface
   */
  class ISlideItemClickListener
  {
	public:
		virtual ~ISlideItemClickListener() { }
		virtual void onSlideItemClick(ZKSlideWindow *pSlideWindow, int index) = 0;
	};

	void setSlideItemClickListener(ISlideItemClickListener *pListener);

	/**
	 * @brief Slide page change listener interface
	 */
	class ISlidePageChangeListener {
	public:
		virtual ~ISlidePageChangeListener() { }
		virtual void onSlidePageChange(ZKSlideWindow *pSlideWindow, int page) = 0;
	};

	void setSlidePageChangeListener(ISlidePageChangeListener *pListener);

	/**
	 * @brief Get the current page position
	 */
	int getCurrentPage() const;

	/**
	 * @brief Get the total number of pages
	 */
	int getPageSize() const;

	/**
	 * @brief Switch to the next page
	 * @param isAnimatable Whether to enable page flipping animation, default is false, animation is disabled
	 */
	void turnToNextPage(bool isAnimatable = false);

	/**
	 * @brief Switch to the previous page
	 * @param isAnimatable Whether to enable page flipping animation, default is false, animation is disabled
	 */
	void turnToPrevPage(bool isAnimatable = false);

protected:
	ZKSlideWindow(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_SLIDEWINDOW; }

	virtual void onDraw(ZKCanvas *pCanvas);
	virtual bool onTouchEvent(const MotionEvent &ev);
	virtual void onTimer(int id);

private:
	void parseSlideWindowAttributeFromJson(const Json::Value &json);
};

#endif /* _WINDOW_ZKSLIDEWINDOW_H_ */
