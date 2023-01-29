/*
 * UsbSwitchHelper.h
 *
 *  Created on: Aug 22, 2020
 *      Author: ZKSWE Develop Team
 */

#ifndef _UTILS_USB_SWITCH_HELPER_H_
#define _UTILS_USB_SWITCH_HELPER_H_

#include "system/Thread.h"

typedef enum {
	E_USB_MODE_NULL,		// 无usb状态
	E_USB_MODE_DEVICE,		// usb从设备，即adb模式
	E_USB_MODE_HOST,		// usb主设备，即u盘模式
} EUsbMode;

class UsbSwitchHelper {
public:
	class IUsbModeListener {
	public:
		virtual ~IUsbModeListener() { }
		virtual void onModeUpdate(EUsbMode mode) = 0;
	};

	void setUsbModeListener(IUsbModeListener *pListener) {
		mUsbModeListenerPtr = pListener;
	}

	void setUsbMode(EUsbMode mode);
	EUsbMode getUsbMode() const;

	static UsbSwitchHelper* getInstance();

private:
	UsbSwitchHelper();

	class SwitchThread : public Thread {
	public:
		SwitchThread(UsbSwitchHelper &helper) : mHelper(helper), mReqMode(E_USB_MODE_NULL) { }

		void setMode(EUsbMode mode) {
			mReqMode = mode;
			run("switch");
		}

	protected:
		bool threadLoop();

	private:
		UsbSwitchHelper &mHelper;
		EUsbMode mReqMode;
	};

protected:
	IUsbModeListener *mUsbModeListenerPtr;
	SwitchThread mSwitchThread;
};

#define USBSWITCHHELPER		UsbSwitchHelper::getInstance()

#endif /* _UTILS_USB_SWITCH_HELPER_H_ */
