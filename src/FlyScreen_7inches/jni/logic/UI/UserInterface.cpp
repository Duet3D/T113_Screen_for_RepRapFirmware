/*
 * UserInterface.cpp
 *
 *  Created on: 18 Dec 2023
 *      Author: andy
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "UserInterface.h"
#include "Debug.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Tool.h"
#include "utils.h"
#include <algorithm>
#include <map>

template <typename T>
bool InVector(std::vector<T>& vec, T item)
{
	auto it = std::find(vec.begin(), vec.end(), item);
	if (it != vec.end()) { return true; }
	return false;
}

template <typename T>
bool RemoveFromVector(std::vector<T>& vec, T item)
{
	auto it = std::find(vec.begin(), vec.end(), item);
	if (it != vec.end())
	{
		dbg("Removing window %d from vector", item->getID());
		vec.erase(it);
		return true;
	}
	return false;
}

template <typename T>
bool AddToVector(std::vector<T>& vec, T item)
{
	RemoveFromVector<T>(vec, item);
	dbg("Adding window %d to vector", item->getID());
	vec.push_back(item);
	return true;
}

const OM::FileSystem::File* sSelectedFile;

namespace UI
{
	uint32_t g_extrusionFeedRates[] = {50, 10, 5, 2, 1};
	uint32_t g_extrusionFeedDistances[] = {100, 50, 20, 10, 5, 2, 1};
	uint32_t g_defaultExtrusionFeedRate = 5;
	uint32_t g_defaultExtrusionFeedDistance = 10;

	Window* Window::GetInstance()
	{
		static Window window;
		return &window;
	}

	void Window::AddHome(ZKBase* home)
	{
		homeWindows_.push_back(home);
	}

	void Window::ClearHome()
	{
		homeWindows_.clear();
	}

	void Window::OpenWindow(ZKBase* window)
	{
		CloseOverlay();
		info("Opening window %d", window->getID());
		window->setVisible(true);
		RemoveFromVector<ZKBase*>(closedWindows_, window);
		if (!InVector<ZKBase*>(homeWindows_, window))
			AddToVector<ZKBase*>(openedWindows_, window);
	}

	void Window::OpenOverlay(ZKBase* overlay, bool closeAlreadyOpened)
	{
		if (overlay == nullptr)
		{
			warn("Overlay is null");
			return;
		}
		if (!overlayWindows_.empty() && closeAlreadyOpened)
		{
			CloseOverlay();
		}
		dbg("Opening overlay %d", overlay->getID());
		overlay->setVisible(true);
		AddToVector<ZKBase*>(overlayWindows_, overlay);
	}

	bool Window::CloseOverlay()
	{
		if (overlayWindows_.empty())
			return false;

		verbose("%d overlay windows to close", overlayWindows_.size());
		// Ensure that you can't close the popup window if it's blocking.
		if (POPUP_WINDOW->IsBlocking())
		{
			verbose("Popup window is blocking");
			return false;
		}
		if (POPUP_WINDOW->IsOpen())
		{
			POPUP_WINDOW->Clear();
		}
		for (auto& window : overlayWindows_)
		{
			window->setVisible(false);
			RemoveFromVector<ZKBase*>(overlayWindows_, window);
		}
		verbose("Closed overlays, %d remaining", overlayWindows_.size());
		return true;
	}

	size_t Window::ReOpenLastWindow(size_t numWindows)
	{
		size_t count = 0;
		for (size_t i = 0; i < numWindows; ++i)
		{
			if (closedWindows_.empty()) return count;
			auto window = closedWindows_.back();
			OpenWindow(window);
		}
		return count;
	}

	void Window::CloseLastWindow()
	{
		for (auto window : homeWindows_)
		{
			if (!window->isVisible())
				continue;
			CloseWindow(window);
		}
		if (openedWindows_.empty()) return;
		CloseWindow(openedWindows_.back());
	}

	void Window::CloseWindow(ZKBase* window, const bool returnable)
	{
		info("Closing window %d", window->getID());
		window->setVisible(false);
		RemoveFromVector<ZKBase*>(openedWindows_, window);
		if (returnable)
			AddToVector<ZKBase*>(closedWindows_, window);
	}

	void Window::Back()
	{
		if (POPUP_WINDOW->IsOpen())
		{
			POPUP_WINDOW->Cancel(false);
		}
		if (CloseOverlay())
		{
			return;
		}
		if (OM::FileSystem::IsInSubFolder())
		{
			info("Returning to previous folder");
			if (OM::FileSystem::IsUsbFolder())
			{
				OM::FileSystem::RequestUsbFiles(OM::FileSystem::GetParentDirPath());
				return;
			}
			OM::FileSystem::RequestFiles(OM::FileSystem::GetParentDirPath());
			return;
		}
		if (!openedWindows_.empty())
		{
			ZKBase* lastOpened = openedWindows_.back();
			CloseWindow(lastOpened, false);
		}
		if (!closedWindows_.empty())
		{
			ZKBase* lastClosed = closedWindows_.back();
			OpenWindow(lastClosed);
		}
	}

	void Window::Home()
	{
		for (auto window : openedWindows_)
		{
			window->setVisible(false);
		}
		for (auto window : homeWindows_)
		{
			window->setVisible(true);
		}
		CloseOverlay();
		Clear();
	}

	void Window::Clear()
	{
		openedWindows_.clear();
		closedWindows_.clear();
	}

	static std::map<const char*, ToolsList*> sToolsListMap;

	ToolsList::ToolsList(const char* id) : id(id), toolCount_(0), bedCount_(0), chamberCount_(0)
	{
		dbg("Registering tool list to id %s", id);
	}

	ToolsList* ToolsList::Create(const char* id)
	{
		sToolsListMap[id] = new ToolsList(id);
		return sToolsListMap[id];
	}

	ToolsList* ToolsList::Get(const char* id)
	{
		auto it = sToolsListMap.find(id);
		return (it != sToolsListMap.end()) ? it->second : nullptr;
	}

	void ToolsList::Init(ZKListView* toolListView)
	{
		pToolListView_ = toolListView;
	}

	void ToolsList::CalculateTotalHeaterCount()
	{
		verbose("Bed count %d", OM::GetBedCount());
		bedCount_ = OM::GetBedCount();

		verbose("Chamber count %d", OM::GetChamberCount());
		chamberCount_ = OM::GetChamberCount();

		size_t count = 0;
		OM::IterateToolsWhile([&count](OM::Tool*& tool, size_t) {
			const bool hasHeater = tool->heaters[0] != nullptr;
			const bool hasSpindle = tool->spindle != nullptr;
			// Spindle takes precedence
			if (hasSpindle) { ++count; }
			else if (hasHeater) { count += tool->GetHeaterCount(); }
			else
			{
				// Hides everything by default
				++count;
			}
			return true;
		});

		verbose("Tool count %d", count);
		toolCount_ = count;
	}

	size_t ToolsList::GetTotalHeaterCount(const bool calculate, const bool addTools, const bool addBeds,
										  const bool addChambers)
	{
		if (calculate) CalculateTotalHeaterCount();

		size_t count = 0;
		if (addTools) { count += toolCount_; }
		if (addBeds) { count += bedCount_; }
		if (addChambers) { count += chamberCount_; }
		return count;
	}

	void ToolsList::ObtainListItemData(ZKListView::ZKListItem* pListItem, int index,
									   ZKListView::ZKListSubItem* pToolName,
									   ZKListView::ZKListSubItem* pCurrentTemperature,
									   ZKListView::ZKListSubItem* pActiveTemperature,
									   ZKListView::ZKListSubItem* pStandbyTemperature,
									   ZKListView::ZKListSubItem* pStatus)
	{
		// Check Tools to see if list index is within tool heaters
		OM::Tool* tool = nullptr;
		int8_t toolHeaterIndex = UI::GetToolHeaterIndex(index, tool);
		if (tool != nullptr)
		{
			OM::ToolHeater* toolHeater;
			toolHeater = tool->GetHeater(toolHeaterIndex);
			if (toolHeater == nullptr)
			{
				warn("List index %d: Tool %d heaterIndex %d is null", index, tool->index, toolHeaterIndex);
				pToolName->setText(tool->name.c_str());
				return;
			}
			// dbg("List index %d: Updating Tool %d heater %d=%d temperatures %.2f:%d:%d", index, tool->index,
			// 	toolHeaterIndex, toolHeater->heater->index, toolHeater->heater->current, toolHeater->activeTemp,
			// 	toolHeater->standbyTemp);
			if (tool->GetHeaterCount() > 1) { pToolName->setTextf("%s (%d)", tool->name.c_str(), toolHeaterIndex); }
			else { pToolName->setText(tool->name.c_str()); }
			pActiveTemperature->setText((int)toolHeater->activeTemp);
			pStandbyTemperature->setText((int)toolHeater->standbyTemp);
			pCurrentTemperature->setText(toolHeater->heater->current);
			pStatus->setTextTr(toolHeater->heater->GetHeaterStatusStr());
			(tool->status == OM::ToolStatus::active) ? pListItem->setSelected(true) : pListItem->setSelected(false);

			return;
		}

		int8_t bedOrChamberIndex = index - toolHeaterIndex;
		OM::BedOrChamber* bedOrChamber = OM::GetBedBySlot(bedOrChamberIndex);
		OM::Heat::Heater* heater;
		if (bedOrChamber != nullptr)
		{
			heater = OM::Heat::GetHeater(bedOrChamber->heater);
			if (heater == nullptr)
			{
				warn("List index %d: Bed %d heater %d is null", index, bedOrChamber->index, bedOrChamber->heater);
				return;
			}
			// dbg("List index %d: Updating Bed %d heater %d=%d temperatures %.2f:%d:%d", index, bedOrChamber->index,
			// 	bedOrChamber->heater, heater->index, heater->current, heater->activeTemp, heater->standbyTemp);
			if (OM::GetBedCount() > 1)
			{
				pToolName->setTextf("%s %d", LANGUAGEMANAGER->getValue("bed").c_str(), bedOrChamber->index);
			}
			else
			{
				pToolName->setTextTr("bed");
			}
			pActiveTemperature->setText((int)heater->activeTemp);
			pStandbyTemperature->setText((int)heater->standbyTemp);
			pCurrentTemperature->setText(heater->current);
			pStatus->setText(heater->GetHeaterStatusStr());
			pListItem->setSelected(false);
			return;
		}

		bedOrChamberIndex -= OM::GetBedCount();
		bedOrChamber = OM::GetChamberBySlot(bedOrChamberIndex);
		if (bedOrChamber != nullptr)
		{
			heater = OM::Heat::GetHeater(bedOrChamber->heater);
			if (heater == nullptr)
			{
				warn("List index %d: Bed %d heater %d is null", index, bedOrChamber->index, bedOrChamber->heater);
				return;
			}
			// dbg("List index %d: Updating Chamber %d heater %d=%d temperatures %.2f:%d:%d", index,
			// bedOrChamber->index, 	bedOrChamber->heater, heater->index, heater->current, heater->activeTemp,
			// heater->standbyTemp);
			if (OM::GetChamberCount() > 1)
			{
				pToolName->setTextf("%s %d", LANGUAGEMANAGER->getValue("chamber").c_str(), bedOrChamber->index);
			}
			else
			{
				pToolName->setTextTr("chamber");
			}
			pActiveTemperature->setText((int)heater->activeTemp);
			pStandbyTemperature->setText((int)heater->standbyTemp);
			pCurrentTemperature->setText(heater->current);
			pStatus->setText(heater->GetHeaterStatusStr());
			pListItem->setSelected(false);
			return;
		}
		warn("Unknown index");
	}

	void ToolsList::OnListItemClick(int index, int id, const int nameId, int statusId, int activeId, int standbyId)
	{
		dbg("index=%d, id=%d", index, id);
		if (id == NULL) return;
		UI::NumPadData numPadData;

		CalculateTotalHeaterCount();
		if ((size_t)index < GetTotalHeaterCount(false, true, false, false))
		{
			OM::Tool* tool = nullptr;
			int8_t toolHeaterIndex = UI::GetToolHeaterIndex(index, tool);
			if (tool == nullptr) return;
			dbg("Tool index=%d", tool->index);
			numPadData.heaterType = UI::HeaterType::tool;
			numPadData.toolIndex = tool->index;
			numPadData.toolHeaterIndex = toolHeaterIndex;
			if (id == nameId)
			{
				tool->ToggleState();
				return;
			}
			if (id == statusId)
			{
				tool->ToggleHeaterState(toolHeaterIndex);
				return;
			}
		}
		else if ((size_t)index < GetTotalHeaterCount(false, true, true, false))
		{
			OM::Bed* bed = OM::GetBedBySlot(index - GetTotalHeaterCount(false, true, false, false));
			if (bed == nullptr) return;
			dbg("Bed index=%d", bed->index);
			numPadData.heaterType = UI::HeaterType::bed;
			numPadData.bedOrChamberIndex = bed->index;
			if (id == nameId || id == statusId)
			{
				bed->ToggleBedState();
				return;
			}
		}

		else if ((size_t)index < GetTotalHeaterCount(false, true, true, true))
		{
			OM::Chamber* chamber = OM::GetChamberBySlot(index - GetTotalHeaterCount(false, true, true, false));
			if (chamber == nullptr) return;
			dbg("Chamber index=%d", chamber->index);
			numPadData.heaterType = UI::HeaterType::chamber;
			numPadData.bedOrChamberIndex = chamber->index;
			if (id == nameId || id == statusId)
			{
				chamber->ToggleChamberState();
				return;
			}
		}

		if (id == activeId)
		{
			numPadData.active = true;
			OpenNumPad(numPadData);
			return;
		}
		if (id == standbyId)
		{
			numPadData.active = false;
			OpenNumPad(numPadData);
			return;
		}
	}

	void ToolsList::RefreshToolList(const bool lengthChanged)
	{
		if (lengthChanged) { CalculateTotalHeaterCount(); }
		pToolListView_->refreshListView();
	}

	void ToolsList::RefreshAllToolLists(const bool lengthChanged)
	{
		for (auto list : sToolsListMap)
		{
			list.second->RefreshToolList(lengthChanged);
		}
	}

	void ToolsList::OpenNumPad(const NumPadData data)
	{
		std::string header;
		numPadData_.heaterType = data.heaterType;
		numPadData_.active = data.active;
		int currentTarget = 0;

		switch (data.heaterType)
		{
		case HeaterType::tool: {

			header = utils::format(
				"Tool %d Heater %d %s", data.toolIndex, data.toolHeaterIndex, data.active ? "Active" : "Standby");
			numPadData_.toolIndex = data.toolIndex;
			numPadData_.toolHeaterIndex = data.toolHeaterIndex;
			OM::ToolHeater* toolHeater = OM::GetTool(data.toolIndex)->GetHeater(data.toolHeaterIndex);
			if (toolHeater == nullptr)
				return;
			currentTarget = data.active ? toolHeater->activeTemp : toolHeater->standbyTemp;
			break;
		}
		case HeaterType::bed:
		case HeaterType::chamber: {
			header = utils::format("%s %d %s",
								   (data.heaterType == HeaterType::bed) ? "Bed" : "Chamber",
								   data.bedOrChamberIndex,
								   data.active ? "Active" : "Standby");
			numPadData_.bedOrChamberIndex = data.bedOrChamberIndex;
			OM::BedOrChamber* bedOrChamber = (data.heaterType == HeaterType::bed)
												 ? OM::GetBed(data.bedOrChamberIndex)
												 : OM::GetChamber(data.bedOrChamberIndex);
			if (bedOrChamber == nullptr)
				return;
			currentTarget = data.active ? bedOrChamber->GetActiveTemp() : bedOrChamber->GetStandbyTemp();
			break;
		}
		}
		UI::NUMPAD_WINDOW->Open(
			header.c_str(), currentTarget, [](int) {}, [this](int value) { SendTempTarget(value); });
	}

	bool ToolsList::SendTempTarget(int target)
	{
		OM::Tool* tool = nullptr;
		OM::BedOrChamber* bedOrChamber = nullptr;
		dbg("heaterType=%d", (int)numPadData_.heaterType);
		switch (numPadData_.heaterType)
		{
		case HeaterType::tool:
			tool = OM::GetTool(numPadData_.toolIndex);
			if (tool == nullptr) return false;

			tool->SetHeaterTemps(numPadData_.toolHeaterIndex, target, numPadData_.active);
			break;
		case HeaterType::bed:
			bedOrChamber = OM::GetBed(numPadData_.bedOrChamberIndex);
			if (bedOrChamber == nullptr) return false;

			bedOrChamber->SetBedTemp(target, numPadData_.active);
			break;
		case HeaterType::chamber:
			bedOrChamber = OM::GetChamber(numPadData_.bedOrChamberIndex);
			if (bedOrChamber == nullptr) return false;

			bedOrChamber->SetChamberTemp(target, numPadData_.active);
			break;
		}

		return true;
	}

	int8_t GetToolHeaterIndex(const size_t listIndex, OM::Tool*& tool)
	{
		uint8_t count = 0;
		int8_t toolHeaterIndex = -1;
		OM::IterateToolsWhile([&](OM::Tool*& toolIter, size_t) {
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
			//			dbg("List index %d is Tool %d toolHeaterIndex %d=%d", listIndex, tool->index, toolHeaterIndex,
			// tool->GetHeater(toolHeaterIndex)->index);
			return toolHeaterIndex;
		}
		//		dbg("List index %d is not in tool heaters (total tool heaters %d)", listIndex, count);
		tool = nullptr;
		return count; // list index is greater than all heaters for tools
	}

	void Console::Init(ZKListView* console, ZKEditText* input)
	{
		pConsole_ = console;
		pInput_ = input;
	}

	void Console::AddCommand(const std::string& command)
	{
		dbg("AddingCommand %s", command.c_str());
		String<MaxResponseLineLength> line;
		AddLineBreak();
		line.catf("> %s:", command.c_str());
		AddMessage(line.GetRef());
		AddLineBreak();
	}

	void Console::AddResponse(const char* str)
	{
		AddMessage(str);
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
		info("Adding line to Console buffer[%d] = %s", buffer_.GetHead(), line.c_str());
		Refresh();
	}

	void Console::AddMessage(const StringRef& ref)
	{
		AddMessage(ref.c_str());
	}

	void Console::Refresh()
	{
		pConsole_->refreshListView();
		if (!buffer_.Full()) pConsole_->setSelection(buffer_.GetHead() - pConsole_->getRows());
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

	static ZKWindow* s_root = nullptr;

	void Init(ZKWindow* root)
	{
		info("Initialising UI root %p", root);
		s_root = root;
	}

	ZKBase* GetUIControl(int id)
	{
		verbose("Retrieving control with id %d", id);
		ZKBase* control = s_root->findControlByID(id);
		if (control == nullptr)
		{
			error("Control with id %d not found", id);
		}
		return control;
	}

	void SetSelectedFile(const OM::FileSystem::File* file)
	{
		sSelectedFile = file;
	}

	const OM::FileSystem::File* GetSelectedFile()
	{
		return sSelectedFile;
	}

	void RunSelectedFile()
	{
		return OM::FileSystem::RunFile(sSelectedFile);
	}

	static ZKTextView* s_thumbnail;

	void SetThumbnail(ZKTextView* thumbnail)
	{
		s_thumbnail = thumbnail;
	}

	ZKTextView* GetThumbnail()
	{
		return s_thumbnail;
	}
} // namespace UI
