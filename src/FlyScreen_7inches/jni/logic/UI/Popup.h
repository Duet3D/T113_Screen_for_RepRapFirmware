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
	class PopupWindow
	{
	  public:
		PopupWindow() : okCb_([]() {}), cancelCb_([]() {}) {}
		static PopupWindow* GetInstance()
		{
			static PopupWindow cWindow;
			return &cWindow;
		}
		void Init(ZKWindow* window,
				  ZKButton* okBtn,
				  ZKButton* cancelBtn,
				  ZKTextView* title,
				  ZKTextView* text,
				  ZKTextView* warningText,
				  ZKTextView* minText,
				  ZKTextView* maxText,
				  ZKListView* choicesList,
				  ZKEditText* textInput,
				  ZKEditText* numberInput);
		void Open();
		void Open(function<void(void)> okCb);
		void Open(function<void(void)> okCb, function<void(void)> cancelCb);
		void Ok()
		{
			okCb_();
			Close();
		}
		void Cancel()
		{
			cancelCb_();
			Close();
		}
		void SetText(const std::string& text);
		void SetText(const char* text);
		void SetTextf(const char* format, ...);
		void SetOkBtnText(const char* text);
		void SetCancelBtnText(const char* text);
		void Close();

		bool IsOpen() const { return window_->isVisible(); }
		bool IsBlocking() const;
		bool IsResponse() const;

	  private:
		ZKWindow* window_ = nullptr;
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
		function<void(void)> okCb_;
		function<void(void)> cancelCb_;
		OM::Alert::Mode mode_;
	};

	class SliderWindow
	{
	  public:
		SliderWindow()
			: window_(nullptr), slider_(nullptr), header_(nullptr), value_(nullptr), prefix_(nullptr), suffix_(nullptr),
			  min_(0), max_(100), onProgressChanged_([](int) {})
		{
		}
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
		void Open(const char* header,
				  const char* prefix,
				  const char* suffix,
				  const char* unit,
				  const int min,
				  const int max,
				  const int value,
				  function<void(int)> onProgressChanged,
				  bool displayRaw = false);
		void Callback() const;
		void SetOnProgressChanged(function<void(int)> onProgressChanged) { onProgressChanged_ = onProgressChanged; }
		void SetRange(const int min, const int max);
		const int GetValue() const;
		void SetValue(const int progress);
		void SetHeader(const char* header);
		void SetHeaderf(const char* header, ...);
		void SetPrefix(const char* prefix);
		void SetSuffix(const char* suffix);
		void SetUnit(const char* unit);

	  private:
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
} // namespace UI

#endif /* JNI_LOGIC_UI_POPUP_H_ */
