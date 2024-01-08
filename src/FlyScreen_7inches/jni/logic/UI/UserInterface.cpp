/*
 * UserInterface.cpp
 *
 *  Created on: 18 Dec 2023
 *      Author: andy
 */

#define DEBUG (1)
#include "Debug.hpp"
#include <algorithm>
#include "UserInterface.hpp"
#include "ObjectModel/Tool.hpp"
#include "ObjectModel/Files.hpp"


template<typename T>
bool InVector(std::vector<T> &vec, T item)
{
	auto it =  std::find(vec.begin(), vec.end(), item);
	if (it != vec.end())
	{
		return true;
	}
	return false;
}

template<typename T>
bool RemoveFromVector(std::vector<T> &vec, T item)
{
	auto it =  std::find(vec.begin(), vec.end(), item);
	if (it != vec.end())
	{
		dbg("Window: removing window %d from vector", item->getID());
		vec.erase(it);
		return true;
	}
	return false;
}

template<typename T>
bool AddToVector(std::vector<T> &vec, T item)
{
	RemoveFromVector<T>(vec, item);
	dbg("Window: adding window %d to vector", item->getID());
	vec.push_back(item);
	return true;
}

namespace UI
{
	Window* Window::GetInstance()
	{
		static Window window;
		return &window;
	}

	void Window::SetHome(ZKWindow* home)
	{
		home_ = home;
	}

	void Window::OpenWindow(ZKWindow* window)
	{
		dbg("Opening window %d", window->getID());
		window->showWnd();
		RemoveFromVector<ZKWindow*>(closedWindows_, window);
		if (!closedWindows_.empty())
			AddToVector<ZKWindow*>(openedWindows_, window);
	}

	size_t Window::ReOpenLastWindow(size_t numWindows)
	{
		size_t count = 0;
		for (size_t i=0; i < numWindows; ++i)
		{
			if (closedWindows_.empty())
				return count;
			auto window = closedWindows_.back();
			OpenWindow(window);
		}
		return count;
	}

	void Window::CloseLastWindow()
	{
		if (openedWindows_.empty())
			return;
		CloseWindow(openedWindows_.back());
	}

	void Window::CloseWindow(ZKWindow* window, const bool returnable)
	{
		dbg("Closing window %d", window->getID());
		window->hideWnd();
		RemoveFromVector<ZKWindow*>(openedWindows_, window);
		if (returnable)
			AddToVector<ZKWindow*>(closedWindows_, window);
	}

	void Window::Back()
	{
		if (OM::IsInSubFolder())
		{
			OM::RequestFiles(OM::GetParentDirPath());
			return;
		}
		if (!openedWindows_.empty())
		{
			ZKWindow* lastOpened = openedWindows_.back();
			dbg("Hiding window %d", lastOpened->getID());
			lastOpened->hideWnd();
		}
		if (!closedWindows_.empty())
		{
			ZKWindow* lastClosed = closedWindows_.back();
			OpenWindow(lastClosed);
		}
	}

	void Window::Home()
	{
		for (auto window : openedWindows_)
		{
			window->hideWnd();
		}
		home_->showWnd();
		Clear();
	}

	void Window::Clear()
	{
		openedWindows_.clear();
		closedWindows_.clear();
	}

	ToolsList* ToolsList::GetInstance()
	{
		static ToolsList instance;
		return &instance;
	}

	void ToolsList::Init(ZKListView* toolListView, ZKWindow* numPadWindow, ZKTextView* numPadInput)
	{
		pToolListView_ = toolListView;
		pNumPadWindow_ = numPadWindow;
		pNumPadInput_ = numPadInput;
	}

	void ToolsList::CalculateTotalHeaterCount(
				const bool addTools,
				const bool addBeds,
				const bool addChambers)
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
					count += tool->GetHeaterCount();
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
		totalCount_ = count;
	}

	void ToolsList::RefreshToolList(const bool lengthChanged)
	{
		if (lengthChanged)
		{
			CalculateTotalHeaterCount();
		}
		pToolListView_->refreshListView();
	}

	void ToolsList::OpenNumPad(const NumPadData data)
	{
		numPadData_.numPadStr = "";
		pNumPadInput_->setText("");
		numPadData_.heaterType = data.heaterType;
		numPadData_.active = data.active;

		switch (data.heaterType)
		{
		case HeaterType::tool:
			numPadData_.toolIndex = data.toolIndex;
			numPadData_.toolHeaterIndex = data.toolHeaterIndex;
			break;
		case HeaterType::bed:
		case HeaterType::chamber:
			numPadData_.bedOrChamberIndex = data.bedOrChamberIndex;
			break;
		}
		WINDOW->OpenWindow(pNumPadWindow_);
	}

	void ToolsList::CloseNumPad()
	{
		WINDOW->CloseWindow(pNumPadWindow_, false);
		WINDOW->Home();
	}

	void ToolsList::NumPadAddOneChar(char ch)
	{
		numPadData_.numPadStr += ch;
		pNumPadInput_->setText(numPadData_.numPadStr);
	}

	void ToolsList::NumPadDelOneChar()
	{
		if (!numPadData_.numPadStr.empty())
		{
			numPadData_.numPadStr.erase(numPadData_.numPadStr.length() - 1, 1);
			pNumPadInput_->setText(numPadData_.numPadStr);
		}
	}

	bool ToolsList::SendTempTarget()
	{
		if (numPadData_.numPadStr.empty())
			return false;

		int32_t target = atoi(numPadData_.numPadStr.c_str());
		OM::Tool* tool = nullptr;
		OM::BedOrChamber* bedOrChamber = nullptr;
		dbg("onListItemClick heaterType=%d", numPadData_.heaterType);
		switch (numPadData_.heaterType)
		{
		case HeaterType::tool:
			tool = OM::GetTool(numPadData_.toolIndex);
			if (tool == nullptr)
				return false;

			tool->SetHeaterTemps(numPadData_.toolHeaterIndex, target, numPadData_.active);
			break;
		case HeaterType::bed:
			bedOrChamber = OM::GetBed(numPadData_.bedOrChamberIndex);
			if (bedOrChamber == nullptr)
				return false;

			bedOrChamber->SetBedTemp(target, numPadData_.active);
			break;
		case HeaterType::chamber:
			bedOrChamber = OM::GetChamber(numPadData_.bedOrChamberIndex);
			if (bedOrChamber == nullptr)
				return false;

			bedOrChamber->SetChamberTemp(target, numPadData_.active);
			break;
		}

		return true;
	}

	int8_t GetToolHeaterIndex(const size_t listIndex, OM::Tool *&tool)
	{
		uint8_t count = 0;
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

	void Console::Init(ZKListView* console, ZKEditText* input)
	{
		pConsole_ = console;
		pInput_ = input;
	}

	void Console::AddCommand(const std::string &command)
	{
		dbg("AddingCommand %s", command.c_str());
	    String<MaxResponseLineLength> line;
	    AddLineBreak();
	    line.catf("> %s:", command.c_str());
	    AddMessage(line.GetRef());
	    AddLineBreak();
	    Refresh();
	}

	void Console::AddResponse(const StringRef& ref)
	{
		AddMessage(ref);
	}

	void Console::AddLineBreak()
	{
	    AddMessage("");
	}

	void Console::AddMessage(const char* str)
	{
		String<MaxResponseLineLength> line;
		line.copy(str);
		buffer_.Push(line);
		dbg("resp: adding line to Console buffer_[%d] = %s", buffer_.GetHead(), line.c_str());
	}

	void Console::AddMessage(const StringRef& ref)
	{
		String<MaxResponseLineLength> line;
		line.copy(ref.c_str());
		buffer_.Push(line);
		dbg("resp: adding line to Console buffer_[%d] = %s", buffer_.GetHead(), line.c_str());
	}

	void Console::Refresh()
	{
		pConsole_->refreshListView();
		if (!buffer_.Full())
			pConsole_->setSelection(buffer_.GetHead() - pConsole_->getRows());
	}

	void Console::Clear()
	{
		String<MaxResponseLineLength> line;
		line.copy("");
		for (size_t i = 0; i < MaxResponseLines; i++)
		{
			buffer_.Push(line);
		}
		buffer_.Reset();
		Refresh();
	}
}


