/*
 * Popup.h
 *
 *  Created on: 23 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_UI_POPUP_H_
#define JNI_UI_POPUP_H_

#include "control/ZKEditText.h"
#include "control/ZKListView.h"
#include "control/ZKSeekBar.h"
#include "control/ZKTextView.h"
#include "std_fixed/functional.h"
#include "window/ZKWindow.h"
#include <string>
#include <sys/types.h>
#include <vector>

#include "Configuration.h"
#include "Duet3D/General/CircularBuffer.h"
#include "Duet3D/General/StringRef.h"
#include "ObjectModel/Alert.h"
#include "ObjectModel/BedOrChamber.h"
#include "ObjectModel/Tool.h"

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

		void Open();
		void Open(function<void(void)> okCb);
		void Open(function<void(void)> okCb, function<void(void)> cancelCb);
		void Ok();
		void Cancel(bool close = true);
		void SetPosition(const VerticalPosition& vertical, const HorizontalPosition& horizontal);
		void SetTitle(const std::string& title);
		void SetText(const std::string& text);
		void SetText(const char* text);
		void SetTextf(const char* format, ...);
		void SetTextScrollable(bool scrollable);
		void SetOkBtnText(const char* text);
		void SetCancelBtnText(const char* text);
		void SetImage(const char* imagePath);
		void ShowImage(bool show);
		void SetProgress(int percent);
		void SetMode(OM::Alert::Mode mode);
		void PreventClosing(bool prevent);
		void Close();

		void OkVisible(bool visible);
		void CancelVisible(bool visible);
		void SelectionVisible(bool visible);
		void NumberInputVisible(bool visible);
		void TextInputVisible(bool visible);
		void WarningTextVisible(bool visible);
		void MinTextVisible(bool visible);
		void MaxTextVisible(bool visible);
		void AxisJogVisible(bool visible);

		void SetMinTextf(const char* format, ...);
		void SetMaxTextf(const char* format, ...);
		void SetWarningTextf(const char* format, ...);
		void SetNumberInput(int32_t val);
		void SetNumberInput(float val);
		void SetNumberInput(const char* text);
		void SetTextInput(const char* text);
		void SetAxisJogSelection(uint32_t axisControl);

		const char* GetNumberInput() const;
		const char* GetTextInput() const;

		void CancelTimeout();
		void SetTimeout(uint32_t timeout);
		const uint32_t GetTimeout() const { return m_timeout; }

		bool IsOpen() const { return m_window->isVisible(); }
		bool IsBlocking() const;
		bool IsResponse() const;
		void Clear();

		bool ValidateIntegerInput(const char* text);
		bool ValidateFloatInput(const char* text);
		bool ValidateTextInput(const char* text);

		const OM::Alert::Mode GetMode() const { return m_mode; }

		OM::Move::Axis* GetJogAxis(int listIndex) const;
		size_t GetJogAxisCount() const;

		const float jogAmounts[6] = {2.0f, 0.2f, 0.02f, -0.02f, -0.2f, -2.0f};
		int selectedAxis = 0;

	  private:
		PopupWindow();
		bool ValidateIntegerInputInner(const char* text);
		bool ValidateFloatInputInner(const char* text);
		bool ValidateTextInputInner(const char* text);

		ZKWindow* m_window = nullptr;
		ZKWindow* m_noTouchWindow = nullptr;
		ZKButton* m_okBtn = nullptr;
		ZKButton* m_cancelBtn = nullptr;
		ZKTextView* m_title = nullptr;
		ZKTextView* m_text = nullptr;
		ZKTextView* m_warningText = nullptr;
		ZKTextView* m_minText = nullptr;
		ZKTextView* m_maxText = nullptr;
		ZKListView* m_choicesList = nullptr;
		ZKEditText* m_textInput = nullptr;
		ZKEditText* m_numberInput = nullptr;
		ZKListView* m_axisJogSelection = nullptr;
		ZKListView* m_axisJogAdjustment = nullptr;
		ZKTextView* m_image = nullptr;
		ZKSeekBar* m_progress = nullptr;
		function<void(void)> m_okCb;
		function<void(void)> m_cancelCb;
		OM::Alert::Mode m_mode;
		OM::Move::Axis* m_axes[MAX_TOTAL_AXES];
		uint32_t m_timeout;
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
				  const int min,
				  const int max,
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
			: m_window(nullptr), m_header(nullptr), m_value(nullptr), m_onValueChanged([](int) {}),
			  m_onConfirm([](int) {})
		{
		}
		bool ValidateInput(int value);
		void Callback();

		ZKWindow* m_window;
		ZKTextView* m_header;
		int m_min, m_max;
		ZKTextView* m_value;
		function<void(int)> m_onValueChanged;
		function<void(int)> m_onConfirm;
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
		void SetOnProgressChanged(function<void(int)> onProgressChanged) { m_onProgressChanged = onProgressChanged; }
		void SetRange(const int min, const int max);
		const int GetValue() const;
		void SetValue(const int progress);
		void SetHeader(const char* header);
		void SetHeaderf(const char* header, ...);
		void SetPrefix(const char* prefix);
		void SetSuffix(const char* suffix);
		int GetMin() const { return m_min; }
		int GetMax() const { return m_max; }
		void SetUnit(const char* unit);

	  private:
		SliderWindow()
			: m_window(nullptr), m_slider(nullptr), m_header(nullptr), m_value(nullptr), m_prefix(nullptr),
			  m_suffix(nullptr), m_min(0), m_max(100), m_onProgressChanged([](int) {})
		{
		}

		ZKWindow* m_window;
		ZKSeekBar* m_slider;
		ZKTextView* m_header;
		ZKTextView* m_value;
		ZKTextView* m_prefix;
		ZKTextView* m_suffix;
		int m_min, m_max;
		String<5> m_unit;
		function<void(int)> m_onProgressChanged;
		bool m_displayRaw = false;
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

#endif /* JNI_UI_POPUP_H_ */
