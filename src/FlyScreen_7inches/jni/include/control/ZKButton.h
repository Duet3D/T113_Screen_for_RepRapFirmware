/*
 * ZKButton.h
 *
 *  Created on: Jun 10, 2017
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKBUTTON_H_
#define _CONTROL_ZKBUTTON_H_

#include "ZKTextView.h"

class ZKButtonPrivate;

/**
 * @brief 按钮控件
 */
class ZKButton : public ZKTextView {
	ZK_DECLARE_PRIVATE(ZKButton)

public:
	ZKButton(HWND hParentWnd);
	virtual ~ZKButton();

	/**
	 * @brief 设置状态背景图
	 * @param status 状态
	 *    正常状态： E_CONTROL_STATUS_NORMAL
	 *    按下状态： E_CONTROL_STATUS_PRESSED
	 *    选中状态： E_CONTROL_STATUS_SELECTED
	 *    选中按下状态： E_CONTROL_STATUS_PRESSED | E_CONTROL_STATUS_SELECTED
	 *    无效状态： E_CONTROL_STATUS_INVALID
	 * @param pPicPath 图片路径
	 */
	void setButtonStatusPic(int status, const char *pPicPath);

	/**
	 * @brief 设置图标位置
	 * @param position 位置
	 */
	void setIconPosition(const LayoutPosition &position);

protected:
	ZKButton(HWND hParentWnd, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_BUTTON; }

	virtual void onDraw(HDC hdc);

	void _section_(zk) drawButton(HDC hdc);

	const PBITMAP getCurStatusPic() const;

private:
	void _section_(zk) parseButtonAttributeFromJson(const Json::Value &json);

protected:
	PBITMAP mButtonStatusPicTab[S_CONTROL_STATUS_TAB_LEN];
	LayoutPosition mIconPosition;
};

#endif /* _CONTROL_ZKBUTTON_H_ */
