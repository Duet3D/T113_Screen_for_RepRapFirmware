/*
 * Popup.h
 *
 *  Created on: 23 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_UI_POPUP_H_
#define JNI_LOGIC_UI_POPUP_H_

#include "control/ZKEditText.h"
#include "control/ZKListView.h"
#include "control/ZKSeekBar.h"
#include "control/ZKTextView.h"
#include "std_fixed/functional.h"
#include "window/ZKWindow.h"
#include <string>
#include <sys/types.h>
#include <vector>

#include "Duet3D/General/CircularBuffer.h"
#include "Duet3D/General/StringRef.h"
#include "ObjectModel/Alert.h"
#include "ObjectModel/BedOrChamber.h"
#include "ObjectModel/Tool.h"
#include "UI/UserInterfaceConstants.h"

namespace UI
{
	enum class VerticalPosition
	{
		top = 0,
		center,
		bottom,
	};

	enum class HorizontalPosition
	{
		left = 0,
		center,
		right,
	};

	class PopupWindow
	{
	  public:
		static PopupWindow* GetInstance()
		{
			static PopupWindow cWindow;
			return &cWindow;
		}
		void Init(ZKWindow* window,
				  ZKWindow* noTouchWindow,
				  ZKButton* okBtn,
				  ZKButton* cancelBtn,
				  ZKTextView* title,
				  ZKTextView* text,
				  ZKTextView* warningText,
				  ZKTextView* minText,
				  ZKTextView* maxText,
				  ZKListView* choicesList,
				  ZKEditText* textInput,
				  ZKEditText* numberInput,
				  ZKListView* axisJogSelection,
				  ZKListView* axisJogAdjustment,
				  ZKTextView* image);
		void Open();
		void Open(function<void(void)> okCb);
		void Open(function<void(void)> okCb, function<void(void)> cancelCb);
		void Ok();
		void Cancel(bool close = true);
		void SetPosition(const VerticalPosition& vertical, const HorizontalPosition& horizontal);
		void SetText(const std::string& text);
		void SetText(const char* text);
		void SetTextf(const char* format, ...);
		void SetOkBtnText(const char* text);
		void SetCancelBtnText(const char* text);
		void SetImage(const char* imagePath);
		void ShowImage(bool show);
		void Close();

		void CancelTimeout();
		void SetTimeout(uint32_t timeout);
		const uint32_t GetTimeout() const { return timeout_; }

		bool IsOpen() const { return window_->isVisible(); }
		bool IsBlocking() const;
		bool IsResponse() const;
		void Clear();

		bool ValidateIntegerInput(const char* text);
		bool ValidateFloatInput(const char* text);
		bool ValidateTextInput(const char* text);

		uint32_t GetSeq() const { return seq_; }
		const OM::Alert::Mode GetMode() const { return mode_; }

		OM::Move::Axis* GetJogAxis(int listIndex) const;
		size_t GetJogAxisCount() const;

		float jogAmounts[6] = {2.0f, 0.2f, 0.02f, -0.02f, -0.2f, -2.0f};
		int selectedAxis = 0;

	  private:
		PopupWindow() : okCb_([]() {}), cancelCb_([]() {}) {}
		bool ValidateIntegerInputInner(const char* text);
		bool ValidateFloatInputInner(const char* text);
		bool ValidateTextInputInner(const char* text);

		void SetAxisJogSelection(uint32_t axisControl);

		ZKWindow* window_ = nullptr;
		ZKWindow* noTouchWindow_ = nullptr;
		ZKButton* okBtn_ = nullptr;
		ZKButton* cancelBtn_ = nullptr;
		ZKTextView* title_ = nullptr;
		ZKTextView* text_ = nullptr;
		ZKTextView* warningText_ = nullptr;
		ZKTextView* minText_ = nullptr;
		ZKTextView* maxText_ = nullptr;
		ZKListView* choicesList_ = nullptr;
		ZKEditText* textInput_ = nullptr;
		ZKEditText* numberInput_ = nullptr;
		ZKListView* axisJogSelection_ = nullptr;
		ZKListView* axisJogAdjustment_ = nullptr;
		ZKTextView* image_ = nullptr;
		function<void(void)> okCb_;
		function<void(void)> cancelCb_;
		OM::Alert::Mode mode_;
		uint32_t seq_;
		OM::Move::Axis* axes_[MaxTotalAxes];
		uint32_t timeout_;
	};

	class NumPadWindow
	{
	  public:
		static NumPadWindow* GetInstance()
		{
			static NumPadWindow numPadWindow;
			return &numPadWindow;
		}

		void Init(ZKWindow* window, ZKTextView* header, ZKTextView* value);
		void Open(const char* header,
				  const int value,
				  function<void(int)> onValueChanged,
				  function<void(int)> onConfirm,
				  bool withSlider = false);
		void Clear();
		void Close();
		void Confirm();
		void SetPosition(const HorizontalPosition& horizontal);
		int GetValue();
		void SetValue(int value);
		void SetValue(const char* value);
		void ClearValue();
		void AddOneChar(char c);
		void DelOneChar();

	  private:
		NumPadWindow()
			: window_(nullptr), header_(nullptr), value_(nullptr), onValueChanged_([](int) {}), onConfirm_([](int) {})
		{
		}
		void Callback();

		ZKWindow* window_;
		ZKTextView* header_;
		ZKTextView* value_;
		function<void(int)> onValueChanged_;
		function<void(int)> onConfirm_;
	};

	class SliderWindow
	{
	  public:
		static SliderWindow* GetInstance()
		{
			static SliderWindow sliderWindow;
			return &sliderWindow;
		}
		void Init(ZKWindow* window,
				  ZKSeekBar* slider,
				  ZKTextView* header,
				  ZKTextView* value,
				  ZKTextView* prefix,
				  ZKTextView* suffix);
		void Open(const char* header, // The header text
				  const char* prefix, // The prefix text, displayed on left of slider
				  const char* suffix, // The suffix text, displayed on right of slider
				  const char* unit,	  // The unit text, displayed after the value
				  const int min,
				  const int max,
				  const int value,
				  function<void(int)> onProgressChanged,
				  bool displayRaw = false // Display the raw value instead of the percentage
		);
		void Callback() const;
		void SetPosition(const VerticalPosition& vertical, const HorizontalPosition& horizontal);
		void SetOnProgressChanged(function<void(int)> onProgressChanged) { onProgressChanged_ = onProgressChanged; }
		void SetRange(const int min, const int max);
		const int GetValue() const;
		void SetValue(const int progress);
		void SetHeader(const char* header);
		void SetHeaderf(const char* header, ...);
		void SetPrefix(const char* prefix);
		void SetSuffix(const char* suffix);
		int GetMin() const { return min_; }
		int GetMax() const { return max_; }
		void SetUnit(const char* unit);

	  private:
		SliderWindow()
			: window_(nullptr), slider_(nullptr), header_(nullptr), value_(nullptr), prefix_(nullptr), suffix_(nullptr),
			  min_(0), max_(100), onProgressChanged_([](int) {})
		{
		}

		ZKWindow* window_;
		ZKSeekBar* slider_;
		ZKTextView* header_;
		ZKTextView* value_;
		ZKTextView* prefix_;
		ZKTextView* suffix_;
		int min_, max_;
		String<5> unit_;
		function<void(int)> onProgressChanged_;
		bool displayRaw_ = false;
	};

	void OpenSliderNumPad(const char* header,
						  const char* prefix,
						  const char* suffix,
						  const char* unit,
						  const int min,
						  const int max,
						  const int value,
						  function<void(int)> callback,
						  bool displayRaw = false);
} // namespace UI

#endif /* JNI_LOGIC_UI_POPUP_H_ */
