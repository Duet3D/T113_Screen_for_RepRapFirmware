/*
 * Tool.cpp
 *
 *  Created on: 17 Feb 2021
 *      Author: manuel & Andy Everitt
 */

#define DEBUG (1)
#include "uart/CommDef.h"
#include "Tool.hpp"
#include "ListHelpers.hpp"
#include <Duet3D/General/String.h>
#include <Duet3D/General/Vector.hpp>
#include <UI/UserInterfaceConstants.hpp>
#include "Hardware/SerialIo.hpp"
#include "ObjectModel/Utils.hpp"

#include "Debug.hpp"

typedef Vector<OM::Tool*, MaxSlots> ToolList;
static ToolList tools;

namespace OM
{
	void ToolHeater::Reset()
	{
		activeTemp = 0;
		standbyTemp = 0;
	}

	void Tool::operator delete(void * p) noexcept
	{
		Tool* t = static_cast<Tool*>(p);
		for (size_t i = 0; i < MaxHeatersPerTool; ++i)
		{
			delete t->heaters[i];
		}
		FreelistManager::Release<Tool>(p);
	}

	ToolHeater* Tool::GetHeater(const uint8_t toolHeaterIndex)
	{
		if (toolHeaterIndex >= MaxHeatersPerTool)
		{
			return nullptr;
		}
		return heaters[toolHeaterIndex];
	}

	ToolHeater* Tool::GetOrCreateHeater(const uint8_t toolHeaterIndex, const uint8_t heaterIndex)
	{
		ToolHeater *toolHeater = GetHeater(toolHeaterIndex);
		if (toolHeater != nullptr && toolHeater->heater->index == heaterIndex)
		{
			return toolHeater;
		}
		ToolHeater* th = new ToolHeater;
		th->Reset();
		Heat::Heater* heater = Heat::GetOrCreateHeater(heaterIndex);
		th->heater = heater;
		dbg("Setting tool %d heater %d=%d", index, toolHeaterIndex, heaterIndex);
		heaters[toolHeaterIndex] = th;
		return th;
	}

	int32_t Tool::GetHeaterTarget(const uint8_t toolHeaterIndex, const bool active)
	{
		ToolHeater *heater = GetHeater(toolHeaterIndex);
		if (heater == nullptr)
		{
			return -2000;
		}
		return active ? heater->activeTemp : heater->standbyTemp;
	}

	bool Tool::GetHeaterTemps(const StringRef& ref, const bool active)
	{
		for (size_t i = 0; i < MaxHeatersPerTool && heaters[i] != nullptr; ++i)
		{
			if (i > 0)
			{
				ref.cat(':');
			}
			ref.catf("%d", (active ? heaters[i]->activeTemp : heaters[i]->standbyTemp));
		}

		return !ref.IsEmpty();
	}

	bool Tool::SetHeaterTemps(const size_t toolHeaterIndex, const int32_t temp, const bool active)
	{
		String<MaxCommandLength> command;

		for (size_t i = 0; i < MaxHeatersPerTool && heaters[i] != nullptr; ++i)
		{
			if (i > 0)
			{
				command.cat(':');
			}
			if (i == toolHeaterIndex)
			{
				command.catf("%d", temp);
				continue;
			}
			command.catf("%d", (active ? heaters[i]->activeTemp : heaters[i]->standbyTemp));
		}
		if (command.IsEmpty())
			return false;

		SerialIo::Sendf("M568 P%d %s%s", index, active ? "S" : "R", command.c_str());

		return true;
	}

	uint8_t Tool::GetHeaterCount() const
	{
		int8_t count;
		for (count = 0; count < MaxHeatersPerTool && heaters[count] != nullptr; ++count){}
		return count;
	}

	int8_t Tool::HasHeater(const uint8_t heaterIndex) const
	{
		for (size_t i = 0; i < MaxHeatersPerTool && heaters[i] != nullptr; ++i)
		{
			if (heaters[i]->heater->index == (int) heaterIndex)
			{
				return i;
			}
		}
		return -1;
	}

	void Tool::IterateHeaters(function_ref<void(ToolHeater*, size_t)> func, const size_t startAt)
	{
		for (size_t i = startAt; i < MaxHeatersPerTool && heaters[i] != nullptr; ++i)
		{
			func(heaters[i], i);
		}
	}

	size_t Tool::RemoveHeatersFrom(const uint8_t heaterIndex)
	{
		if (heaterIndex >= MaxHeatersPerTool)
		{
			return 0;
		}
		size_t removed = 0;
		for (size_t i = heaterIndex; i < MaxHeatersPerTool && heaters[i] != nullptr; ++i)
		{
			delete heaters[i];
			heaters[i] = nullptr;
			++removed;
		}
		return removed;
	}

	void Tool::UpdateTemp(const uint8_t toolHeaterIndex, const int32_t temp, const bool active)
	{
		ToolHeater* toolHeater = GetHeater(toolHeaterIndex);
		if (toolHeater == nullptr)
		{
			return;
		}
		if (active)
		{
			toolHeater->activeTemp = temp;
		}
		else
		{
			toolHeater->standbyTemp = temp;
		}
	}

	void Tool::ToggleState()
	{
		switch (status)
		{
		case ToolStatus::active:
			SerialIo::Sendf("T-1", index);
			break;
		case ToolStatus::standby:
		case ToolStatus::off:
			SerialIo::Sendf("T%d", index);
			break;
		}
	}

	void Tool::ToggleHeaterState(const uint8_t toolHeaterIndex)
	{
		ToolHeater* toolHeater = GetHeater(toolHeaterIndex);
		if (toolHeater == nullptr)
		{
			return;
		}
		switch (toolHeater->heater->status)
		{
		case Heat::HeaterStatus::active:
			SerialIo::Sendf("M568 P%d A1", index);
			break;
		case Heat::HeaterStatus::fault:
			SerialIo::Sendf("M562 P%d", toolHeater->heater->index);
			break;
		case Heat::HeaterStatus::off:
			SerialIo::Sendf("M568 P%d A2", index);
			break;
		case Heat::HeaterStatus::standby:
			SerialIo::Sendf("M568 P%d A0", index);
			break;
		case Heat::HeaterStatus::offline:
		case Heat::HeaterStatus::tuning:
			break;
		}
	}

	void Tool::Reset()
	{
		index = 0;
		for (size_t i = 0; i < MaxHeatersPerTool; ++i)
		{
			heaters[i] = nullptr;
		}
		extruders.Clear();
		spindle = nullptr;
		spindleRpm = 0;
		fans.Clear();
		for (size_t i = 0; i < MaxTotalAxes; ++i)
		{
			offsets[i] = 0.0f;
		}
		status = ToolStatus::off;
		slot = MaxSlots;
	}

	Tool* GetTool(const size_t index)
	{
		return GetOrCreate<ToolList, Tool>(tools, index, false);
	}

	Tool* GetOrCreateTool(const size_t index)
	{
		dbg("%d", index);
		return GetOrCreate<ToolList, Tool>(tools, index, true);
	}

	bool IterateToolsWhile(function_ref<bool(Tool*&, size_t)> func, const size_t startAt)
	{
		return tools.IterateWhile(func, startAt);
	}

	size_t RemoveTool(const size_t index, const bool allFollowing)
	{
		dbg("Removing tool %d (allFollowing=%s)", index, allFollowing ? "true" : "false");
		return Remove<ToolList, Tool>(tools, index, allFollowing);
	}

	bool UpdateToolHeater(const size_t toolIndex, const size_t toolHeaterIndex, const uint8_t heaterIndex)
	{
		if (toolHeaterIndex >= MaxHeatersPerTool)
		{
			return false;
		}
		OM::Tool *tool = OM::GetOrCreateTool(toolIndex);
		if (tool == nullptr)
		{
			return false;
		}
		ToolHeater *heater = tool->GetOrCreateHeater(toolHeaterIndex, heaterIndex);
		if (heater == nullptr)
		{
			dbg("Failed to get or create tool %d heater %d=%d", toolIndex, toolHeaterIndex, heaterIndex);
			return false;
		}
		dbg("Assigned heater %d to tool %d heaterIndex %d", heaterIndex, toolIndex, toolHeaterIndex);
		tool->heaters[toolHeaterIndex] = heater;
		return true;
	}

	bool RemoveToolHeaters(const size_t toolIndex, const uint8_t firstIndexToDelete)
	{
		OM::Tool *tool = OM::GetTool(toolIndex);
		if (tool == nullptr)
		{
			return false;
		}
		return tool->RemoveHeatersFrom(firstIndexToDelete) > 0;
	}

	bool UpdateToolTemp(const size_t toolIndex, const size_t toolHeaterIndex, const int32_t temp, const bool active)
	{
		OM::Tool *tool = OM::GetOrCreateTool(toolIndex);

		// If we do not handle this tool back off
		if (tool == nullptr)
		{
			return false;
		}

		tool->UpdateTemp(toolHeaterIndex, temp, active);
		return true;
	}

	bool UpdateToolName(const size_t toolIndex, const char *name)
	{
		OM::Tool *tool = OM::GetOrCreateTool(toolIndex);

		// If we do not handle this tool back off
		if (tool == nullptr)
		{
			return false;
		}

		if (strcmp(name, "") == 0)
		{
			tool->name.printf("Tool %d", tool->index);
		}
		else
		{
			tool->name.copy(name, TOOL_NAME_MAX_LEN);
		}
		dbg("Tool %d name=%s", tool->index, tool->name.c_str());
		return true;
	}

	bool UpdateToolStatus(const size_t toolIndex, const char *statusStr)
	{
		OM::Tool *tool = OM::GetOrCreateTool(toolIndex);

		// If we do not handle this tool back off
		if (tool == nullptr)
		{
			return false;
		}

		const ToolStatusMapEntry key = {statusStr, ToolStatus::off};
		const ToolStatusMapEntry *statusFromMap =
			(ToolStatusMapEntry *)bsearch(
				&key,
				toolStatusMap,
				ARRAY_SIZE(toolStatusMap),
				sizeof(ToolStatusMapEntry),
				compareKey<ToolStatusMapEntry>);
		ToolStatus status = (statusFromMap != nullptr) ? statusFromMap->val : ToolStatus::off;
		tool->status = status;
		return true;
	}
}
