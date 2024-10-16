/*
 * ZKDigitalClock.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Sep 19, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _CONTROL_ZKDIGITALCLOCK_H_
#define _CONTROL_ZKDIGITALCLOCK_H_

#include "ZKTextView.h"

class ZKDigitalClockPrivate;

/**
 * @brief Digital clock control
 */
class ZKDigitalClock : public ZKTextView {
	ZK_DECLARE_PRIVATE(ZKDigitalClock)

public:
	ZKDigitalClock(ZKBase *pParent);
	virtual ~ZKDigitalClock();

	void setHourFormat(bool is24HourFormat);

protected:
	ZKDigitalClock(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual void onAfterCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_DIGITALCLOCK; }

	virtual void onDraw(ZKCanvas *pCanvas);
	virtual void onTimer(int id);

private:
	void parseDigitalClockAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKDIGITALCLOCK_H_ */
