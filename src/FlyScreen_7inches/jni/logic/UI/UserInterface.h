/*
 * MainMenuFunc.h
 *
 *  Created on: 18 Dec 2023
 *      Author: Andy E
 */

#ifndef JNI_LOGIC_UI_USERINTERFACE_HPP_
#define JNI_LOGIC_UI_USERINTERFACE_HPP_

#include "control/ZKEditText.h"
#include "control/ZKListView.h"
#include "control/ZKTextView.h"
#include "window/ZKWindow.h"
#include <string>
#include <sys/types.h>
#include <vector>

#include "Duet3D/General/CircularBuffer.h"
#include "Duet3D/General/StringRef.h"
#include "Duet3D/General/function_ref.h"
#include "ObjectModel/BedOrChamber.h"
#include "ObjectModel/Tool.h"
#include "UI/UserInterfaceConstants.h"

namespace UI
{
	enum class SlideWindowIndex
	{
		move = 0,
		extrude,
		status,
		heightmap,
		fans,
		print,
		network,
		settings,
	};

	class Window
	{
	  public:
		static Window* GetInstance();
		void AddHome(ZKWindow* home);
		void ClearHome();
		void OpenWindow(ZKWindow* window);
		void OpenOverlay(ZKWindow* window);
		bool CloseOverlay();
		size_t ReOpenLastWindow(size_t numWindows);
		void CloseLastWindow();
		void CloseWindow(ZKWindow* window, const bool returnable = true);
		void Back();
		void Home();
		void Clear();

	  private:
		std::vector<ZKWindow*> openedWindows_;
		std::vector<ZKWindow*> closedWindows_;
		std::vector<ZKWindow*> homeWindows_;
		ZKWindow* overlayWindow_ = nullptr;
	};

	class ConfirmationWindow
	{
	  public:
		ConfirmationWindow() : okCb_([]() {}), cancelCb_([]() {}) {}
		static ConfirmationWindow* GetInstance()
		{
			static ConfirmationWindow cWindow;
			return &cWindow;
		}
		void Init(ZKWindow* window, ZKButton* okBtn, ZKButton* cancelBtn, ZKTextView* text);
		void Open(function_ref<void(void)> okCb);
		void Open(function_ref<void(void)> okCb, function_ref<void(void)> cancelCb);
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
		void SetText(const char* text);
		void SetTextf(const char* format, ...);
		void SetOkBtnText(const char* text);
		void SetCancelBtnText(const char* text);
		void Close();

	  private:
		ZKWindow* window_ = nullptr;
		ZKButton* okBtn_ = nullptr;
		ZKButton* cancelBtn_ = nullptr;
		ZKTextView* text_ = nullptr;
		function_ref<void(void)> okCb_;
		function_ref<void(void)> cancelCb_;
	};

	enum class HeaterType
	{
		tool = 0,
		bed,
		chamber
	};

	struct NumPadData
	{
		HeaterType heaterType;
		std::string numPadStr;
		bool active;
		union {
			struct
			{
				size_t toolIndex;
				size_t toolHeaterIndex;
			};
			struct
			{
				size_t bedOrChamberIndex;
			};
		};
	};

	class ToolsList
	{
	  public:
		ToolsList(const char* id);
		static ToolsList* Create(const char* id);
		static ToolsList* Get(const char* id);
		void Init(ZKListView* toolListView, ZKWindow* numPadWindow, ZKTextView* numPadInput);
		void CalculateTotalHeaterCount();
		size_t GetTotalHeaterCount(const bool calculate, const bool addTools = true, const bool addBeds = true,
								   const bool addChambers = true);
		void ObtainListItemData(ZKListView::ZKListItem* pListItem, int index, ZKListView::ZKListSubItem* pToolName,
								ZKListView::ZKListSubItem* pCurrentTemperature,
								ZKListView::ZKListSubItem* pActiveTemperature,
								ZKListView::ZKListSubItem* pStandbyTemperature, ZKListView::ZKListSubItem* pStatus);
		void OnListItemClick(int index, int id, const int nameId, int statusId, int activeId, int standbyId);
		void RefreshToolList(const bool lengthChanged = true);
		static void RefreshAllToolLists(const bool lengthChanged = true);
		void OpenNumPad(const NumPadData data);
		void CloseNumPad();
		void NumPadAddOneChar(char ch);
		void NumPadDelOneChar();
		bool SendTempTarget();

	  private:
		const char* id;
		size_t toolCount_, bedCount_, chamberCount_;
		NumPadData numPadData_;
		ZKListView* pToolListView_ = nullptr;
		ZKWindow* pNumPadWindow_ = nullptr;
		ZKTextView* pNumPadInput_ = nullptr;
	};

	int8_t GetToolHeaterIndex(const size_t listIndex, OM::Tool*& tool);

	class Console
	{
	  public:
		Console() {}
		static Console* GetInstance()
		{
			static Console console;
			return &console;
		}
		void Init(ZKListView* console, ZKEditText* input);
		void AddCommand(const std::string& command);
		void AddResponse(const StringRef& ref);
		void AddLineBreak();
		String<MaxResponseLineLength> GetItem(size_t index) { return buffer_.GetItem(index); }
		void Refresh();
		void Clear();

	  private:
		void AddMessage(const StringRef& ref);
		void AddMessage(const char* str);

		CircularBuffer<String<MaxResponseLineLength>, MaxResponseLines> buffer_;
		ZKListView* pConsole_ = nullptr;
		ZKEditText* pInput_ = nullptr;
	};

	void SetSelectedFile(const std::string& filePath);
	std::string& GetSelectedFile();
	void RunSelectedFile();

#define WINDOW Window::GetInstance()
#define CONF_WINDOW ConfirmationWindow::GetInstance()
#define CONSOLE Console::GetInstance()
} // namespace UI

#endif /* JNI_LOGIC_UI_USERINTERFACE_HPP_ */
