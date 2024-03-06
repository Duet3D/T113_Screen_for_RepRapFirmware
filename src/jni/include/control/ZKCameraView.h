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
	E_MIRROR_NORMAL = 0,	// 正常镜像
	E_MIRROR_UPPER_LOWER,	// 上下镜像
	E_MIRROR_LEFT_RIGHT		// 左右镜像
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
 * @brief 摄像头控件
 */
class ZKCameraView : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKCameraView)

public:
	ZKCameraView(ZKBase *pParent);
	virtual ~ZKCameraView();

	/**
	 * @brief 设置分辨率大小
	 */
	void setFormatSize(int width, int height);

	/**
	 * @brief 开始预览
	 */
	ECameraStatusCode startPreview();

	/**
	 * @brief 停止预览
	 */
	ECameraStatusCode stopPreview();

	/**
	 * @brief 是否预览中
	 */
	bool isPreviewing() const;

	/**
	 * @brief 拍照
	 */
	ECameraStatusCode takePicture();

	void setPosition(const LayoutPosition &position);
	void setCropPosition(const LayoutPosition &position);

	void setRotation(ERotation rotation);
	void setMirror(EMirror mirror);

	/* 设置亮度 */
	bool setBrightness(int brightness);
	/* 设置对比度 */
	bool setContrast(int contrast);
	/* 设置饱和度 */
	bool setSaturation(int saturation);
	/* 设置色调 */
	bool setHue(int hue);
	/* 设置伽马值 */
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
