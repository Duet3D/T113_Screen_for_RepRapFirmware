/*
 * Popup.cpp
 *
 *  Created on: 23 Jan 2024
 *      Author: Andy Everitt
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "Popup.h"

#include "Comm/Communication.h"

#include "Comm/FileInfo.h"
#include "Hardware/Duet.h"
#include "Storage.h"
#include "UserInterface.h"
#include "storage/StoragePreferences.h"
#include "timer.h"
#include <algorithm>
#include <map>

namespace UI
{
	PopupWindow::PopupWindow() : m_okCb([]() {}), m_cancelCb([]() {}), m_mode(OM::Alert::Mode::None)
	{
		m_window = UI::GetUIControl<ZKWindow>(ID_MAIN_PopupWindow);
		m_noTouchWindow = UI::GetUIControl<ZKWindow>(ID_MAIN_NoTouchWindow);
		m_okBtn = UI::GetUIControl<ZKButton>(ID_MAIN_PopupOkBtn);
		m_cancelBtn = UI::GetUIControl<ZKButton>(ID_MAIN_PopupCancelBtn);
		m_title = UI::GetUIControl<ZKTextView>(ID_MAIN_PopupTitle);
		m_text = UI::GetUIControl<ZKTextView>(ID_MAIN_PopupText);
		m_warningText = UI::GetUIControl<ZKTextView>(ID_MAIN_PopupWarning);
		m_minText = UI::GetUIControl<ZKTextView>(ID_MAIN_PopupMin);
		m_maxText = UI::GetUIControl<ZKTextView>(ID_MAIN_PopupMax);
		m_choicesList = UI::GetUIControl<ZKListView>(ID_MAIN_PopupSelectionList);
		m_textInput = UI::GetUIControl<ZKEditText>(ID_MAIN_PopupTextInput);
		m_numberInput = UI::GetUIControl<ZKEditText>(ID_MAIN_PopupNumberInput);
		m_axisJogSelection = UI::GetUIControl<ZKListView>(ID_MAIN_PopupAxisSelection);
		m_axisJogAdjustment = UI::GetUIControl<ZKListView>(ID_MAIN_PopupAxisAdjusment);
		m_image = UI::GetUIControl<ZKTextView>(ID_MAIN_PopupImage);
		m_progress = UI::GetUIControl<ZKSeekBar>(ID_MAIN_PopupProgress);

		m_title->setLongMode(ZKTextView::E_LONG_MODE_SCROLL);
		SetTimeout(StoragePreferences::getInt(ID_INFO_TIMEOUT, DEFAULT_POPUP_TIMEOUT));
	}

	void PopupWindow::Open()
	{
		Open([]() {}, []() {});
	}

	void PopupWindow::Open(function<void(void)> okCb)
	{
		Open(okCb, []() {});
	}

	void PopupWindow::Open(function<void(void)> okCb, function<void(void)> cancelCb)
	{
		info("Opening popup window");
		if (IsOpen() && IsBlocking())
		{
			/* TODO
			 * Unsure if returning is needed, it might need adding if response messages clear a blocking M291 popup.
			 However returning here has an issue that if a blocking M291 is called immediately after clearing a previous
			 one from DWC. It will not appear on the screen. This is because the screen hasn't received an update from
			 the OM saying the previous blocking M291 has been cleared.
			 */
			warn("Blocking M291 alert already open, overwriting");
			// return;
		}

		Clear();

		UI::GetUIControl<ZKButton>(ID_MAIN_OverlayModalZone)->setVisible(true);

		if (m_timeout > 0)
		{
			registerDelayedCallback("popup_timeout", m_timeout, []() {
				UI::POPUP_WINDOW->Close();
				return false;
			});
		}

		m_cancelBtn->setVisible(true);
		m_okBtn->setVisible(true);

		SetOkBtnText(LANGUAGEMANAGER->getValue("ok").c_str());
		SetCancelBtnText(LANGUAGEMANAGER->getValue("cancel").c_str());
		SetPosition(VerticalPosition::center, HorizontalPosition::center);
		SetTextScrollable(false);
		WINDOW->OpenOverlay(m_window);
		m_okCb = okCb;
		m_cancelCb = cancelCb;
	}

	void PopupWindow::Ok()
	{
		m_okCb();
		Close();
	}

	void PopupWindow::Cancel(bool close)
	{
		if (!IsResponse())
		{
			Comm::DUET.SendGcode("M292 P1");
		}
		m_cancelCb();

		if (close)
			Close();
	}

	void PopupWindow::SetPosition(const VerticalPosition& vertical, const HorizontalPosition& horizontal)
	{
		LayoutPosition position = m_window->getPosition();
		switch (vertical)
		{
		case VerticalPosition::top:
			position.mLeft = 40;
			break;
		case VerticalPosition::center:
			position.mLeft = 125;
			break;
		case VerticalPosition::bottom:
			position.mLeft = 236;
			break;
		}
		switch (horizontal)
		{
		case HorizontalPosition::left:
			position.mLeft = 115;
			break;
		case HorizontalPosition::center:
			position.mLeft = 270;
			break;
		case HorizontalPosition::right:
			position.mLeft = 415;
			break;
		}
		m_window->setPosition(position);
	}

	void PopupWindow::SetTitle(const std::string& title)
	{
		m_title->setText(title);
	}

	void PopupWindow::SetText(const std::string& text)
	{
		m_text->setText(text);
	}

	void PopupWindow::SetText(const char* text)
	{
		m_text->setText(text);
	}

	void PopupWindow::SetTextf(const char* format, ...)
	{
		va_list vargs;
		va_start(vargs, format);
		m_text->setTextf(format, vargs);
		va_end(vargs);
	}

	void PopupWindow::SetTextScrollable(bool scrollable)
	{
		m_text->setLongMode(scrollable ? ZKTextView::E_LONG_MODE_SCROLL : ZKTextView::E_LONG_MODE_NONE);
	}

	void PopupWindow::SetOkBtnText(const char* text)
	{
		m_okBtn->setText(text);
	}

	void PopupWindow::SetCancelBtnText(const char* text)
	{
		m_cancelBtn->setText(text);
	}

	void PopupWindow::Close()
	{
		info("Closing popup window");
		Clear();
		WINDOW->CloseOverlay();
	}

	void PopupWindow::OkVisible(bool visible)
	{
		m_okBtn->setVisible(visible);
	}

	void PopupWindow::CancelVisible(bool visible)
	{
		m_cancelBtn->setVisible(visible);
	}

	void PopupWindow::SelectionVisible(bool visible)
	{
		m_choicesList->setVisible(visible);
	}

	void PopupWindow::NumberInputVisible(bool visible)
	{
		m_numberInput->setVisible(visible);
	}

	void PopupWindow::TextInputVisible(bool visible)
	{
		m_textInput->setVisible(visible);
	}

	void PopupWindow::WarningTextVisible(bool visible)
	{
		m_warningText->setVisible(visible);
	}

	void PopupWindow::MinTextVisible(bool visible)
	{
		m_minText->setVisible(visible);
	}

	void PopupWindow::MaxTextVisible(bool visible)
	{
		m_maxText->setVisible(visible);
	}

	void PopupWindow::AxisJogVisible(bool visible)
	{
		m_axisJogAdjustment->setVisible(visible);
		m_axisJogSelection->setVisible(visible);
	}

	void PopupWindow::SetMinTextf(const char* format, ...)
	{
		va_list vargs;
		va_start(vargs, format);
		m_minText->setTextf(format, vargs);
		va_end(vargs);
	}

	void PopupWindow::SetMaxTextf(const char* format, ...)
	{
		va_list vargs;
		va_start(vargs, format);
		m_maxText->setTextf(format, vargs);
		va_end(vargs);
	}

	void PopupWindow::SetWarningTextf(const char* format, ...)
	{
		va_list vargs;
		va_start(vargs, format);
		m_warningText->setTextf(format, vargs);
		va_end(vargs);
	}

	void PopupWindow::SetNumberInput(int32_t val)
	{
		m_numberInput->setText(val);
	}

	void PopupWindow::SetNumberInput(float val)
	{
		m_numberInput->setText(val);
	}

	void PopupWindow::SetNumberInput(const char* text)
	{
		m_numberInput->setText(text);
	}

	void PopupWindow::SetTextInput(const char* text)
	{
		m_textInput->setText(text);
	}

	void PopupWindow::SetImage(const char* imagePath)
	{
		m_image->setBackgroundPic(imagePath);
		ShowImage(true);
	}

	void PopupWindow::ShowImage(bool show)
	{
		LayoutPosition position = m_text->getPosition();
		if (show)
		{
			position.mWidth = m_image->getPosition().mLeft - 2 * position.mLeft - 10;
		}
		else
		{
			position.mWidth = m_window->getPosition().mWidth - 2 * position.mLeft;
		}
		m_text->setPosition(position);
		m_image->setVisible(show);
	}

	void PopupWindow::SetProgress(int percent)
	{
		m_progress->setVisible(true);
		m_progress->setProgress(percent);
	}

	void PopupWindow::SetMode(OM::Alert::Mode mode)
	{
		m_mode = mode;
		if (IsResponse())
		{
			return;
		}
		CancelTimeout();
		if (IsBlocking())
		{
			m_noTouchWindow->setVisible(true);
		}
	}

	void PopupWindow::PreventClosing(bool prevent)
	{
		m_noTouchWindow->setVisible(prevent);
		m_okBtn->setVisible(!prevent);
		m_cancelBtn->setVisible(!prevent);
	}

	void PopupWindow::CancelTimeout()
	{
		info("Cancelling timeout");
		if (!IsOpen())
		{
			warn("Cannot cancel timeout when popup is not open");
			return;
		}

		unregisterDelayedCallback("popup_timeout");
	}

	void PopupWindow::SetTimeout(uint32_t timeout)
	{
		info("Setting info timeout to %u", timeout);
		StoragePreferences::putInt(ID_INFO_TIMEOUT, (int)timeout);
		m_timeout = timeout;
	}

	bool PopupWindow::IsBlocking() const
	{
		switch (m_mode)
		{
		case OM::Alert::Mode::InfoConfirm:
		case OM::Alert::Mode::ConfirmCancel:
		case OM::Alert::Mode::Choices:
		case OM::Alert::Mode::NumberInt:
		case OM::Alert::Mode::NumberFloat:
		case OM::Alert::Mode::Text:
			// These alerts are blocking
			return true;
		default:
			break;
		}
		return false;
	}

	bool PopupWindow::IsResponse() const
	{
		return m_mode == OM::Alert::Mode::None;
	}

	void PopupWindow::Clear()
	{
		m_mode = OM::Alert::Mode::None;
		m_noTouchWindow->setVisible(false);
		m_title->setText("");
		m_text->setText("");
		m_warningText->setText("");
		m_warningText->setVisible(false);
		m_minText->setVisible(false);
		m_maxText->setVisible(false);
		m_choicesList->setVisible(false);
		m_textInput->setVisible(false);
		m_numberInput->setVisible(false);
		m_cancelBtn->setVisible(true);
		m_okBtn->setInvalid(false);
		m_okBtn->setVisible(true);
		m_axisJogSelection->setVisible(false);
		m_axisJogAdjustment->setVisible(false);
		m_image->setVisible(false);
		m_image->setText("");
		m_image->setBackgroundPic(nullptr);
		m_progress->setVisible(false);

		LayoutPosition position = m_text->getPosition();
		position.mWidth = m_window->getPosition().mWidth - 2 * position.mLeft;
		m_text->setPosition(position);

		for (auto& axis : m_axes)
		{
			axis = nullptr;
		}
		selectedAxis = 0;

		FILEINFO_CACHE->StopThumbnailRequest(true);
	}

	bool PopupWindow::ValidateIntegerInput(const char* text)
	{
		if (!ValidateIntegerInputInner(text))
		{
			m_okBtn->setInvalid(true);
			return false;
		}
		m_okBtn->setInvalid(false);
		return true;
	}

	bool PopupWindow::ValidateFloatInput(const char* text)
	{
		if (!ValidateFloatInputInner(text))
		{
			m_okBtn->setInvalid(true);
			return false;
		}
		m_okBtn->setInvalid(false);
		return true;
	}

	bool PopupWindow::ValidateTextInput(const char* text)
	{
		if (!ValidateTextInputInner(text))
		{
			m_okBtn->setInvalid(true);
			return false;
		}
		m_okBtn->setInvalid(false);
		return true;
	}

	bool PopupWindow::ValidateIntegerInputInner(const char* text)
	{
		if (m_mode != OM::Alert::Mode::NumberInt)
		{
			return false;
		}

		int32_t value;
		m_warningText->setVisible(true);
		if (!Comm::GetInteger(text, value))
		{
			m_warningText->setTextTr("invalid_int_malformed");
			return false;
		}
		dbg("input %s, value %d", text, value);
		if (value < OM::g_currentAlert.limits.numberInt.min)
		{
			m_warningText->setTextTr("invalid_int_min");
			return false;
		}
		if (value > OM::g_currentAlert.limits.numberInt.max)
		{
			m_warningText->setTextTr("invalid_int_max");
			return false;
		}

		m_warningText->setVisible(false);
		m_warningText->setText("");
		m_numberInput->setText(value); // This is to convert float to int and revalidate
		return true;
	}

	bool PopupWindow::ValidateFloatInputInner(const char* text)
	{
		if (m_mode != OM::Alert::Mode::NumberFloat)
			return false;

		float value;
		m_warningText->setVisible(true);
		if (!Comm::GetFloat(text, value))
		{
			m_warningText->setTextTr("invalid_float_malformed");
			return false;
		}
		dbg("input %s, value %.3f", text, value);
		if (value < OM::g_currentAlert.limits.numberFloat.min)
		{
			m_warningText->setTextTr("invalid_float_min");
			return false;
		}
		if (value > OM::g_currentAlert.limits.numberFloat.max)
		{
			m_warningText->setTextTr("invalid_float_max");
			return false;
		}

		m_warningText->setVisible(false);
		m_warningText->setText("");
		return true;
	}

	bool PopupWindow::ValidateTextInputInner(const char* text)
	{
		if (m_mode != OM::Alert::Mode::Text)
			return false;

		size_t len = Strnlen(text, ALERT_RESPONSE_LENGTH);
		m_warningText->setVisible(true);
		if (len < (size_t)OM::g_currentAlert.limits.text.min)
		{
			m_warningText->setTextTr("invalid_text_min");
			return false;
		}
		if (len > (size_t)OM::g_currentAlert.limits.text.max)
		{
			m_warningText->setTextTr("invalid_text_max");
			return false;
		}

		m_warningText->setVisible(false);
		m_warningText->setText("");
		return true;
	}

	OM::Move::Axis* PopupWindow::GetJogAxis(int listIndex) const
	{
		return m_axes[listIndex];
	}

	size_t PopupWindow::GetJogAxisCount() const
	{
		size_t count = 0;
		for (auto axis : m_axes)
		{
			if (axis != nullptr)
			{
				count++;
			}
		}
		return count;
	}

	void PopupWindow::SetAxisJogSelection(uint32_t axisControl)
	{
		size_t count = 0;
		dbg("axisControl %d", axisControl);
		for (size_t i = 0; i < MAX_TOTAL_AXES; ++i)
		{
			if (!(axisControl & (1 << i)))
				continue;
			OM::Move::Axis* axis = OM::Move::GetAxis(i);
			if (axis == nullptr)
			    continue;
			m_axes[count] = axis;
			dbg("Axis %d, count %d", i, count);
			count++;
		}
		m_axisJogSelection->setSelection(0);
	}

	const char* PopupWindow::GetNumberInput() const
	{
		return m_numberInput->getText().c_str();
	}

	const char* PopupWindow::GetTextInput() const
	{
		return m_textInput->getText().c_str();
	}

	void NumPadWindow::Init(ZKWindow* window, ZKTextView* header, ZKTextView* value)
	{
		m_window = window;
		m_header = header;
		m_value = value;
	}

	void NumPadWindow::Open(const char* header,
							const int min,
							const int max,
							const int value,
							function<void(int)> onValueChanged,
							function<void(int)> onConfirm,
							bool withSlider)
	{
		m_header->setText(header);
		m_min = min;
		m_max = max;
		m_value->setText(value);
		m_onValueChanged = onValueChanged;
		m_onConfirm = onConfirm;
		SetPosition(HorizontalPosition::right);
		UI::GetUIControl<ZKButton>(ID_MAIN_OverlayModalZone)->setVisible(true);
		WINDOW->OpenOverlay(m_window, !withSlider);
	}

	void NumPadWindow::Clear()
	{
		m_onValueChanged = [](int) {};
		m_onConfirm = [](int) {};
		m_header->setText("");
		m_value->setText("");
	}

	void NumPadWindow::Close()
	{
		Clear();
		WINDOW->CloseOverlay();
	}

	bool NumPadWindow::ValidateInput(int value)
	{
		bool valid = value >= m_min && value <= m_max;
		UI::GetUIControl<ZKButton>(ID_MAIN_NumPadConfirm)->setInvalid(!valid);
		return valid;
	}

	void NumPadWindow::Callback()
	{
		m_onValueChanged(GetValue());
	}

	void NumPadWindow::Confirm()
	{
		m_onConfirm(GetValue());
		Close();
		Clear();
	}

	void NumPadWindow::SetPosition(const HorizontalPosition& horizontal)
	{
		LayoutPosition position = m_window->getPosition();
		switch (horizontal)
		{
		case HorizontalPosition::left:
			position.mLeft = 115;
			break;
		case HorizontalPosition::center:
			position.mLeft = 345;
			break;
		case HorizontalPosition::right:
			position.mLeft = 606;
			break;
		}
		m_window->setPosition(position);
	}

	int NumPadWindow::GetValue()
	{
		int value;
		if (!Comm::GetInteger(m_value->getText().c_str(), value))
		{
			return 0;
		}
		return value;
	}

	void NumPadWindow::SetValue(int value)
	{
		if (value == GetValue())
			return;
		m_value->setText(value);
		if (ValidateInput(value))
			Callback();
	}

	void NumPadWindow::SetValue(const char* value)
	{
		m_value->setText(value);
		if (ValidateInput(GetValue()))
			Callback();
	}

	void NumPadWindow::ClearValue()
	{
		SetValue("");
	}

	void NumPadWindow::AddOneChar(char ch)
	{
		std::string value = m_value->getText() + ch;
		SetValue(value.c_str());
	}

	void NumPadWindow::DelOneChar()
	{
		std::string value = m_value->getText();
		if (value.empty())
		{
			return;
		}
		value.erase(value.length() - 1, 1);
		SetValue(value.c_str());
	}

	void SliderWindow::Init(ZKWindow* window,
							ZKSeekBar* slider,
							ZKTextView* header,
							ZKTextView* value,
							ZKTextView* prefix,
							ZKTextView* suffix)
	{
		m_window = window;
		m_slider = slider;
		m_header = header;
		m_value = value;
		m_prefix = prefix;
		m_suffix = suffix;
	}

	void SliderWindow::Open(const char* header,
							const char* prefix,
							const char* suffix,
							const char* unit,
							const int min,
							const int max,
							const int value,
							function<void(int)> onProgressChanged,
							bool displayRaw)
	{
		m_onProgressChanged = [](int) {}; // reset callback
		SetUnit(unit);
		m_displayRaw = displayRaw;
		SetRange(min, max);
		SetHeader(header);
		SetPrefix(prefix);
		SetSuffix(suffix);
		SetValue(value);
		SetOnProgressChanged(onProgressChanged);
		SetPosition(VerticalPosition::center, HorizontalPosition::center);
		UI::GetUIControl<ZKButton>(ID_MAIN_OverlayModalZone)->setVisible(true);
		WINDOW->OpenOverlay(m_window);
	}

	void SliderWindow::Callback() const
	{
		m_value->setTextf("%d %s", GetValue(), m_unit.c_str());
		if (m_displayRaw)
		{
			// Display the raw slider value
			m_value->setTextf("%d %s", m_slider->getProgress(), m_unit.c_str());
		}
		else
		{
			// Display the value adjusted by min and max
			m_value->setTextf("%d %s",
							 GetValue(),
							 m_unit.c_str()); // this lets you set the displayed value outside the range
											 // which may be useful maybe? If not, change to GetValue()
		}
		m_onProgressChanged(GetValue());
	}

	void SliderWindow::SetPosition(const VerticalPosition& vertical, const HorizontalPosition& horizontal)
	{
		LayoutPosition position = m_window->getPosition();
		switch (vertical)
		{
		case VerticalPosition::top:
			position.mTop = 75;
			break;
		case VerticalPosition::center:
			position.mTop = 210;
			break;
		case VerticalPosition::bottom:
			position.mTop = 380;
			break;
		}
		switch (horizontal)
		{
		case HorizontalPosition::left:
			position.mLeft = 130;
			break;
		case HorizontalPosition::center:
			position.mLeft = 320;
			break;
		case HorizontalPosition::right:
			position.mLeft = 530;
			break;
		}
		m_window->setPosition(position);
	}

	void SliderWindow::SetRange(const int min, const int max)
	{
		m_min = min;
		m_max = max;
		m_slider->setMax(max - min);
	}

	const int SliderWindow::GetValue() const
	{
		int progress = m_slider->getProgress();
		int val = progress + m_min;
		warn("Slider progress %d (%d)", progress, val);
		return val;
	}

	void SliderWindow::SetValue(const int val)
	{
		if (val == GetValue())
			return;

		if (val < m_min)
		{
			warn("Slider value %d below min %d", val, m_min);
			return;
		}

		if (val > m_max)
		{
			warn("Slider value %d exceeds max %d", val, m_max);
			return;
		}

		int progress = val - m_min;
		warn("Slider progress %d (%d)", progress, val);
		m_slider->setProgress(progress);
	}

	void SliderWindow::SetHeader(const char* header)
	{
		m_header->setText(header);
	}

	void SliderWindow::SetHeaderf(const char* format, ...)
	{
		va_list vargs;
		va_start(vargs, format);
		m_header->setTextf(format, vargs);
		va_end(vargs);
	}

	void SliderWindow::SetPrefix(const char* prefix)
	{
		m_prefix->setTextf("%s %d", prefix, m_min);
	}

	void SliderWindow::SetSuffix(const char* suffix)
	{
		m_suffix->setTextf("%d %s %s", m_max, m_unit.c_str(), suffix);
	}

	void SliderWindow::SetUnit(const char* unit)
	{
		m_unit.copy(unit);
	}

	void OpenSliderNumPad(const char* header,
						  const char* prefix,
						  const char* suffix,
						  const char* unit,
						  const int min,
						  const int max,
						  const int defaultValue,
						  function<void(int)> onProgressChanged,
						  bool displayRaw)
	{
		warn("defaultValue %d", defaultValue);
		SLIDER_WINDOW->Open(
			header,
			prefix,
			suffix,
			unit,
			min,
			max,
			defaultValue,
			[onProgressChanged](int value) {
				warn("Slider value %d", value);
				NUMPAD_WINDOW->SetValue(value);
				onProgressChanged(value);
			},
			displayRaw);
		SLIDER_WINDOW->SetPosition(VerticalPosition::center, HorizontalPosition::left);
		NUMPAD_WINDOW->Open(
			"",
			min,
			max,
			defaultValue,
			[](int value) {
				warn("Numpad value %d", value);
				SLIDER_WINDOW->SetValue(value);
			},
			[](int) { WINDOW->CloseOverlay(); },
			true);
	}
} // namespace UI
