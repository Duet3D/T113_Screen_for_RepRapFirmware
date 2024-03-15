/*
 * ZKTextView.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Jun 10, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _CONTROL_ZKTEXTVIEW_H_
#define _CONTROL_ZKTEXTVIEW_H_

#include "ZKBase.h"
#include "manager/LanguageManager.h"
#include <Duet3D/General/SafeVsnprintf.h>

class ZKTextViewPrivate;

/**
 * @brief 文本控件
 */
class ZKTextView : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKTextView)

public:
	ZKTextView(ZKBase *pParent);
	virtual ~ZKTextView();

	enum ELongMode {
		E_LONG_MODE_NONE,
		E_LONG_MODE_DOTS,
		E_LONG_MODE_SCROLL,
		E_LONG_MODE_SCROLL_CIRCULAR,
	};

	/**
	 * @brief 设置string文本
	 */
	void setText(const std::string &text);

	/**
	 * @brief 设置字符串文本
	 */
	void setText(const char *text);

	/**
	 * @brief 设置字符文本
	 */
	void setText(char text);

	/**
	 * @brief 设置数字文本
	 */
	void setText(int text);

	int setTextf(const char* fmt, va_list args)
	{
		std::string tmp;
		size_t ret = SafeVsnprintf(0, 0, fmt, args);
		if (ret >= tmp.capacity())
			tmp.reserve(ret + sizeof(char));
		tmp.resize(ret);
		ret = SafeVsnprintf((char*)tmp.data(), tmp.capacity(), fmt, args);
		setText(tmp);
		return ret;
	}

	int setTextf(const char* fmt, ...)
	{
		std::string tmp;
		va_list args;
		va_start(args, fmt);
		size_t ret = SafeVsnprintf(0, 0, fmt, args);
		if (ret >= tmp.capacity())
			tmp.reserve(ret + sizeof(char));
		tmp.resize(ret);
		SafeVsnprintf((char*)tmp.data(), tmp.capacity(), fmt, args);
		va_end(args);
		setText(tmp);
		return ret;
	}

	void setText(float text)
	{
		char str[10];
		SafeSnprintf(str, 10, "%.2f", text);
		setText(str);
	}

	/**
	 * @brief 获取文本内容
	 */
	const std::string& getText() const;

	/**
	 * @brief 支持多国语言设置接口
	 */
	void setTextTr(const char *name);

	int setTextTrf(const char *format_id, ...)
	{
		// TODO: implement this so it doesn't give a compiler warning
		va_list vargs;
        std::string format = LANGUAGEMANAGER->getValue(format_id);
        va_start(vargs, format.c_str());
		char buffer[255];
		const int ret = SafeVsnprintf(buffer, 255, format.c_str(), vargs);
		va_end(vargs);
		setText(buffer);
		return ret;
	}

	void reloadTextTr();

	/**
	 * @brief 设置文本颜色
	 * @param color 颜色值为0x ARGB
	 */
	void setTextColor(int color);

	/**
	 * @brief 设置文本状态颜色
	 * @param status 状态
	 *    正常状态： ZK_CONTROL_STATUS_NORMAL
	 *    按下状态： ZK_CONTROL_STATUS_PRESSED
	 *    选中状态： ZK_CONTROL_STATUS_SELECTED
	 *    选中按下状态： ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED
	 *    无效状态： ZK_CONTROL_STATUS_INVALID
	 * @param color 颜色值为0x ARGB
	 */
	void setTextStatusColor(int status, uint32_t color);

	/**
	 * @brief 获取文本内容宽高
	 * @param text 文本内容
	 * @param w 返回文本宽度
	 * @param h 返回文本高度
	 */
	void getTextExtent(const char *text, int &w, int &h);

	/**
	 * @brief 设置文本大小
	 * @param size 文本大小
	 */
	void setTextSize(uint32_t size);

	/**
	 * @brief 获取文本大小
	 */
	uint32_t getTextSize() const;

	/**
	 * @brief 设置文本行间距
	 * @param space 间距值
	 */
	void setTextRowSpace(int space);

	/**
	 * @brief 设置文本列间距
	 * @param space 间距值
	 */
	void setTextColSpace(int space);

	/**
	 * @brief 设置字体
	 * @param family 字体名
	 */
	void setFontFamily(const char *family);

	/**
	 * @brief 设置文本超长显示模式
	 */
	void setLongMode(ELongMode mode);

public:
	/**
	 * @brief 文本内容改变监听接口
	 */
	class ITextChangeListener {
	public:
		virtual ~ITextChangeListener() { }
		virtual void onTextChanged(ZKTextView *pTextView, const std::string &text) = 0;
	};

	void setTextChangeListener(ITextChangeListener *pListener);

protected:
	ZKTextView(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual void onAfterCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_TEXTVIEW; }

	virtual void onDraw(ZKCanvas *pCanvas);
	virtual void onTimer(int id);

private:
	void parseTextViewAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKTEXTVIEW_H_ */
