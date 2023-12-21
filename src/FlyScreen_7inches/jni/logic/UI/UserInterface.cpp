/*
 * UserInterface.cpp
 *
 *  Created on: 18 Dec 2023
 *      Author: andy
 */

#include <algorithm>
#include "UserInterface.hpp"
#include "UserInterfaceConstants.hpp"
#include "ObjectModel/Tool.hpp"

#define DEBUG (1)
#include "Debug.hpp"

template<typename T>
bool removeFromVector(std::vector<T> &vec, T item)
{
	auto it =  std::find(vec.begin(), vec.end(), item);
	if (it != vec.end())
	{
		vec.erase(it);
		return true;
	}
	return false;
}

template<typename T>
bool addToVector(std::vector<T> &vec, T item)
{
	removeFromVector<T>(vec, item);
	vec.push_back(item);
	return true;
}

namespace UI
{
	static Window* Window::GetInstance()
	{
		static Window window;
		return &window;
	}

	void Window::OpenWindow(ZKWindow* window)
	{
		dbg("Opening window %d", window->getID());
		window->showWnd();
		removeFromVector<ZKWindow*>(closedWindows, window);
		addToVector<ZKWindow*>(openedWindows, window);

	}

	size_t Window::ReOpenLastWindow(size_t numWindows)
	{
		size_t count = 0;
		for (size_t i=0; i < numWindows; ++i)
		{
			if (closedWindows.empty())
				return count;
			auto window = closedWindows.back();
			OpenWindow(window);
		}
		return count;
	}

	void Window::CloseWindow(ZKWindow* window, const bool returnable)
	{
		dbg("Closing window %d", window->getID());
		window->hideWnd();
		removeFromVector<ZKWindow*>(openedWindows, window);
		if (returnable)
			addToVector<ZKWindow*>(closedWindows, window);
	}

	void Window::Back()
	{
		if (!openedWindows.empty())
		{
			ZKWindow* lastOpened = openedWindows.back();
			dbg("Hiding window %d", lastOpened->getID());
			lastOpened->hideWnd();
		}
		if (!closedWindows.empty())
		{
			ZKWindow* lastClosed = closedWindows.back();
			OpenWindow(lastClosed);
		}
	}

	void Window::Home()
	{
		for (auto window : openedWindows)
		{
			window->hideWnd();
		}
		for (auto window : closedWindows)
		{
			window->showWnd();
		}
		Clear();
	}

	void Window::Clear()
	{
		openedWindows.clear();
		closedWindows.clear();
	}

	static ToolsList* ToolsList::GetInstance()
	{
		static ToolsList instance;
		return &instance;
	}

	void ToolsList::Init(ZKListView* toolListView, ZKWindow* numPadWindow, ZKTextView* numPadInput)
	{
		this->toolListView = toolListView;
		this->numPadWindow = numPadWindow;
		this->numPadInput = numPadInput;
	}

	void ToolsList::CalculateTotalHeaterCount(
				const bool addTools = true,
				const bool addBeds = true,
				const bool addChambers = true)
	{
		size_t count = 0;
		if (addBeds)
		{
			dbg("Bed count %d", OM::GetBedCount());
			count += OM::GetBedCount();
		}
		if (addChambers)
		{
			dbg("Chamber count %d", OM::GetChamberCount());
			count += OM::GetChamberCount();
		}

		if (addTools)
		{
			OM::IterateToolsWhile([&count](OM::Tool*& tool, size_t)
			{
				const bool hasHeater = tool->heaters[0] != nullptr;
				const bool hasSpindle = tool->spindle != nullptr;
				// Spindle takes precedence
				if (hasSpindle)
				{
					++count;
				}
				else if (hasHeater)
				{
					tool->IterateHeaters([&count, &tool](OM::Heat::Heater*, size_t)
							{
								++count;
							});
				}
				else
				{
					// Hides everything by default
					++count;
				}
				return true;
			});
		}
		if (count > MaxSlots)
		{
			dbg("Total heater items (%d) exceeds MaxSlots (%d)", count, MaxSlots);
			count = MaxSlots;
		}
		totalCount = count;
	}

	void ToolsList::RefreshToolList(const bool lengthChanged)
	{
		if (lengthChanged)
		{
			CalculateTotalHeaterCount();
		}
		toolListView->refreshListView();
	}

	void ToolsList::OpenNumPad(const size_t toolIndex, const size_t toolHeaterIndex, const bool active)
	{
		numPadData.numPadStr = "";
		numPadInput->setText("");
		numPadData.active = active;
		numPadData.toolIndex = toolIndex;
		numPadData.toolHeaterIndex = toolHeaterIndex;
		WINDOW->OpenWindow(numPadWindow);
	}

	void ToolsList::CloseNumPad()
	{
		WINDOW->CloseWindow(numPadWindow, false);
		WINDOW->Home();
	}

	void ToolsList::NumPadAddOneChar(char ch)
	{
		numPadData.numPadStr += ch;
		numPadInput->setText(numPadData.numPadStr);
	}

	void ToolsList::NumPadDelOneChar()
	{
		if (!numPadData.numPadStr.empty())
		{
			numPadData.numPadStr.erase(numPadData.numPadStr.length() - 1, 1);
			numPadInput->setText(numPadData.numPadStr);
		}
	}

	bool ToolsList::SendTempTarget()
	{
		OM::Tool *tool = OM::GetTool(numPadData.toolIndex);
		if (tool == nullptr)
			return false;

		if (numPadData.numPadStr.empty())
			return false;

		int32_t target = atoi(numPadData.numPadStr.c_str());
		tool->SetHeaterTemps(numPadData.toolHeaterIndex, target, numPadData.active);
		return true;
	}

	int8_t GetToolHeaterIndex(const size_t listIndex, OM::Tool *&tool)
	{
		int8_t count = 0;
		int8_t toolHeaterIndex = -1;
		OM::IterateToolsWhile([&](OM::Tool*& toolIter, size_t)
			{
				if (listIndex < count + toolIter->GetHeaterCount())
				{
					tool = toolIter;
					toolHeaterIndex = listIndex - count;
					return false;
				}
				count += toolIter->GetHeaterCount();
				return true;
			});
		if (toolHeaterIndex > -1)
		{
//			dbg("List index %d is Tool %d toolHeaterIndex %d=%d", listIndex, tool->index, toolHeaterIndex, tool->GetHeater(toolHeaterIndex)->index);
			return toolHeaterIndex;
		}
//		dbg("List index %d is not in tool heaters (total tool heaters %d)", listIndex, count);
		tool = nullptr;
		return count;				// list index is greater than all heaters for tools
	}
}


