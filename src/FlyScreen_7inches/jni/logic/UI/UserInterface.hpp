/*
 * MainMenuFunc.hpp
 *
 *  Created on: 18 Dec 2023
 *      Author: Andy E
 */

#ifndef JNI_LOGIC_UI_USERINTERFACE_HPP_
#define JNI_LOGIC_UI_USERINTERFACE_HPP_

#include <sys/types.h>
#include <string>
#include <vector>
#include "control/ZKTextView.h"
#include "control/ZKEditText.h"
#include "control/ZKListView.h"
#include "window/ZKWindow.h"
#include "Duet3D/General/CircularBuffer.hpp"
#include "Duet3D/General/StringRef.h"
#include "ObjectModel/Tool.hpp"
#include "ObjectModel/BedOrChamber.hpp"
#include "UI/UserInterfaceConstants.hpp"

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
		static Window * GetInstance();
		void AddHome(ZKWindow* home);
		void ClearHome();
		void OpenWindow(ZKWindow* window);
		void OpenOverlay(ZKWindow* window);
		size_t ReOpenLastWindow(size_t numWindows);
		void CloseLastWindow();
		void CloseWindow(ZKWindow *window, const bool returnable = true);
		void Back();
		void Home();
		void Clear();

	private:
		std::vector<ZKWindow*> openedWindows_;
		std::vector<ZKWindow*> closedWindows_;
		std::vector<ZKWindow*> homeWindows_;
		ZKWindow* overlayWindow_ = nullptr;
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
			struct {
				size_t toolIndex;
				size_t toolHeaterIndex;
			};
			struct {
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
		size_t GetTotalHeaterCount(const bool calculate, const bool addTools =
				true, const bool addBeds = true, const bool addChambers = true);
		void ObtainListItemData(
				ZKListView::ZKListItem* pListItem, int index,
				ZKListView::ZKListSubItem* pToolName,
				ZKListView::ZKListSubItem* pCurrentTemperature,
				ZKListView::ZKListSubItem* pActiveTemperature,
				ZKListView::ZKListSubItem* pStandbyTemperature,
				ZKListView::ZKListSubItem* pStatus);
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

	int8_t GetToolHeaterIndex(const size_t listIndex, OM::Tool *&tool);

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
		void AddCommand(const std::string &command);
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

#define WINDOW Window::GetInstance()
#define TOOLSLIST ToolsList::GetInstance()
#define CONSOLE Console::GetInstance()
}

#endif /* JNI_LOGIC_UI_USERINTERFACE_HPP_ */
