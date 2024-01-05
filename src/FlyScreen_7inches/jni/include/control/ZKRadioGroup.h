/*
 * ZKRadioGroup.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Oct 23, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _CONTROL_ZKRADIOGROUP_H_
#define _CONTROL_ZKRADIOGROUP_H_

#include "ZKCheckBox.h"

class ZKRadioGroupPrivate;

class ZKRadioGroup : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKRadioGroup)

public:
	ZKRadioGroup(ZKBase *pParent);
	virtual ~ZKRadioGroup();

	/**
	 * @brief 设置选中项
	 * @param id 子项id值
	 */
	void setCheckedID(int id);

	/**
	 * @brief 获取选中项
	 * @return 子项id值
	 */
	int getCheckedID() const;

	ZKCheckBox* getRadioItem(int index) const;

	void clearCheck();

	void reloadTextTr();

public:
	class ICheckedChangeListener {
	public:
		virtual ~ICheckedChangeListener() { }
		virtual void onCheckedChanged(ZKRadioGroup *pRadioGroup, int checkedID) = 0;
	};

	void setCheckedChangeListener(ICheckedChangeListener *pListener);

protected:
	ZKRadioGroup(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual void onAfterCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_RADIOGROUP; }

	virtual bool onTouchEvent(const MotionEvent &ev);

private:
	void parseRadioGroupAttributeFromJson(const Json::Value &json);
	void parseRadioButtonsAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKRADIOGROUP_H_ */
