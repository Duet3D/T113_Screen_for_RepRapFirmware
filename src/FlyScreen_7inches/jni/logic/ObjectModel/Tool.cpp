/*
 * Tool.cpp
 *
 *  Created on: 17 Feb 2021
 *      Author: manuel & Andy Everitt
 */

#define DEBUG (0)
#include "Tool.h"
#include "Hardware/SerialIo.h"
#include "ListHelpers.h"
#include "ObjectModel/Utils.h"
#include "uart/CommDef.h"
#include <Duet3D/General/String.h>
#include <Duet3D/General/Vector.h>
#include <UI/UserInterfaceConstants.h>

#include "Debug.h"

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

	Move::ExtruderAxis* Tool::GetExtruder(const uint8_t toolExtruderIndex)
	{
		if (toolExtruderIndex >= MaxExtrudersPerTool)
		{
			return nullptr;
		}
		return extruders[toolExtruderIndex];
	}

	Move::ExtruderAxis* Tool::GetOrCreateExtruder(const uint8_t toolExtruderIndex, const uint8_t extruderIndex)
	{
		Move::ExtruderAxis* extruder = GetExtruder(toolExtruderIndex);
		if (extruder != nullptr && extruder->index == extruderIndex)
		{
			return extruder;
		}
		extruder = Move::GetOrCreateExtruderAxis(extruderIndex);
		dbg("Setting tool %d extruder %d=%d", index, toolExtruderIndex, extruderIndex);
		extruders[toolExtruderIndex] = extruder;
		return extruder;
	}

	Fan* Tool::GetFan(const uint8_t toolFanIndex)
	{
		if (toolFanIndex >= MaxFans)
		{
			return nullptr;
		}
		return fans[toolFanIndex];
	}

	Fan* Tool::GetOrCreateFan(const uint8_t toolFanIndex, const uint8_t fanIndex)
	{
		Fan *fan = GetFan(toolFanIndex);
		if (fan != nullptr && fan->index == fanIndex)
		{
			return fan;
		}
		fan = OM::GetOrCreateFan(fanIndex);
		dbg("Setting tool %d fan %d=%d", index, toolFanIndex, fanIndex);
		fans[toolFanIndex] = fan;
		return fan;
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

	size_t Tool::RemoveExtrudersFrom(const uint8_t extruderIndex)
	{
		if (extruderIndex >= MaxExtrudersPerTool)
		{
			return 0;
		}
		size_t removed = 0;
		for (size_t i = extruderIndex; i < MaxExtrudersPerTool && extruders[i] != nullptr; ++i)
		{
			extruders[i] = nullptr;
			++removed;
		}
		return removed;
	}

	size_t Tool::RemoveFansFrom(const uint8_t fanIndex)
	{
		if (fanIndex >= MaxFans)
		{
			return 0;
		}
		size_t removed = 0;
		for (size_t i = fanIndex; i < MaxFans && fans[i] != nullptr; ++i)
		{
			fans[i] = nullptr;
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
		for (size_t i = 0; i < MaxExtrudersPerTool; ++i)
		{
			extruders[i] = nullptr;
		}
		for (size_t i = 0; i < MaxFans; ++i)
		{
			fans[i] = nullptr;
		}
		spindle = nullptr;
		spindleRpm = 0;
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

	const size_t GetToolCount()
	{
		return tools.Size();
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

	bool UpdateToolExtruder(const size_t toolIndex, const size_t toolExtruderIndex, const uint8_t extruderIndex)
	{
		if (toolExtruderIndex >= MaxExtrudersPerTool)
		{
			return false;
		}
		OM::Tool *tool = OM::GetOrCreateTool(toolIndex);
		if (tool == nullptr)
		{
			return false;
		}
		Move::ExtruderAxis *extruder = tool->GetOrCreateExtruder(toolExtruderIndex, extruderIndex);
		if (extruder == nullptr)
		{
			dbg("Failed to get or create tool %d extruder %d=%d", toolIndex, toolExtruderIndex, extruderIndex);
			return false;
		}
		dbg("Assigned extruder %d to tool %d extruderIndex %d", extruderIndex, toolIndex, toolExtruderIndex);
		tool->extruders[toolExtruderIndex] = extruder;
		return true;
	}

	bool RemoveToolExtruders(const size_t toolIndex, const uint8_t firstIndexToDelete)
	{
		OM::Tool *tool = OM::GetTool(toolIndex);
		if (tool == nullptr)
		{
			return false;
		}
		return tool->RemoveExtrudersFrom(firstIndexToDelete) > 0;
	}

	bool UpdateToolFan(const size_t toolIndex, const size_t toolFanIndex, const uint8_t fanIndex)
	{
		if (toolFanIndex >= MaxFans)
		{
			return false;
		}
		OM::Tool *tool = OM::GetOrCreateTool(toolIndex);
		if (tool == nullptr)
		{
			return false;
		}
		Fan *fan = tool->GetOrCreateFan(toolFanIndex, fanIndex);
		if (fan == nullptr)
		{
			dbg("Failed to get or create tool %d fan %d=%d", toolIndex, toolFanIndex, fanIndex);
			return false;
		}
		dbg("Assigned fan %d to tool %d fanIndex %d", fanIndex, toolIndex, toolFanIndex);
		tool->fans[toolFanIndex] = fan;
		return true;
	}

	bool RemoveToolFans(const size_t toolIndex, const uint8_t firstIndexToDelete)
	{
		OM::Tool *tool = OM::GetTool(toolIndex);
		if (tool == nullptr)
		{
			return false;
		}
		return tool->RemoveFansFrom(firstIndexToDelete) > 0;
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
