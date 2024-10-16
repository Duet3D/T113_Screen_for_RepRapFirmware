/*
 * ZKEditText.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Oct 27, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _CONTROL_ZKEDITTEXT_H_
#define _CONTROL_ZKEDITTEXT_H_

#include "ZKTextView.h"

class ZKEditTextPrivate;

/**
 * @brief Editable text control
 */
class ZKEditText : public ZKTextView {
	ZK_DECLARE_PRIVATE(ZKEditText)

public:
	ZKEditText(ZKBase *pParent);
	virtual ~ZKEditText();

	/**
	 * @brief Set as password type
	 */
	void setPassword(bool isPassword);

	/**
	 * @brief Check if it is password type
	 */
	bool isPassword() const;

protected:
	ZKEditText(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_EDITTEXT; }

	virtual void onDraw(ZKCanvas *pCanvas);
	virtual bool onTouchEvent(const MotionEvent &ev);

private:
	void parseEditTextAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKEDITTEXT_H_ */
