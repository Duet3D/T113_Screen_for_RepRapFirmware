/*
 * Tool.h
 *
 *  Created on: 17 Feb 2021
 *      Author: manuel
 */

#ifndef SRC_OBJECTMODEL_TOOL_HPP_
#define SRC_OBJECTMODEL_TOOL_HPP_

//#include <cstdint>
#include <sys/types.h>
#include "Axis.h"
#include "Fan.h"
#include "Spindle.h"
#include "Heat.h"
#include <Duet3D/General/FreelistManager.h>
#include <Duet3D/General/String.h>
#include <Duet3D/General/StringRef.h>
#include <Duet3D/General/function_ref.h>
#include <UI/UserInterfaceConstants.h>

namespace OM
{
	// Status that a tool may report to us.
	enum class ToolStatus
	{
		off = 0,
		active = 1,
		standby = 2,
	};

	struct ToolStatusMapEntry
	{
		const char* key;
		ToolStatus val;
	};

	// This table must be kept in case-insensitive alphabetical order of the search string.
	const ToolStatusMapEntry toolStatusMap[] =
	{
		{"active",	ToolStatus::active },
		{"off",		ToolStatus::off },
		{"standby",	ToolStatus::standby },
	};

	struct ToolHeater
	{
		void* operator new(size_t) noexcept { return FreelistManager::Allocate<ToolHeater>(); }
		void operator delete(void* p) noexcept { FreelistManager::Release<ToolHeater>(p); }

		int32_t activeTemp;
		int32_t standbyTemp;
		Heat::Heater* heater;

		void Reset();
	};

	struct Tool
	{
		void* operator new(size_t) noexcept { return FreelistManager::Allocate<Tool>(); }
		void operator delete(void* p) noexcept;

		// tool number
		uint8_t index;
		String<MaxToolNameLength> name;
		ToolHeater* heaters[MaxHeatersPerTool];
		Move::ExtruderAxis* extruders[MaxExtrudersPerTool];
		float mix[MaxExtrudersPerTool];
		Fan* fans[MaxFans];
		Spindle* spindle;
		int32_t spindleRpm;
		float offsets[MaxTotalAxes];
		ToolStatus status;
		int8_t filamentExtruder;

		ToolHeater* GetHeater(const uint8_t toolHeaterIndex);
		ToolHeater* GetOrCreateHeater(const uint8_t toolHeaterIndex, const uint8_t heaterIndex);

		Move::ExtruderAxis* GetExtruder(const uint8_t toolExtruderIndex) const;
		Move::ExtruderAxis* GetOrCreateExtruder(const uint8_t toolExtruderIndex, const uint8_t extruderIndex);

		Fan* GetFan(const uint8_t toolFanIndex);
		Fan* GetOrCreateFan(const uint8_t toolFanIndex, const uint8_t fanIndex);

		StringRef GetFilament() const;

		int32_t GetHeaterTarget(const uint8_t toolHeaterIndex, const bool active);
		bool GetHeaterTemps(const StringRef& ref, const bool active);
		bool SetHeaterTemps(const size_t toolHeaterIndex, const int32_t temp, const bool active);
		uint8_t GetHeaterCount() const;
		int8_t HasHeater(const uint8_t heaterIndex) const;
		void IterateHeaters(function_ref<void(ToolHeater*, size_t)> func, const size_t startAt = 0);
		void IterateExtruders(function_ref<void(Move::ExtruderAxis*, size_t)> func, const size_t startAt = 0);
		void IterateFans(function_ref<void(Fan*, size_t)> func, const size_t startAt = 0);
		size_t RemoveHeatersFrom(const uint8_t toolHeaterIndex);
		size_t RemoveExtrudersFrom(const uint8_t toolExtruderIndex);
		size_t RemoveFansFrom(const uint8_t toolFanIndex);
		void UpdateTemp(const uint8_t toolHeaterIndex, const int32_t temp, const bool active);
		void ToggleState();
		void ToggleHeaterState(const uint8_t toolHeaterIndex);

		void Reset();
	};

	Tool* GetTool(const size_t index);
	Tool* GetOrCreateTool(const size_t index);
	const size_t GetToolCount();
	bool IterateToolsWhile(function_ref<bool(Tool*&, size_t)> func, const size_t startAt = 0);
	size_t RemoveTool(const size_t index, const bool allFollowing);

	bool UpdateToolHeater(const size_t toolIndex, const size_t toolHeaterIndex, const uint8_t heaterIndex);
	bool RemoveToolHeaters(const size_t toolIndex, const uint8_t firstIndexToDelete = 0);

	bool UpdateToolExtruder(const size_t toolIndex, const size_t toolExtruderIndex, const uint8_t extruderIndex);
	bool RemoveToolExtruders(const size_t toolIndex, const uint8_t firstIndexToDelete = 0);
	bool UpdateToolMix(const size_t toolIndex, const size_t toolExtruderIndex, const float mix);

	bool UpdateToolFan(const size_t toolIndex, const size_t toolFanIndex, const uint8_t fanIndex);
	bool RemoveToolFans(const size_t toolIndex, const uint8_t firstIndexToDelete = 0);

	bool UpdateToolFilamentExtruder(const size_t toolIndex, const int8_t extruderIndex);

	bool UpdateToolTemp(const size_t toolIndex, const size_t toolHeaterIndex, const int32_t temp, const bool active);
	bool UpdateToolName(const size_t toolIndex, const char *name);
	bool UpdateToolStatus(const size_t toolIndex, const char *statusStr);

	bool UpdateToolSpindle(const size_t toolIndex, const int8_t spindleIndex);

	void SetCurrentTool(const size_t toolIndex);
	Tool* GetCurrentTool();
}

#endif /* SRC_OBJECTMODEL_TOOL_HPP_ */
