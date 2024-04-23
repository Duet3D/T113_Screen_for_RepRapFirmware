/*
 * MainMenuFunc.h
 *
 *  Created on: 18 Dec 2023
 *      Author: Andy E
 */

#ifndef JNI_UI_USERINTERFACE_HPP_
#define JNI_UI_USERINTERFACE_HPP_

#include "activity/mainActivity.h"
#include "control/ZKBase.h"
#include "control/ZKEditText.h"
#include "control/ZKListView.h"
#include "control/ZKSeekBar.h"
#include "control/ZKTextView.h"
#include "std_fixed/functional.h"
#include "window/ZKWindow.h"
#include <string>
#include <sys/types.h>
#include <vector>

#include "Comm/FileInfo.h"
#include "Configuration.h"
#include "Duet3D/General/CircularBuffer.h"
#include "Duet3D/General/String.h"
#include "Duet3D/General/StringRef.h"
#include "Duet3D/General/Vector.h"
#include "ObjectModel/BedOrChamber.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Tool.h"
#include "UI/Popup.h"

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
		debug,
		object_cancel,
		webcam,
	};

	enum class SettingsSlideWindowIndex
	{
		language = 0,
		duet,
		update,
		restart,
		dev,
		power_off,
		zk_setting,
		touch_calibration,
		guides,
		brightness,
		theme,
		screensaver,
		buzzer,
		webcam,
	};

	extern uint32_t g_extrusionFeedRates[5];
	extern uint32_t g_extrusionFeedDistances[7];
	extern uint32_t g_defaultExtrusionFeedRate;
	extern uint32_t g_defaultExtrusionFeedDistance;

	class Window
	{
	  public:
		static Window* GetInstance();
		void AddHome(ZKBase* home);
		void ClearHome();
		void OpenWindow(ZKBase* window);
		void OpenOverlay(ZKBase* window, bool closeAlreadyOpened = true);
		bool IsOverlayOpened();
		bool CloseOverlay();
		size_t ReOpenLastWindow(size_t numWindows);
		void CloseLastWindow();
		void CloseWindow(ZKBase* window, const bool returnable = true);
		void Back();
		void Home();
		void Clear();

	  private:
		std::vector<ZKBase*> openedWindows_;
		std::vector<ZKBase*> closedWindows_;
		std::vector<ZKBase*> homeWindows_;
		std::vector<ZKBase*> overlayWindows_;
	};

	enum class HeaterType
	{
		tool = 0,
		bed,
		chamber
	};

	class ToolsList
	{
	  public:
		struct ToolListItemData
		{
			OM::Tool* tool;
			int8_t toolHeaterIndex;
			OM::ToolHeater* toolHeater;
			OM::Spindle* spindle;

			ToolListItemData()
			{
				tool = nullptr;
				toolHeaterIndex = -1;
				toolHeater = nullptr;
				spindle = nullptr;
			}
		};

		struct NumPadData
		{
			HeaterType heaterType;
			bool active;
			ToolListItemData toolData; // Tool control
			size_t bedOrChamberIndex;  // bed or chamber control
		};

		static ToolsList*
		Create(const char* id);
		static ToolsList* Get(const char* id);
		void Init(ZKListView* toolListView);
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
		bool SendTempTarget(int value);
		bool SendSpindleTarget(int value);
		ToolListItemData GetToolListItemDataBySlot(const size_t listIndex);

	  private:
		ToolsList(const char* id);

		const char* id;
		size_t toolCount_, bedCount_, chamberCount_;
		NumPadData numPadData_;
		ZKListView* pToolListView_ = nullptr;
	};

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
		void AddResponse(const char* str);
		void AddResponse(const StringRef& ref);
		void AddLineBreak();
		String<MAX_RESPONSE_LINE_LENGTH> GetItem(size_t index) { return buffer_.GetItem(index); }
		void Refresh();
		void Clear();

	  private:
		void AddMessage(const StringRef& ref);
		void AddMessage(const char* str);

		CircularBuffer<String<MAX_RESPONSE_LINE_LENGTH>, MAX_RESPONSE_LINES> buffer_;
		ZKListView* pConsole_ = nullptr;
		ZKEditText* pInput_ = nullptr;
	};

	void Init(ZKWindow* root);
	ZKWindow* GetRootWindow();
	ZKBase* GetUIControl(int id);

	template <typename T>
	T* GetUIControl(int id)
	{
		ZKBase* control = GetUIControl(id);
		if (control == nullptr)
		{
			return nullptr;
		}
		return static_cast<T*>(control);
	}

	void SetSelectedFile(const OM::FileSystem::File* file);
	const OM::FileSystem::File* GetSelectedFile();
	void RunSelectedFile();
	void SetThumbnail(ZKTextView* thumbnail);
	void SetM291Popup(OM::Alert alert);
	void SetPopupFileInfo();
	ZKTextView* GetThumbnail();

#define WINDOW Window::GetInstance()
#define NUMPAD_WINDOW NumPadWindow::GetInstance()
#define POPUP_WINDOW PopupWindow::GetInstance()
#define SLIDER_WINDOW SliderWindow::GetInstance()
#define CONSOLE Console::GetInstance()
} // namespace UI

#endif /* JNI_UI_USERINTERFACE_HPP_ */
