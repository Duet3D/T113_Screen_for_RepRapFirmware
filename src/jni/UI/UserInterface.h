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
	/// @brief Used to display an item in a list view and a callback for when the item is selected
	/// @param id The name of the item. The string will be used to look up the translation
	/// @param callback The function to call when the item is selected
	struct WindowSelectItem
	{
		const char* id;			   // name of the window. The string will be used to look up the translation
		function<void()> callback; // function to call when the item is selected
	};

	class Window
	{
	  public:
		static Window& GetInstance();
		void AddHome(ZKBase* home);
		void ClearHome();
		void OpenWindow(const int windowId);
		void OpenWindow(ZKBase* window);
		void OpenOverlay(const int windowId, bool closeAlreadyOpened = true);
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
		std::vector<ZKBase*> m_openedWindows;
		std::vector<ZKBase*> m_closedWindows;
		std::vector<ZKBase*> m_homeWindows;
		std::vector<ZKBase*> m_overlayWindows;
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

		ToolsList();
		void Init(ZKListView* toolListView);
		void CalculateTotalHeaterCount();
		size_t GetTotalHeaterCount(const bool calculate, const bool addTools = true, const bool addBeds = true,
								   const bool addChambers = true);
		void ObtainListItemData(ZKListView::ZKListItem* pListItem,
								int index,
								const int nameId,
								const int statusId,
								const int currentId,
								const int activeId,
								const int standbyId);
		void OnListItemClick(int index, int id, const int nameId, int statusId, int activeId, int standbyId);
		void RefreshToolList(const bool lengthChanged = true);
		static void RefreshAllToolLists(const bool lengthChanged = true);
		void OpenNumPad(const NumPadData data);
		bool SendTempTarget(int value);
		bool SendSpindleTarget(int value);
		ToolListItemData GetToolListItemDataBySlot(const size_t listIndex);

	  private:
		size_t m_toolCount, m_bedCount, m_chamberCount;
		NumPadData m_numPadData;
		ZKListView* m_pToolListView = nullptr;
	};

	class Console
	{
	  public:
		Console() {}
		static Console& GetInstance()
		{
			static Console console;
			return console;
		}
		void Init(ZKListView* console, ZKEditText* input);
		void AddCommand(const std::string& command);
		void AddResponse(const char* str);
		void AddResponse(const StringRef& ref);
		void AddLineBreak();
		size_t GetItemCount() const { return m_buffer.GetFilled(); }
		String<MAX_RESPONSE_LINE_LENGTH> GetItem(size_t index) { return m_buffer.GetItem(index); }
		void Refresh();
		void Clear();

	  private:
		void AddMessage(const StringRef& ref);
		void AddMessage(const char* str);

		CircularBuffer<String<MAX_RESPONSE_LINE_LENGTH>, MAX_RESPONSE_LINES> m_buffer;
		ZKListView* m_console = nullptr;
		ZKEditText* m_input = nullptr;
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
		return dynamic_cast<T*>(control);
	}

	/**
	 * Sets the relative position of an icon within a ZKButton control.
	 *
	 * @param control The ZKButton control.
	 * @param xPos The x-position of the icon center as a percentage of the control's width.
	 * Relative to the control's top-left corner. 0.0 is the left edge, 1.0 is the right edge.
	 * @param yPos The y-position of the icon center as a percentage of the control's height.
	 * Relative to the control's top-left corner. 0.0 is the top edge, 1.0 is the bottom edge.
	 * @param scale The scale factor to apply to the icon's size. Scaled based on the smallest of the control's width or
	 * height.
	 */
	void SetIconRelativePosition(ZKButton* control, float xPos, float yPos, float scale);

	/**
	 * Sets the relative position of an icon within a ZKButton control.
	 *
	 * @param control The ZKButton control.
	 * @param xPos The x-position of the icon center as a percentage of the control's width.
	 * Relative to the control's top-left corner. 0.0 is the left edge, 1.0 is the right edge.
	 * @param yPos The y-position of the icon center as a percentage of the control's height.
	 * Relative to the control's top-left corner. 0.0 is the top edge, 1.0 is the bottom edge.
	 * @param xScale The scale factor to apply to the icon's width.
	 * @param yScale The scale factor to apply to the icon's height.
	 */
	void SetIconRelativePosition(ZKButton* control, float xPos, float yPos, float xScale, float yScale);

	void SetM291Popup(OM::Alert alert);

#define WINDOW Window::GetInstance()
#define NUMPAD_WINDOW NumPadWindow::GetInstance()
#define POPUP_WINDOW PopupWindow::GetInstance()
#define SLIDER_WINDOW SliderWindow::GetInstance()
#define CONSOLE Console::GetInstance()
} // namespace UI

#endif /* JNI_UI_USERINTERFACE_HPP_ */
