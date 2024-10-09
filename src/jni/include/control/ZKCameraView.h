/*
 * ZKCameraView.h - Zkswe
 *
 * Copyright (C) 2018 Zkswe Technology Corp.
 *
 *  Created on: Mar 10, 2018
 *      Author: zkswe@zkswe.com
 */

#ifndef _CONTROL_ZKCAMERAVIEW_H_
#define _CONTROL_ZKCAMERAVIEW_H_

#include "ZKBase.h"

typedef enum {
	E_CAMERA_STATUS_CODE_OK = 0,
	E_CAMERA_STATUS_CODE_DEV_NOT_EXIST,
	E_CAMERA_STATUS_CODE_DEV_OPEN_FAILED,
	E_CAMERA_STATUS_CODE_TAKE_PIC_FAILED,
	E_CAMERA_STATUS_CODE_NO_SIGNAL,
	E_CAMERA_STATUS_CODE_HAS_SIGNAL,
	E_CAMERA_STATUS_CODE_UNKNOWN_ERRORNO
} ECameraStatusCode;

typedef enum {
	E_MIRROR_NORMAL = 0,  // Normal mirror
	E_MIRROR_UPPER_LOWER, // Mirror vertically (upper and lower)
	E_MIRROR_LEFT_RIGHT	  // Mirror horizontally (left and right)
} EMirror;

typedef enum {
	E_ROTATION_0 = 0,
	E_ROTATION_90,
	E_ROTATION_180,
	E_ROTATION_270
} ERotation;

class ZKCameraViewPrivate;
class ZKCamera;

/**
 * @brief Camera control
 */
class ZKCameraView : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKCameraView)

public:
	ZKCameraView(ZKBase *pParent);
	virtual ~ZKCameraView();

	/**
	 * @brief Set the resolution size
	 */
	void setFormatSize(int width, int height);

	/**
	 * @brief Start preview
	 */
	ECameraStatusCode startPreview();

	/**
	 * @brief Stop preview
	 */
	ECameraStatusCode stopPreview();

	/**
	 * @brief Check if preview is in progress
	 */
	bool isPreviewing() const;

	/**
	 * @brief Take a picture
	 */
	ECameraStatusCode takePicture();

	void setPosition(const LayoutPosition &position);
	void setCropPosition(const LayoutPosition &position);

	void setRotation(ERotation rotation);
	void setMirror(EMirror mirror);

	/* Set brightness */
	bool setBrightness(int brightness);
	/* Set contrast */
	bool setContrast(int contrast);
	/* Set saturation */
	bool setSaturation(int saturation);
	/* Set hue */
	bool setHue(int hue);
	/* Set gamma value */
	bool setGamma(int gamma);

public:
	class IPictureCallback {
	public:
		virtual ~IPictureCallback() { }
		virtual void onPictureTakenStarted() { }
		virtual void onPictureTakenEnd() { }
		virtual void onPictureTakenError() { }
		virtual const char* onPictureSavePath() = 0;
	};

	class IErrorCodeCallback {
	public:
		virtual ~IErrorCodeCallback() { }
		virtual void onErrorCode(int error) = 0;
	};

	void setPictureCallback(IPictureCallback *pCallback);
	void setErrorCodeCallback(IErrorCodeCallback *pCallback);

protected:
	ZKCameraView(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual void onAfterCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_CAMERAVIEW; }

	virtual void onDraw(ZKCanvas *pCanvas);

private:
	void parseCameraViewAttributeFromJson(const Json::Value &json);

private:
	ZKCamera *mCameraPtr;
};

#endif /* _CONTROL_ZKCAMERAVIEW_H_ */
