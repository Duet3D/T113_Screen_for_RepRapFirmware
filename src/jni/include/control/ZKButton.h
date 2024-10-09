/*
 * ZKButton.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Jun 10, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _CONTROL_ZKBUTTON_H_
#define _CONTROL_ZKBUTTON_H_

#include "ZKTextView.h"

class ZKButtonPrivate;

/**
 * @brief Button control
 */
class ZKButton : public ZKTextView {
	ZK_DECLARE_PRIVATE(ZKButton)

public:
	ZKButton(ZKBase *pParent);
	virtual ~ZKButton();

	/**
	 * @brief Set the background image for different button states
	 * @param status Button state
	 *    Normal state: ZK_CONTROL_STATUS_NORMAL
	 *    Pressed state: ZK_CONTROL_STATUS_PRESSED
	 *    Selected state: ZK_CONTROL_STATUS_SELECTED
	 *    Selected pressed state: ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED
	 *    Invalid state: ZK_CONTROL_STATUS_INVALID
	 * @param pPicPath Image path
	 */
	void setButtonStatusPic(int status, const char *pPicPath);

	/**
	 * @brief Set the icon position
	 * @param position Position
	 */
	void setIconPosition(const LayoutPosition &position);

protected:
	ZKButton(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_BUTTON; }

	virtual void onDraw(ZKCanvas *pCanvas);

private:
	void parseButtonAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKBUTTON_H_ */
