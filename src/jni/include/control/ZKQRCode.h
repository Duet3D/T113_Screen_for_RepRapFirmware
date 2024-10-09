/*
 * ZKQRCode.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Oct 9, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _CONTROL_ZKQRCODE_H_
#define _CONTROL_ZKQRCODE_H_

#include "ZKBase.h"

class ZKQRCodePrivate;

/**
 * @brief QR Code control
 */
class ZKQRCode : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKQRCode)

public:
	ZKQRCode(ZKBase *pParent);
	virtual ~ZKQRCode();

	/**
	 * @brief Set foreground color
	 * @param color Color value in 0xARGB format
	 */
	void setForegroundColor(uint32_t color);

	/**
	 * @brief Load QR Code data
	 */
	bool loadQRCode(const char *pStr);

protected:
	ZKQRCode(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_QRCODE; }
	virtual void onDraw(ZKCanvas *pCanvas);

private:
	void parseQRCodeAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKQRCODE_H_ */
