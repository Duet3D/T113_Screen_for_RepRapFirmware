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
#include "ObjectModel/Tool.hpp"
#include "ObjectModel/BedOrChamber.hpp"

namespace UI
{
	enum class SlideWindowIndex
	{
		move = 0,
		extrude,
		console,
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
		void OpenWindow(ZKWindow *window);
		size_t ReOpenLastWindow(size_t numWindows);
		void CloseLastWindow();
		void CloseWindow(ZKWindow *window, const bool returnable = true);
		void Back();
		void Home();
		void Clear();

	private:
		std::vector<ZKWindow*> openedWindows;
		std::vector<ZKWindow*> closedWindows;
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
		ToolsList() :
				totalCount_(0)
		{
		}
		static ToolsList* GetInstance();
		void Init(ZKListView* toolListView, ZKWindow* numPadWindow, ZKTextView* numPadInput);
		void CalculateTotalHeaterCount(const bool addTools = true,
				const bool addBeds = true, const bool addChambers = true);
		size_t GetTotalHeaterCount(const bool calculate, const bool addTools =
				true, const bool addBeds = true, const bool addChambers = true)
		{
			if (calculate)
				CalculateTotalHeaterCount(addTools, addBeds, addChambers);
			return totalCount_;
		}
		void RefreshToolList(const bool lengthChanged = true);
		void OpenNumPad(const NumPadData data);
		void CloseNumPad();
		void NumPadAddOneChar(char ch);
		void NumPadDelOneChar();
		bool SendTempTarget();
	private:
		size_t totalCount_;
		NumPadData numPadData_;
		ZKListView* pToolListView_;
		ZKWindow* pNumPadWindow_;
		ZKTextView* pNumPadInput_;
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
		void AddResponse(String<MaxResponseLineLength> line);
		void AddLineBreak();
		String<MaxResponseLineLength> GetItem(size_t index) { return buffer_.GetItem(index); }
		void Refresh();
		void Clear();
	private:
		void AddMessage(String<MaxResponseLineLength> line);

		CircularBuffer<String<MaxResponseLineLength>, MaxResponseLines> buffer_;
		ZKListView* pConsole_;
		ZKEditText* pInput_;
	};

#define WINDOW Window::GetInstance()
#define TOOLSLIST ToolsList::GetInstance()
#define CONSOLE Console::GetInstance()
}

#endif /* JNI_LOGIC_UI_USERINTERFACE_HPP_ */
