/*
 * ZKBase.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 * Created on: Jun 8, 2017
 * Author: zkswe@zkswe.com
 */

#ifndef _CONTROL_ZKBASE_H_
#define _CONTROL_ZKBASE_H_

#include <string>
#include <string.h>

#include "Common.h"
#include "json/json.h"

using namespace std;

class ZKCanvas;
class ZKBase;
class ZKBasePrivate;

class ZKBaseData {
public:
	virtual ~ZKBaseData() = 0;
	ZKBase *q_ptr;
};

/**
 * @brief Base control class
 */
class ZKBase {
	ZK_DECLARE_PRIVATE(ZKBase)
	friend class ControlFactory;
public:
	ZKBase(ZKBase *pParent);
	virtual ~ZKBase();

	bool create(const Json::Value &json);

	/**
	 * @brief Get the control ID
	 */
	int getID() const;

	/**
	 * @brief Set the control display position
	 */
	void setPosition(const LayoutPosition &position);

	/**
	 * @brief Get the control position
	 */
	const LayoutPosition& getPosition() const;

	/**
	 * @brief Set the invalid state
	 */
	void setInvalid(bool isInvalid);

	/**
	 * @brief Check if the control is in an invalid state
	 */
	bool isInvalid() const;

	/**
	 * @brief Set the visibility state
	 */
	void setVisible(bool isVisible);

	/**
	 * @brief Check if the control is visible
	 */
	bool isVisible() const;

	/**
	 * @brief Set the selected state
	 */
	void setSelected(bool isSelected);

	/**
	 * @brief Check if the control is selected
	 */
	bool isSelected() const;

	/**
	 * @brief Set the pressed state
	 */
	void setPressed(bool isPressed);

	/**
	 * @brief Check if the control is pressed
	 */
	bool isPressed() const;

	/**
	 * @brief Set the touchable state
	 */
	void setTouchable(bool isTouchable);

	/**
	 * @brief Check if the control is touchable
	 */
	bool isTouchable() const;

	/**
	 * @brief Set the touch pass-through
	 */
	void setTouchPass(bool isPass);

	/**
	 * @brief Check if the control has touch pass-through enabled
	 */
	bool isTouchPass() const;

	bool isWndValid() const;

	/**
	 * @brief Set the background image
	 * @param pPicPath Image path
	 */
	void setBackgroundPic(const char *pPicPath);
	void setBackgroundBmp(struct _bitmap_t *pBmp);

	/**
	 * @brief Set the background color
	 * @param color Color value in 0x ARGB format
	 */
	void setBackgroundColor(uint32_t color);

	/**
	 * @brief Set the background color for different control states
	 * @param status State
	 *    Normal state: ZK_CONTROL_STATUS_NORMAL
	 *    Pressed state: ZK_CONTROL_STATUS_PRESSED
	 *    Selected state: ZK_CONTROL_STATUS_SELECTED
	 *    Pressed and selected state: ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED
	 *    Invalid state: ZK_CONTROL_STATUS_INVALID
	 * @param color Color value in 0x ARGB format
	 */
	void setBgStatusColor(int status, uint32_t color);

	/**
	 * @brief Set the control opacity
	 * @param alpha Opacity value, range: 0 to 255, 0 means fully transparent, 255 means opaque
	 */
	void setAlpha(uint8_t alpha);

	/**
	 * @brief Produce a beep sound
	 */
	void beep();

	void setLayerIndex(int index);

	virtual const char* getClassName() const { return NULL; }

public:
	/**
	 * @brief Click event listener interface
	 */
	class IClickListener {
	public:
		virtual ~IClickListener() { }
		virtual void onClick(ZKBase *pBase) = 0;
	};

	/**
	 * @brief Long-press event listener interface
	 */
	class ILongClickListener {
	public:
		virtual ~ILongClickListener() { }
		virtual void onLongClick(ZKBase *pBase) = 0;
	};

	/**
	 * @brief Touch event listener interface
	 */
	class ITouchListener {
	public:
		virtual ~ITouchListener() { }
		virtual void onTouchEvent(ZKBase *pBase, const MotionEvent &ev) = 0;
	};

	void setClickListener(IClickListener *pListener);
	void setLongClickListener(ILongClickListener *pListener);
	void setTouchListener(ITouchListener *pListener);

protected:
	ZKBase(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual bool createWindow();
	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual void onAfterCreateWindow(const Json::Value &json);

	virtual ret_t procCtrlFun(const struct _message_t *pMsg);
	static ret_t ctrlFun(const struct _message_t *pMsg);

	virtual bool onInterceptMessage(const struct _message_t *pMsg);
	virtual void onDraw(ZKCanvas *pCanvas);
	virtual bool onTouchEvent(const MotionEvent &ev);
	virtual void onTimer(int id);

	void startTimer(int id, uint32_t time);
	void stopTimer(int id);
	void resetTimer(int id, uint32_t time);
	bool hasTimer(int id);

private:
	void parseBaseAttributeFromJson(const Json::Value &json);

protected:
	ZKBaseData *d_ptr;
};

#include <vector>

class ControlFactory {
public:
	static ControlFactory* getInstance();

	bool registerControl(const char *name, ZKBase* (*create)(ZKBase *pParent));
	ZKBase* create(const char *name, ZKBase *pParent);

private:
	ControlFactory() { }

private:
	typedef struct {
		std::string ctrlName;
		ZKBase* (*create)(ZKBase *pParent);
	} SControlInfo;

	std::vector<SControlInfo> mControlInfos;
};

#define CONTROLFACTORY		ControlFactory::getInstance()
#define REGISTER_CONTROL(name, _ctrl) \
    static struct _ControlFactory_##_ctrl { \
        static ZKBase* create(ZKBase *pParent) { \
            return new _ctrl(pParent); \
        } \
        _ControlFactory_##_ctrl() { \
        	CONTROLFACTORY->registerControl(name, create); \
        } \
    } _autoRegister_Control_##_ctrl

#endif /* _CONTROL_ZKBASE_H_ */
