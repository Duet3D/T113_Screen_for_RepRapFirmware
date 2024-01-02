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
#include "control/ZKListView.h"
#include "window/ZKWindow.h"
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
				totalCount(0)
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
			return totalCount;
		}
		void RefreshToolList(const bool lengthChanged = true);
		void OpenNumPad(const NumPadData data);
		void CloseNumPad();
		void NumPadAddOneChar(char ch);
		void NumPadDelOneChar();
		bool SendTempTarget();
	private:
		size_t totalCount;
		NumPadData numPadData;
		ZKListView* toolListView;
		ZKWindow* numPadWindow;
		ZKTextView* numPadInput;
	};

	int8_t GetToolHeaterIndex(const size_t listIndex, OM::Tool *&tool);

#define WINDOW Window::GetInstance()
#define TOOLSLIST ToolsList::GetInstance()
}

#endif /* JNI_LOGIC_UI_USERINTERFACE_HPP_ */
