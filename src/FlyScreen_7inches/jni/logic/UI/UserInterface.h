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
#include "control/ZKSeekBar.h"
#include "control/ZKTextView.h"
#include "std_fixed/functional.h"
#include "window/ZKWindow.h"
#include <string>
#include <sys/types.h>
#include <vector>

#include "Duet3D/General/CircularBuffer.h"
#include "Duet3D/General/StringRef.h"
#include "Library/Thumbnail.h"
#include "ObjectModel/BedOrChamber.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Tool.h"
#include "UI/Popup.h"
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
	};

	class Window
	{
	  public:
		static Window* GetInstance();
		void AddHome(ZKBase* home);
		void ClearHome();
		void OpenWindow(ZKBase* window);
		void OpenOverlay(ZKBase* window, bool closeAlreadyOpened = true);
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

	struct NumPadData
	{
		HeaterType heaterType;
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

	  private:
		const char* id;
		size_t toolCount_, bedCount_, chamberCount_;
		NumPadData numPadData_;
		ZKListView* pToolListView_ = nullptr;
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
		void AddResponse(const char* str);
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

	void SetSelectedFile(const OM::FileSystem::File* file);
	const OM::FileSystem::File* GetSelectedFile();
	void RunSelectedFile();
	bool UpdateFileThumbnailChunk(const struct Thumbnail& thumbnail,
								  uint32_t pixels_offset,
								  const qoi_rgba_t* pixels,
								  size_t pixels_count);

#define WINDOW Window::GetInstance()
#define NUMPAD_WINDOW NumPadWindow::GetInstance()
#define POPUP_WINDOW PopupWindow::GetInstance()
#define SLIDER_WINDOW SliderWindow::GetInstance()
#define CONSOLE Console::GetInstance()
} // namespace UI

#endif /* JNI_LOGIC_UI_USERINTERFACE_HPP_ */
