/*
 * UserInterface.cpp
 *
 *  Created on: 18 Dec 2023
 *      Author: andy
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "UserInterface.h"

#include "Hardware/Duet.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Tool.h"
#include "utils/utils.h"
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

namespace UI
{
	Window& Window::GetInstance()
	{
		static Window window;
		return window;
	}

	void Window::AddHome(ZKBase* home)
	{
		m_homeWindows.push_back(home);
	}

	void Window::ClearHome()
	{
		m_homeWindows.clear();
	}

	void Window::OpenWindow(const int windowId)
	{
		ZKBase* window = UI::GetUIControl<ZKBase>(windowId);
		if (window == nullptr)
		{
			warn("Window %d is null", windowId);
			return;
		}
		OpenWindow(window);
	}

	void Window::OpenWindow(ZKBase* window)
	{
		if (window == nullptr)
		{
			warn("Window is null");
			return;
		}
		CloseOverlay();
		CloseLastWindow();
		info("Opening window %d", window->getID());
		window->setVisible(true);
		RemoveFromVector<ZKBase*>(m_closedWindows, window);
		if (!InVector<ZKBase*>(m_homeWindows, window))
			AddToVector<ZKBase*>(m_openedWindows, window);
	}

	void Window::OpenOverlay(const int overlayId, bool closeAlreadyOpened)
	{
		ZKBase* overlay = UI::GetUIControl<ZKBase>(overlayId);
		if (overlay == nullptr)
		{
			warn("Overlay %d is null", overlayId);
			return;
		}
		OpenOverlay(overlay, closeAlreadyOpened);
	}

	void Window::OpenOverlay(ZKBase* overlay, bool closeAlreadyOpened)
	{
		if (overlay == nullptr)
		{
			warn("Overlay is null");
			return;
		}
		if (!m_overlayWindows.empty() && closeAlreadyOpened)
		{
			CloseOverlay();
		}
		dbg("Opening overlay %d", overlay->getID());
		overlay->setVisible(true);
		AddToVector<ZKBase*>(m_overlayWindows, overlay);
	}

	bool Window::IsOverlayOpened()
	{
		for (auto& window : m_overlayWindows)
		{
			if (window->isVisible())
			{
				return true;
			}
		}
		return false;
	}

	bool Window::CloseOverlay()
	{
		if (m_overlayWindows.empty())
			return false;

		verbose("%d overlay windows to close", m_overlayWindows.size());
		// Ensure that you can't close the popup window if it's blocking.
		if (POPUP_WINDOW.IsBlocking())
		{
			verbose("Popup window is blocking");
			return false;
		}
		if (POPUP_WINDOW.IsOpen())
		{
			POPUP_WINDOW.Clear();
		}
		UI::GetUIControl<ZKButton>(ID_MAIN_OverlayModalZone)->setVisible(false);
		size_t count = 0;
		for (auto& window : m_overlayWindows)
		{
			if (!window->isVisible())
				continue;
			window->setVisible(false);
			RemoveFromVector<ZKBase*>(m_overlayWindows, window);
			count++;
		}
		verbose("Closed %d overlays, %d remaining", count, m_overlayWindows.size());
		return count > 0;
	}

	size_t Window::ReOpenLastWindow(size_t numWindows)
	{
		size_t count = 0;
		for (size_t i = 0; i < numWindows; ++i)
		{
			if (m_closedWindows.empty())
				return count;
			auto window = m_closedWindows.back();
			OpenWindow(window);
		}
		return count;
	}

	void Window::CloseLastWindow()
	{
		for (auto window : m_homeWindows)
		{
			if (!window->isVisible())
				continue;
			CloseWindow(window);
		}
		if (m_openedWindows.empty())
			return;
		CloseWindow(m_openedWindows.back());
	}

	void Window::CloseWindow(ZKBase* window, const bool returnable)
	{
		info("Closing window %d", window->getID());
		window->setVisible(false);
		RemoveFromVector<ZKBase*>(m_openedWindows, window);
		if (returnable)
			AddToVector<ZKBase*>(m_closedWindows, window);
	}

	void Window::Back()
	{
		info("Back button pressed");
		if (POPUP_WINDOW.IsOpen())
		{
			POPUP_WINDOW.Cancel(false);
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
		if (!m_openedWindows.empty())
		{
			ZKBase* lastOpened = m_openedWindows.back();
			CloseWindow(lastOpened, false);
		}
		if (!m_closedWindows.empty())
		{
			ZKBase* lastClosed = m_closedWindows.back();
			OpenWindow(lastClosed);
		}
	}

	void Window::Home()
	{
		info("Returning to home");
		for (auto window : m_openedWindows)
		{
			verbose("Hiding window %d", window->getID());
			window->setVisible(false);
		}
		for (auto window : m_homeWindows)
		{
			verbose("Showing window %d", window->getID());
			window->setVisible(true);
		}
		CloseOverlay();
		Clear();
	}

	void Window::Clear()
	{
		m_openedWindows.clear();
		m_closedWindows.clear();
	}

	static std::vector<ToolsList*> s_toolsLists;

	ToolsList::ToolsList() : m_toolCount(0), m_bedCount(0), m_chamberCount(0)
	{
		s_toolsLists.push_back(this);
	}

	void ToolsList::Init(ZKListView* toolListView)
	{
		m_pToolListView = toolListView;
	}

	void ToolsList::CalculateTotalHeaterCount()
	{
		verbose("Bed count %d", OM::GetBedCount());
		m_bedCount = OM::GetBedCount();

		verbose("Chamber count %d", OM::GetChamberCount());
		m_chamberCount = OM::GetChamberCount();

		size_t count = 0;
		OM::IterateToolsWhile([&count](OM::Tool*& tool, size_t) {
			const bool hasHeater = tool->heaters[0] != nullptr;
			const bool hasSpindle = tool->spindle != nullptr;
			// Heater takes precedence (like in DWC)
			if (hasHeater)
			{
				count += tool->GetHeaterCount();
			}
			if (hasSpindle || !hasHeater)
			{
				// For a tool that has no heaters or spindle, we want to show it so that the user can load/unload
				// filament
				++count;
			}
			return true;
		});

		verbose("Tool count %d", count);
		m_toolCount = count;
	}

	size_t ToolsList::GetTotalHeaterCount(const bool calculate, const bool addTools, const bool addBeds,
										  const bool addChambers)
	{
		if (calculate) CalculateTotalHeaterCount();

		size_t count = 0;
		if (addTools)
		{
			count += m_toolCount;
		}
		if (addBeds)
		{
			count += m_bedCount;
		}
		if (addChambers)
		{
			count += m_chamberCount;
		}
		return count;
	}

	void ToolsList::ObtainListItemData(ZKListView::ZKListItem* pListItem,
									   int index,
									   const int nameId,
									   const int statusId,
									   const int currentId,
									   const int activeId,
									   const int standbyId)
	{
		ZKListView::ZKListSubItem* pToolName = pListItem->findSubItemByID(nameId);
		ZKListView::ZKListSubItem* pStatus = pListItem->findSubItemByID(statusId);
		ZKListView::ZKListSubItem* pCurrentTemperature = pListItem->findSubItemByID(currentId);
		ZKListView::ZKListSubItem* pActiveTemperature = pListItem->findSubItemByID(activeId);
		ZKListView::ZKListSubItem* pStandbyTemperature = pListItem->findSubItemByID(standbyId);

		if (!pToolName || !pStatus || !pCurrentTemperature || !pActiveTemperature || !pStandbyTemperature)
		{
			warn("One or more subitems are null");
			return;
		}

		// Check Tools to see if list index is within tool heaters
		ToolListItemData data = GetToolListItemDataBySlot(index);
		if (data.tool != nullptr)
		{
			pListItem->setSelected(data.tool->status == OM::ToolStatus::active);
			pCurrentTemperature->setVisible(data.toolHeater != nullptr || data.spindle != nullptr);
			pActiveTemperature->setVisible(data.toolHeater != nullptr || data.spindle != nullptr);
			pStandbyTemperature->setVisible(data.toolHeater != nullptr || data.spindle != nullptr);
			if (data.toolHeater != nullptr)
			{
				// Heater
				if (data.tool->GetHeaterCount() > 1)
				{
					pToolName->setTextf("%s (%d)", data.tool->name.c_str(), data.toolHeaterIndex);
				}
				else
				{
					pToolName->setText(data.tool->name.c_str());
				}
				pActiveTemperature->setText((int)data.toolHeater->activeTemp);
				pStandbyTemperature->setText((int)data.toolHeater->standbyTemp);
				pCurrentTemperature->setText(data.toolHeater->heater->current);
				pStatus->setTextTr(data.toolHeater->heater->GetHeaterStatusStr());
				return;
			}
			else if (data.spindle != nullptr)
			{
				// Spindle
				pToolName->setTextf("%s (spindle)", data.tool->name.c_str());
				pActiveTemperature->setText((int)data.tool->spindleRpm);
				pStandbyTemperature->setText("RPM");
				pCurrentTemperature->setText((int)data.spindle->current);
				pStatus->setTextTr(data.spindle->GetStateStr());
				return;
			}
			else
			{
				// Tool with no heaters or spindle
				pToolName->setText(data.tool->name.c_str());
				pActiveTemperature->setVisible(false);
				pStandbyTemperature->setVisible(false);
				pCurrentTemperature->setVisible(false);
				return;
			}
			return;
		}

		int8_t bedOrChamberIndex = index - m_toolCount;
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
			pStatus->setTextTr(heater->GetHeaterStatusStr());
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
			pStatus->setTextTr(heater->GetHeaterStatusStr());
			pListItem->setSelected(false);
			return;
		}
		warn("Unknown index");
	}

	void ToolsList::OnListItemClick(int index, int id, const int nameId, int statusId, int activeId, int standbyId)
	{
		dbg("index=%d, id=%d", index, id);
		if (id == NULL) return;
		NumPadData numPadData;

		CalculateTotalHeaterCount();
		if ((size_t)index < GetTotalHeaterCount(false, true, false, false))
		{
			ToolListItemData data = GetToolListItemDataBySlot(index);
			if (data.tool == nullptr)
				return;
			dbg("Tool index=%d", data.tool->index);
			numPadData.heaterType = UI::HeaterType::tool;
			numPadData.toolData = data;
			if (id == nameId)
			{
				data.tool->ToggleState();
				return;
			}
			if (id == statusId)
			{
				if (data.toolHeater != nullptr)
				{
					data.tool->ToggleHeaterState(data.toolHeaterIndex);
				}
				else if (data.spindle != nullptr)
				{
					data.tool->ToggleSpindleState();
				}
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
		if (lengthChanged)
		{
			CalculateTotalHeaterCount();
		}

		if (m_pToolListView == nullptr)
		{
			warn("Tool list view is null");
			return;
		}
		m_pToolListView->refreshListView();
	}

	void ToolsList::RefreshAllToolLists(const bool lengthChanged)
	{
		for (auto list : s_toolsLists)
		{
			list->RefreshToolList(lengthChanged);
		}
	}

	void ToolsList::OpenNumPad(const NumPadData data)
	{
		dbg("");
		std::string header;
		m_numPadData.heaterType = data.heaterType;
		m_numPadData.active = data.active;
		int currentTarget = 0;

		switch (data.heaterType)
		{
		case HeaterType::tool: {
			if (data.toolData.tool == nullptr)
			{
				warn("Tool is null");
				return;
			}

			m_numPadData.toolData = data.toolData;

			if (data.toolData.toolHeater != nullptr)
			{
				header = utils::format("Tool %d Heater %d %s",
									   data.toolData.tool->index,
									   data.toolData.toolHeaterIndex,
									   data.active ? "Active" : "Standby");

				OM::ToolHeater* toolHeater = data.toolData.toolHeater;
				if (toolHeater == nullptr)
				{
					warn("Tool heater is null");
					return;
				}

				if (toolHeater->heater == nullptr)
				{
					warn("Heater is null");
					return;
				}

				currentTarget = data.active ? toolHeater->activeTemp : toolHeater->standbyTemp;
				UI::NUMPAD_WINDOW.Open(
					header.c_str(),
					toolHeater->heater->min,
					toolHeater->heater->max,
					currentTarget,
					[](int) {},
					[this](int value) { SendTempTarget(value); });
				return;
			}

			if (data.toolData.spindle != nullptr)
			{
				OM::Spindle* spindle = data.toolData.spindle;
				header = utils::format("Tool %d Spindle", data.toolData.tool->index);
				currentTarget = spindle->active;
				UI::NUMPAD_WINDOW.Open(
					header.c_str(),
					spindle->min,
					spindle->max,
					currentTarget,
					[](int) {},
					[this](int value) { SendSpindleTarget(value); });
				return;
			}
			break;
		}
		case HeaterType::bed:
		case HeaterType::chamber: {
			header = utils::format("%s %d %s",
								   (data.heaterType == HeaterType::bed) ? "Bed" : "Chamber",
								   data.bedOrChamberIndex,
								   data.active ? "Active" : "Standby");
			m_numPadData.bedOrChamberIndex = data.bedOrChamberIndex;
			OM::BedOrChamber* bedOrChamber = (data.heaterType == HeaterType::bed)
												 ? OM::GetBed(data.bedOrChamberIndex)
												 : OM::GetChamber(data.bedOrChamberIndex);
			if (bedOrChamber == nullptr)
			{
				warn("Bed or chamber is null, type=%d, index=%u", (int)data.heaterType, data.bedOrChamberIndex);
				return;
			}
			currentTarget = data.active ? bedOrChamber->GetActiveTemp() : bedOrChamber->GetStandbyTemp();
			UI::NUMPAD_WINDOW.Open(
				header.c_str(),
				bedOrChamber->GetMin(),
				bedOrChamber->GetMax(),
				currentTarget,
				[](int) {},
				[this](int value) { SendTempTarget(value); });
			return;
		}
		}
		warn("Unknown heater type");
	}

	bool ToolsList::SendTempTarget(int target)
	{
		OM::Tool* tool = nullptr;
		OM::BedOrChamber* bedOrChamber = nullptr;
		dbg("heaterType=%d", (int)m_numPadData.heaterType);
		switch (m_numPadData.heaterType)
		{
		case HeaterType::tool:
			tool = m_numPadData.toolData.tool;
			if (tool == nullptr) return false;

			tool->SetHeaterTemps(m_numPadData.toolData.toolHeaterIndex, target, m_numPadData.active);
			break;
		case HeaterType::bed:
			bedOrChamber = OM::GetBed(m_numPadData.bedOrChamberIndex);
			if (bedOrChamber == nullptr) return false;

			bedOrChamber->SetBedTemp(target, m_numPadData.active);
			break;
		case HeaterType::chamber:
			bedOrChamber = OM::GetChamber(m_numPadData.bedOrChamberIndex);
			if (bedOrChamber == nullptr) return false;

			bedOrChamber->SetChamberTemp(target, m_numPadData.active);
			break;
		}

		return true;
	}

	bool ToolsList::SendSpindleTarget(int target)
	{
		OM::Tool* tool = m_numPadData.toolData.tool;
		if (tool == nullptr)
			return false;

		tool->UpdateSpindleTarget(target);
		return true;
	}

	ToolsList::ToolListItemData ToolsList::GetToolListItemDataBySlot(const size_t listIndex)
	{
		ToolListItemData data;
		uint8_t count = 0;
		OM::IterateToolsWhile([&](OM::Tool*& toolIter, size_t) {
			bool hasHeater = toolIter->GetHeaterCount() > 0;
			bool hasSpindle = toolIter->spindle != nullptr;
			data.tool = toolIter;
			if (listIndex < count + toolIter->GetHeaterCount())
			{
				data.toolHeaterIndex = listIndex - count;
				data.toolHeater = toolIter->GetHeater(data.toolHeaterIndex);
				return false;
			}
			count += toolIter->GetHeaterCount();

			if (listIndex < count + (hasSpindle ? 1u : 0u))
			{
				data.spindle = toolIter->spindle;
				return false;
			}
			count += (hasSpindle ? 1 : 0);

			count += (!hasHeater && !hasSpindle) ? 1 : 0;
			if (listIndex < count)
			{
				return false;
			}
			data.tool = nullptr;
			return true;
		});
		return data;
	}

	void Console::Init(ZKListView* console, ZKEditText* input)
	{
		m_console = console;
		m_input = input;
	}

	void Console::AddCommand(const std::string& command)
	{
		dbg("AddingCommand %s", command.c_str());
		String<MAX_RESPONSE_LINE_LENGTH> line;
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
		String<MAX_RESPONSE_LINE_LENGTH> line;
		line.copy(str);
		m_buffer.Push(line);
		info("Adding line to Console buffer[%d] = %s", m_buffer.GetHead(), line.c_str());
		Refresh();
	}

	void Console::AddMessage(const StringRef& ref)
	{
		AddMessage(ref.c_str());
	}

	void Console::Refresh()
	{
		m_console->refreshListView();
		if (!m_buffer.Full())
			m_console->setSelection(m_buffer.GetHead() - m_console->getRows());
	}

	void Console::Clear()
	{
		String<MAX_RESPONSE_LINE_LENGTH> line;
		line.copy("");
		for (size_t i = 0; i < MAX_RESPONSE_LINES; i++)
		{
			m_buffer.Push(line);
		}
		m_buffer.Reset();
		Refresh();
	}

	static ZKWindow* s_root = nullptr;

	void Init(ZKWindow* root)
	{
		info("Initialising UI root %p", root);
		s_root = root;
	}

	ZKWindow* GetRootWindow()
	{
		return s_root;
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

	void SetIconRelativePosition(ZKButton* control, float xPos, float yPos, float scale)
	{
		if (control == nullptr)
		{
			warn("Control is null");
			return;
		}
		const LayoutPosition& pos = control->getPosition();

		xPos = utils::bound(xPos, 0.0f, 1.0f);
		yPos = utils::bound(yPos, 0.0f, 1.0f);
		scale = utils::bound(scale, 0.0f, 1.0f);
		int iconSize = (int)(std::min(pos.mWidth, pos.mHeight) * scale);

		LayoutPosition iconPos;
		iconPos.mLeft = pos.mWidth * xPos - iconSize / 2;
		iconPos.mTop = pos.mHeight * yPos - iconSize / 2;
		iconPos.mWidth = iconSize;
		iconPos.mHeight = iconSize;

		control->setIconPosition(iconPos);
	}

	void SetIconRelativePosition(ZKButton* control, float xPos, float yPos, float xScale, float yScale)
	{
		if (control == nullptr)
		{
			warn("Control is null");
			return;
		}
		const LayoutPosition& pos = control->getPosition();

		xPos = utils::bound(xPos, 0.0f, 1.0f);
		yPos = utils::bound(yPos, 0.0f, 1.0f);
		xScale = utils::bound(xScale, 0.0f, 1.0f);
		yScale = utils::bound(yScale, 0.0f, 1.0f);

		LayoutPosition iconPos;
		iconPos.mLeft = pos.mWidth * xPos - (pos.mWidth * xScale) / 2;
		iconPos.mTop = pos.mHeight * yPos - (pos.mHeight * yScale) / 2;
		iconPos.mWidth = (pos.mWidth * xScale);
		iconPos.mHeight = (pos.mHeight * yScale);

		control->setIconPosition(iconPos);
	}

	void SetM291Popup(OM::Alert alert)
	{
		info("Setting M291 popup, mode=%d", (int)alert.mode);
		OM::Alert::Mode mode_ = alert.mode;
		uint32_t seq_ = alert.seq;

		UI::POPUP_WINDOW.Open([mode_, seq_]() {
			switch (mode_)
			{
			case OM::Alert::Mode::InfoConfirm:
			case OM::Alert::Mode::ConfirmCancel:
				Comm::DUET.SendGcodef("M292 P0 S%lu\n", seq_);
				break;
			case OM::Alert::Mode::NumberInt:
			case OM::Alert::Mode::NumberFloat:
				Comm::DUET.SendGcodef("M292 P0 R{%s} S%lu\n", UI::POPUP_WINDOW.GetNumberInput(), seq_);
				break;
			case OM::Alert::Mode::Text:
				Comm::DUET.SendGcodef("M292 P0 R{\"%s\"} S%lu\n", UI::POPUP_WINDOW.GetTextInput(), seq_);
				break;
			default:
				break;
			}
		});

		UI::POPUP_WINDOW.SetMode(mode_);

		UI::POPUP_WINDOW.SetTitle(alert.title.c_str());
		UI::POPUP_WINDOW.SetText(alert.text.c_str());

		switch (mode_)
		{
		case OM::Alert::Mode::Info:
			UI::POPUP_WINDOW.CancelVisible(false);
			UI::POPUP_WINDOW.OkVisible(false);
			break;
		case OM::Alert::Mode::InfoClose:
			UI::POPUP_WINDOW.CancelVisible(true);
			UI::POPUP_WINDOW.OkVisible(false);
			break;
		case OM::Alert::Mode::InfoConfirm:
		case OM::Alert::Mode::ConfirmCancel:
			UI::POPUP_WINDOW.CancelVisible(mode_ == OM::Alert::Mode::ConfirmCancel);
			UI::POPUP_WINDOW.OkVisible(true);

			// Axis jog controls
			UI::POPUP_WINDOW.SetAxisJogSelection(alert.controls);
			UI::POPUP_WINDOW.AxisJogVisible(alert.controls > 0);
			break;
		case OM::Alert::Mode::Choices:
			UI::POPUP_WINDOW.CancelVisible(alert.cancelButton == true);
			UI::POPUP_WINDOW.OkVisible(false);
			UI::POPUP_WINDOW.SelectionVisible(true);
			break;
		case OM::Alert::Mode::NumberInt:
			UI::POPUP_WINDOW.CancelVisible(alert.cancelButton == true);
			UI::POPUP_WINDOW.OkVisible(true);
			UI::POPUP_WINDOW.SetMinTextf("Min: %d", alert.limits.numberInt.min);
			UI::POPUP_WINDOW.SetMaxTextf("Max: %d", alert.limits.numberInt.max);
			UI::POPUP_WINDOW.SetNumberInput(alert.limits.numberInt.valueDefault);
			UI::POPUP_WINDOW.MinTextVisible(true);
			UI::POPUP_WINDOW.MaxTextVisible(true);
			UI::POPUP_WINDOW.NumberInputVisible(true);
			break;
		case OM::Alert::Mode::NumberFloat:
			UI::POPUP_WINDOW.CancelVisible(alert.cancelButton == true);
			UI::POPUP_WINDOW.OkVisible(true);
			UI::POPUP_WINDOW.SetMinTextf("Min: %.2f", alert.limits.numberFloat.min);
			UI::POPUP_WINDOW.SetMaxTextf("Max: %.2f", alert.limits.numberFloat.max);
			UI::POPUP_WINDOW.SetNumberInput(alert.limits.numberFloat.valueDefault);
			UI::POPUP_WINDOW.MinTextVisible(true);
			UI::POPUP_WINDOW.MaxTextVisible(true);
			UI::POPUP_WINDOW.NumberInputVisible(true);
			break;
		case OM::Alert::Mode::Text:
			UI::POPUP_WINDOW.CancelVisible(alert.cancelButton == true);
			UI::POPUP_WINDOW.OkVisible(true);
			UI::POPUP_WINDOW.SetMinTextf("Min Length: %d", alert.limits.text.min);
			UI::POPUP_WINDOW.SetMaxTextf("Max Length: %d", alert.limits.text.max);
			UI::POPUP_WINDOW.SetTextInput(alert.limits.text.valueDefault.c_str());
			UI::POPUP_WINDOW.MinTextVisible(true);
			UI::POPUP_WINDOW.MaxTextVisible(true);
			UI::POPUP_WINDOW.TextInputVisible(true);
			break;
		default:
			break;
		}
	}
} // namespace UI
