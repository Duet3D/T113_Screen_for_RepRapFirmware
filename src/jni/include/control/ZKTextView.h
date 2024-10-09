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
#include "utils/utils.h"
#include <cstdarg>

class ZKTextViewPrivate;

/**
 * @brief Text control
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
	 * @brief Set string text
	 */
	void setText(const std::string &text);

	/**
	 * @brief Set string text
	 */
	void setText(const char *text);

	/**
	 * @brief Set character text
	 */
	void setText(char text);

	/**
	 * @brief Set numeric text
	 */
	void setText(int text);

	void setTextf(const char* fmt, va_list args)
	{
		std::string tmp = utils::format(fmt, args);
		setText(tmp);
	}

	void setTextf(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		setTextf(fmt, args);
		va_end(args);
	}

	void setText(float text) { setTextf("%.2f", text); }

	/**
	 * @brief Get text content
	 */
	const std::string& getText() const;

	/**
	 * @brief Support for multi-language setting interface
	 */
	void setTextTr(const char *name);

	void setTextTrf(const char *format_id, ...)
	{
		// TODO: implement this so it doesn't give a compiler warning
		va_list vargs;
		va_start(vargs, format_id);
		std::string format = LANGUAGEMANAGER->getValue(format_id);
		setTextf(format.c_str(), vargs);
		va_end(vargs);
	}

	void reloadTextTr();

	/**
	 * @brief Set text color
	 * @param color Color value in 0xARGB format
	 */
	void setTextColor(int color);

	/**
	 * @brief Set text status color
	 * @param status Status
	 *    Normal state: ZK_CONTROL_STATUS_NORMAL
	 *    Pressed state: ZK_CONTROL_STATUS_PRESSED
	 *    Selected state: ZK_CONTROL_STATUS_SELECTED
	 *    Pressed and selected state: ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED
	 *    Invalid state: ZK_CONTROL_STATUS_INVALID
	 * @param color Color value in 0xARGB format
	 */
	void setTextStatusColor(int status, uint32_t color);

	/**
	 * @brief Get text content width and height
	 * @param text Text content
	 * @param w Return text width
	 * @param h Return text height
	 */
	void getTextExtent(const char *text, int &w, int &h);

	/**
	 * @brief Set text size
	 * @param size Text size
	 */
	void setTextSize(uint32_t size);

	/**
	 * @brief Get text size
	 */
	uint32_t getTextSize() const;

	/**
	 * @brief Set text row spacing
	 * @param space Spacing value
	 */
	void setTextRowSpace(int space);

	/**
	 * @brief Set text column spacing
	 * @param space Spacing value
	 */
	void setTextColSpace(int space);

	/**
	 * @brief Set font
	 * @param family Font name
	 */
	void setFontFamily(const char *family);

	/**
	 * @brief Set text long display mode
	 */
	void setLongMode(ELongMode mode);

public:
  /**
   * @brief Text content change listener interface
   */
  class ITextChangeListener
  {
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
