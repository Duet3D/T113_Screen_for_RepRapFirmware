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
#include "UserInterface.h"
#include "storage/StoragePreferences.h"
#include "timer.h"
#include <algorithm>
#include <map>

namespace UI
{
	PopupWindow::PopupWindow() : okCb_([]() {}), cancelCb_([]() {})
	{
		window_ = UI::GetUIControl<ZKWindow>(ID_MAIN_PopupWindow);
		noTouchWindow_ = UI::GetUIControl<ZKWindow>(ID_MAIN_NoTouchWindow);
		okBtn_ = UI::GetUIControl<ZKButton>(ID_MAIN_PopupOkBtn);
		cancelBtn_ = UI::GetUIControl<ZKButton>(ID_MAIN_PopupCancelBtn);
		title_ = UI::GetUIControl<ZKTextView>(ID_MAIN_PopupTitle);
		text_ = UI::GetUIControl<ZKTextView>(ID_MAIN_PopupText);
		warningText_ = UI::GetUIControl<ZKTextView>(ID_MAIN_PopupWarning);
		minText_ = UI::GetUIControl<ZKTextView>(ID_MAIN_PopupMin);
		maxText_ = UI::GetUIControl<ZKTextView>(ID_MAIN_PopupMax);
		choicesList_ = UI::GetUIControl<ZKListView>(ID_MAIN_PopupSelectionList);
		textInput_ = UI::GetUIControl<ZKEditText>(ID_MAIN_PopupTextInput);
		numberInput_ = UI::GetUIControl<ZKEditText>(ID_MAIN_PopupNumberInput);
		axisJogSelection_ = UI::GetUIControl<ZKListView>(ID_MAIN_PopupAxisSelection);
		axisJogAdjustment_ = UI::GetUIControl<ZKListView>(ID_MAIN_PopupAxisAdjusment);
		image_ = UI::GetUIControl<ZKTextView>(ID_MAIN_PopupImage);
		progress_ = UI::GetUIControl<ZKSeekBar>(ID_MAIN_PopupProgress);

		title_->setLongMode(ZKTextView::E_LONG_MODE_SCROLL);
		text_->setLongMode(ZKTextView::E_LONG_MODE_SCROLL);
		SetTimeout(StoragePreferences::getInt("info_timeout", DEFAULT_POPUP_TIMEOUT));
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

		seq_ = OM::currentAlert.seq;

		UI::GetUIControl<ZKButton>(ID_MAIN_OverlayModalZone)->setVisible(true);

		if (timeout_ > 0)
		{
			registerDelayedCallback("popup_timeout", timeout_, []() {
				UI::POPUP_WINDOW->Close();
				return false;
			});
		}

		cancelBtn_->setVisible(true);
		okBtn_->setVisible(true);

		SetOkBtnText(LANGUAGEMANAGER->getValue("ok").c_str());
		SetCancelBtnText(LANGUAGEMANAGER->getValue("cancel").c_str());
		SetPosition(VerticalPosition::center, HorizontalPosition::center);
		WINDOW->OpenOverlay(window_);
		okCb_ = okCb;
		cancelCb_ = cancelCb;
	}

	void PopupWindow::Ok()
	{
		okCb_();
		Close();
	}

	void PopupWindow::Cancel(bool close)
	{
		if (!IsResponse())
		{
			Comm::duet.SendGcode("M292 P1");
		}
		cancelCb_();

		if (close)
			Close();
	}

	void PopupWindow::SetPosition(const VerticalPosition& vertical, const HorizontalPosition& horizontal)
	{
		LayoutPosition position = window_->getPosition();
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
		window_->setPosition(position);
	}

	void PopupWindow::SetTitle(const std::string& title)
	{
		title_->setText(title);
	}

	void PopupWindow::SetText(const std::string& text)
	{
		text_->setText(text);
	}

	void PopupWindow::SetText(const char* text)
	{
		text_->setText(text);
	}

	void PopupWindow::SetTextf(const char* format, ...)
	{
		va_list vargs;
		va_start(vargs, format);
		text_->setTextf(format, vargs);
		va_end(vargs);
	}

	void PopupWindow::SetTextScrollable(bool scrollable)
	{
		text_->setLongMode(scrollable ? ZKTextView::E_LONG_MODE_SCROLL : ZKTextView::E_LONG_MODE_NONE);
	}

	void PopupWindow::SetOkBtnText(const char* text)
	{
		okBtn_->setText(text);
	}

	void PopupWindow::SetCancelBtnText(const char* text)
	{
		cancelBtn_->setText(text);
	}

	void PopupWindow::Close()
	{
		info("Closing popup window");
		Clear();
		WINDOW->CloseOverlay();
	}

	void PopupWindow::OkVisible(bool visible)
	{
		okBtn_->setVisible(visible);
	}

	void PopupWindow::CancelVisible(bool visible)
	{
		cancelBtn_->setVisible(visible);
	}

	void PopupWindow::SelectionVisible(bool visible)
	{
		choicesList_->setVisible(visible);
	}

	void PopupWindow::NumberInputVisible(bool visible)
	{
		numberInput_->setVisible(visible);
	}

	void PopupWindow::TextInputVisible(bool visible)
	{
		textInput_->setVisible(visible);
	}

	void PopupWindow::WarningTextVisible(bool visible)
	{
		warningText_->setVisible(visible);
	}

	void PopupWindow::MinTextVisible(bool visible)
	{
		minText_->setVisible(visible);
	}

	void PopupWindow::MaxTextVisible(bool visible)
	{
		maxText_->setVisible(visible);
	}

	void PopupWindow::AxisJogVisible(bool visible)
	{
		axisJogAdjustment_->setVisible(visible);
		axisJogSelection_->setVisible(visible);
	}

	void PopupWindow::SetMinTextf(const char* format, ...)
	{
		va_list vargs;
		va_start(vargs, format);
		minText_->setTextf(format, vargs);
		va_end(vargs);
	}

	void PopupWindow::SetMaxTextf(const char* format, ...)
	{
		va_list vargs;
		va_start(vargs, format);
		maxText_->setTextf(format, vargs);
		va_end(vargs);
	}

	void PopupWindow::SetWarningTextf(const char* format, ...)
	{
		va_list vargs;
		va_start(vargs, format);
		warningText_->setTextf(format, vargs);
		va_end(vargs);
	}

	void PopupWindow::SetNumberInput(int32_t val)
	{
		numberInput_->setText(val);
	}

	void PopupWindow::SetNumberInput(float val)
	{
		numberInput_->setText(val);
	}

	void PopupWindow::SetNumberInput(const char* text)
	{
		numberInput_->setText(text);
	}

	void PopupWindow::SetTextInput(const char* text)
	{
		textInput_->setText(text);
	}

	void PopupWindow::SetImage(const char* imagePath)
	{
		image_->setBackgroundPic(imagePath);
		ShowImage(true);
	}

	void PopupWindow::ShowImage(bool show)
	{
		LayoutPosition position = text_->getPosition();
		if (show)
		{
			position.mWidth = image_->getPosition().mLeft - 2 * position.mLeft - 10;
		}
		else
		{
			position.mWidth = window_->getPosition().mWidth - 2 * position.mLeft;
		}
		text_->setPosition(position);
		image_->setVisible(show);
	}

	void PopupWindow::SetProgress(int percent)
	{
		progress_->setVisible(true);
		progress_->setProgress(percent);
	}

	void PopupWindow::SetMode(OM::Alert::Mode mode)
	{
		mode_ = mode;
		if (IsResponse())
		{
			return;
		}
		CancelTimeout();
		if (IsBlocking())
		{
			noTouchWindow_->setVisible(true);
		}
	}

	void PopupWindow::PreventClosing(bool prevent)
	{
		noTouchWindow_->setVisible(prevent);
		okBtn_->setVisible(!prevent);
		cancelBtn_->setVisible(!prevent);
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
		StoragePreferences::putInt("info_timeout", (int)timeout);
		timeout_ = timeout;
	}

	bool PopupWindow::IsBlocking() const
	{
		switch (mode_)
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
		return OM::currentAlert.mode == OM::Alert::Mode::None;
	}

	void PopupWindow::Clear()
	{
		mode_ = OM::Alert::Mode::None;
		noTouchWindow_->setVisible(false);
		title_->setText("");
		text_->setText("");
		warningText_->setText("");
		warningText_->setVisible(false);
		minText_->setVisible(false);
		maxText_->setVisible(false);
		choicesList_->setVisible(false);
		textInput_->setVisible(false);
		numberInput_->setVisible(false);
		cancelBtn_->setVisible(true);
		okBtn_->setInvalid(false);
		okBtn_->setVisible(true);
		axisJogSelection_->setVisible(false);
		axisJogAdjustment_->setVisible(false);
		image_->setVisible(false);
		image_->setText("");
		image_->setBackgroundPic(nullptr);
		progress_->setVisible(false);

		LayoutPosition position = text_->getPosition();
		position.mWidth = window_->getPosition().mWidth - 2 * position.mLeft;
		text_->setPosition(position);

		for (auto& axis : axes_)
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
			okBtn_->setInvalid(true);
			return false;
		}
		okBtn_->setInvalid(false);
		return true;
	}

	bool PopupWindow::ValidateFloatInput(const char* text)
	{
		if (!ValidateFloatInputInner(text))
		{
			okBtn_->setInvalid(true);
			return false;
		}
		okBtn_->setInvalid(false);
		return true;
	}

	bool PopupWindow::ValidateTextInput(const char* text)
	{
		if (!ValidateTextInputInner(text))
		{
			okBtn_->setInvalid(true);
			return false;
		}
		okBtn_->setInvalid(false);
		return true;
	}

	bool PopupWindow::ValidateIntegerInputInner(const char* text)
	{
		if (mode_ != OM::Alert::Mode::NumberInt)
		{
			return false;
		}

		int32_t value;
		warningText_->setVisible(true);
		if (!Comm::GetInteger(text, value))
		{
			warningText_->setTextTr("invalid_int_malformed");
			return false;
		}
		dbg("input %s, value %d", text, value);
		if (value < OM::currentAlert.limits.numberInt.min)
		{
			warningText_->setTextTr("invalid_int_min");
			return false;
		}
		if (value > OM::currentAlert.limits.numberInt.max)
		{
			warningText_->setTextTr("invalid_int_max");
			return false;
		}

		warningText_->setVisible(false);
		warningText_->setText("");
		numberInput_->setText(value); // This is to convert float to int and revalidate
		return true;
	}

	bool PopupWindow::ValidateFloatInputInner(const char* text)
	{
		if (mode_ != OM::Alert::Mode::NumberFloat)
			return false;

		float value;
		warningText_->setVisible(true);
		if (!Comm::GetFloat(text, value))
		{
			warningText_->setTextTr("invalid_float_malformed");
			return false;
		}
		dbg("input %s, value %.3f", text, value);
		if (value < OM::currentAlert.limits.numberFloat.min)
		{
			warningText_->setTextTr("invalid_float_min");
			return false;
		}
		if (value > OM::currentAlert.limits.numberFloat.max)
		{
			warningText_->setTextTr("invalid_float_max");
			return false;
		}

		warningText_->setVisible(false);
		warningText_->setText("");
		return true;
	}

	bool PopupWindow::ValidateTextInputInner(const char* text)
	{
		if (mode_ != OM::Alert::Mode::Text)
			return false;

		size_t len = Strnlen(text, ALERT_RESPONSE_LENGTH);
		warningText_->setVisible(true);
		if (len < (size_t)OM::currentAlert.limits.text.min)
		{
			warningText_->setTextTr("invalid_text_min");
			return false;
		}
		if (len > (size_t)OM::currentAlert.limits.text.max)
		{
			warningText_->setTextTr("invalid_text_max");
			return false;
		}

		warningText_->setVisible(false);
		warningText_->setText("");
		return true;
	}

	OM::Move::Axis* PopupWindow::GetJogAxis(int listIndex) const
	{
		return axes_[listIndex];
	}

	size_t PopupWindow::GetJogAxisCount() const
	{
		size_t count = 0;
		for (auto axis : axes_)
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
			axes_[count] = axis;
			dbg("Axis %d, count %d", i, count);
			count++;
		}
		axisJogSelection_->setSelection(0);
	}

	const char* PopupWindow::GetNumberInput() const
	{
		return numberInput_->getText().c_str();
	}

	const char* PopupWindow::GetTextInput() const
	{
		return textInput_->getText().c_str();
	}

	void NumPadWindow::Init(ZKWindow* window, ZKTextView* header, ZKTextView* value)
	{
		window_ = window;
		header_ = header;
		value_ = value;
	}

	void NumPadWindow::Open(const char* header,
							const int min,
							const int max,
							const int value,
							function<void(int)> onValueChanged,
							function<void(int)> onConfirm,
							bool withSlider)
	{
		header_->setText(header);
		min_ = min;
		max_ = max;
		value_->setText(value);
		onValueChanged_ = onValueChanged;
		onConfirm_ = onConfirm;
		SetPosition(HorizontalPosition::right);
		UI::GetUIControl<ZKButton>(ID_MAIN_OverlayModalZone)->setVisible(true);
		WINDOW->OpenOverlay(window_, !withSlider);
	}

	void NumPadWindow::Clear()
	{
		onValueChanged_ = [](int) {};
		onConfirm_ = [](int) {};
		header_->setText("");
		value_->setText("");
	}

	void NumPadWindow::Close()
	{
		Clear();
		WINDOW->CloseOverlay();
	}

	bool NumPadWindow::ValidateInput(int value)
	{
		bool valid = value >= min_ && value <= max_;
		UI::GetUIControl<ZKButton>(ID_MAIN_NumPadConfirm)->setInvalid(!valid);
		return valid;
	}

	void NumPadWindow::Callback()
	{
		onValueChanged_(GetValue());
	}

	void NumPadWindow::Confirm()
	{
		onConfirm_(GetValue());
		Close();
		Clear();
	}

	void NumPadWindow::SetPosition(const HorizontalPosition& horizontal)
	{
		LayoutPosition position = window_->getPosition();
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
		window_->setPosition(position);
	}

	int NumPadWindow::GetValue()
	{
		int value;
		if (!Comm::GetInteger(value_->getText().c_str(), value))
		{
			return 0;
		}
		return value;
	}

	void NumPadWindow::SetValue(int value)
	{
		if (value == GetValue())
			return;
		value_->setText(value);
		if (ValidateInput(value))
			Callback();
	}

	void NumPadWindow::SetValue(const char* value)
	{
		value_->setText(value);
		if (ValidateInput(GetValue()))
			Callback();
	}

	void NumPadWindow::ClearValue()
	{
		SetValue("");
	}

	void NumPadWindow::AddOneChar(char ch)
	{
		std::string value = value_->getText() + ch;
		SetValue(value.c_str());
	}

	void NumPadWindow::DelOneChar()
	{
		std::string value = value_->getText();
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
		window_ = window;
		slider_ = slider;
		header_ = header;
		value_ = value;
		prefix_ = prefix;
		suffix_ = suffix;
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
		onProgressChanged_ = [](int) {}; // reset callback
		SetUnit(unit);
		displayRaw_ = displayRaw;
		SetRange(min, max);
		SetHeader(header);
		SetPrefix(prefix);
		SetSuffix(suffix);
		SetValue(value);
		SetOnProgressChanged(onProgressChanged);
		SetPosition(VerticalPosition::center, HorizontalPosition::center);
		UI::GetUIControl<ZKButton>(ID_MAIN_OverlayModalZone)->setVisible(true);
		WINDOW->OpenOverlay(window_);
	}

	void SliderWindow::Callback() const
	{
		value_->setTextf("%d %s", GetValue(), unit_.c_str());
		if (displayRaw_)
		{
			// Display the raw slider value
			value_->setTextf("%d %s", slider_->getProgress(), unit_.c_str());
		}
		else
		{
			// Display the value adjusted by min and max
			value_->setTextf("%d %s",
							 GetValue(),
							 unit_.c_str()); // this lets you set the displayed value outside the range
											 // which may be useful maybe? If not, change to GetValue()
		}
		onProgressChanged_(GetValue());
	}

	void SliderWindow::SetPosition(const VerticalPosition& vertical, const HorizontalPosition& horizontal)
	{
		LayoutPosition position = window_->getPosition();
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
		window_->setPosition(position);
	}

	void SliderWindow::SetRange(const int min, const int max)
	{
		min_ = min;
		max_ = max;
		slider_->setMax(max - min);
	}

	const int SliderWindow::GetValue() const
	{
		int progress = slider_->getProgress();
		int val = progress + min_;
		warn("Slider progress %d (%d)", progress, val);
		return val;
	}

	void SliderWindow::SetValue(const int val)
	{
		if (val == GetValue())
			return;

		if (val < min_)
		{
			warn("Slider value %d below min %d", val, min_);
			return;
		}

		if (val > max_)
		{
			warn("Slider value %d exceeds max %d", val, max_);
			return;
		}

		int progress = val - min_;
		warn("Slider progress %d (%d)", progress, val);
		slider_->setProgress(progress);
	}

	void SliderWindow::SetHeader(const char* header)
	{
		header_->setText(header);
	}

	void SliderWindow::SetHeaderf(const char* format, ...)
	{
		va_list vargs;
		va_start(vargs, format);
		header_->setTextf(format, vargs);
		va_end(vargs);
	}

	void SliderWindow::SetPrefix(const char* prefix)
	{
		prefix_->setTextf("%s %d", prefix, min_);
	}

	void SliderWindow::SetSuffix(const char* suffix)
	{
		suffix_->setTextf("%d %s %s", max_, unit_.c_str(), suffix);
	}

	void SliderWindow::SetUnit(const char* unit)
	{
		unit_.copy(unit);
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
