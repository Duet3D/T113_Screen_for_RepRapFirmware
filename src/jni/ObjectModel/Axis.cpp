/*
 * Axis.cpp
 *
 *  Created on: 17 Feb 2021
 *      Author: manuel
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_WARN
#include "Axis.h"

#include "Configuration.h"
#include "ObjectModel/Tool.h"
#include <Duet3D/General/Vector.h>
#include <math.h>

#include "Debug.h"
#include "ListHelpers.h"

typedef Vector<OM::Move::Axis*, MAX_TOTAL_AXES> AxisList;
typedef Vector<OM::Move::ExtruderAxis*, MAX_TOTAL_AXES> ExtruderAxisList;
static AxisList s_axes;
static ExtruderAxisList s_extruderAxes;
static uint8_t s_currentWorkplaceNumber = OM::Move::Workplaces::MaxTotalWorkplaces;

namespace OM::Move
{
	static float sExtrusionRate = 0.0f;

	void Axis::Reset()
	{
		index = 0;
		babystep = 0.0f;
		letter[0] = 0;
		letter[1] = 0;
		for (size_t i = 0; i < Workplaces::MaxTotalWorkplaces; ++i)
		{
			workplaceOffsets[i] = 0.0f;
		}
		homed = false;
		visible = false;
	}

	Axis* GetAxis(const size_t index)
	{
		dbg("Axis index %d / max %d\n", index, MAX_TOTAL_AXES);
		if (index >= MAX_TOTAL_AXES)
		{
			return nullptr;
		}
		return GetOrCreate<AxisList, Axis>(s_axes, index, false);
	}

	Axis* GetAxisBySlot(const size_t slot, const bool includeHidden)
	{
		size_t count = 0;
		for (size_t i = 0; i < MAX_TOTAL_AXES; ++i)
		{
			Axis* axis = GetAxis(i);
			if (axis == nullptr)
			{
				continue;
			}
			if (!includeHidden && !axis->visible)
			{
				continue;
			}
			if (count == slot)
			{
				return axis;
			}
			count++;
		}
		return nullptr;
	}

	Axis* GetAxisByLetter(const char letter)
	{
		for (size_t i = 0; i < MAX_TOTAL_AXES; ++i)
		{
			Axis* axis = GetAxis(i);
			if (axis != nullptr && axis->letter[0] == letter)
			{
				return axis;
			}
		}
		return nullptr;
	}

	Axis* GetOrCreateAxis(const size_t index)
	{
		dbg("Axis index %d / max %d\n", index, MAX_TOTAL_AXES);
		if (index >= MAX_TOTAL_AXES)
		{
			return nullptr;
		}
		return GetOrCreate<AxisList, Axis>(s_axes, index, true);
	}

	size_t GetAxisCount(const bool includeHidden)
	{
		size_t count = 0;
		for (size_t i = 0; i < MAX_TOTAL_AXES; ++i)
		{
			Axis* axis = GetAxis(i);
			if (axis == nullptr)
			{
				continue;
			}
			if (!includeHidden && !axis->visible)
			{
				continue;
			}
			count++;
		}
		return count;
	}

	bool IterateAxesWhile(function_ref<bool(Axis*&, size_t)> func, const size_t startAt)
	{
		return s_axes.IterateWhile(func, startAt);
	}

	size_t RemoveAxis(const size_t index, const bool allFollowing)
	{
		return Remove<AxisList, Axis>(s_axes, index, allFollowing);
	}

#define AXIS_SETTER(funcName, valType, varName)                                                                        \
	bool funcName(size_t index, valType val)                                                                           \
	{                                                                                                                  \
		if (index >= MAX_TOTAL_AXES)                                                                                   \
		{                                                                                                              \
			error("axis[%d] greater than MAX_TOTAL_AXES", index);                                                      \
			return false;                                                                                              \
		}                                                                                                              \
		Axis* axis = GetOrCreateAxis(index);                                                                           \
		if (axis == nullptr)                                                                                           \
		{                                                                                                              \
			error("Could not get or create axis %d", index);                                                           \
			return false;                                                                                              \
		}                                                                                                              \
		axis->varName = val;                                                                                           \
		return true;                                                                                                   \
	}

	AXIS_SETTER(SetBabystepOffset, float, babystep);
	// Update the homed status of the specified axis. If the axis is -1 then it
	// represents the "all homed" status.
	AXIS_SETTER(SetAxisHomedStatus, bool, homed);
	AXIS_SETTER(SetAxisLetter, char, letter[0]);
	AXIS_SETTER(SetAxisUserPosition, float, userPosition);
	AXIS_SETTER(SetAxisMachinePosition, float, machinePosition);
	AXIS_SETTER(SetAxisMinPosition, float, minPosition);
	AXIS_SETTER(SetAxisMaxPosition, float, maxPosition);
	AXIS_SETTER(SetAxisVisible, bool, visible);

	bool SetAxisWorkplaceOffset(size_t axisIndex, size_t workplaceIndex, float offset)
	{
		if (axisIndex >= MAX_TOTAL_AXES || workplaceIndex >= OM::Move::Workplaces::MaxTotalWorkplaces)
			return false;
		Axis* axis = GetOrCreateAxis(axisIndex);
		if (axis == nullptr)
			return false;
		axis->workplaceOffsets[workplaceIndex] = offset;
		return true;
	}

	bool SetCurrentWorkplaceNumber(uint8_t workplaceNumber)
	{
		if (workplaceNumber >= Workplaces::MaxTotalWorkplaces)
		{
			return false;
		}
		s_currentWorkplaceNumber = workplaceNumber;
		return true;
	}

	const uint8_t GetCurrentWorkplaceNumber()
	{
		return s_currentWorkplaceNumber;
	}

	void ExtruderAxis::Reset()
	{
		index = 0;
		position = 0.0f;
		factor = 0.0f;
		stepsPerMm = 0.0f;
	}

	ExtruderAxis* GetExtruderAxis(const size_t index)
	{
		dbg("ExtruderAxis index %d / max %d\n", index, MAX_TOTAL_AXES);
		if (index >= MAX_TOTAL_AXES)
		{
			return nullptr;
		}
		return GetOrCreate<ExtruderAxisList, ExtruderAxis>(s_extruderAxes, index, false);
	}

	ExtruderAxis* GetExtruderAxisBySlot(const size_t slot)
	{
		if (slot >= MAX_TOTAL_AXES)
		{
			return nullptr;
		}
		return s_extruderAxes[slot];
	}

	ExtruderAxis* GetOrCreateExtruderAxis(const size_t index)
	{
		dbg("ExtruderAxis index %d / max %d\n", index, MAX_TOTAL_AXES);
		if (index >= MAX_TOTAL_AXES)
		{
			error("ExtruderAxis index %d greater than MAX_TOTAL_AXES", index);
			return nullptr;
		}
		return GetOrCreate<ExtruderAxisList, ExtruderAxis>(s_extruderAxes, index, true);
	}

	size_t GetExtruderAxisCount()
	{
		return s_extruderAxes.Size();
	}

	bool IterateExtruderAxesWhile(function_ref<bool(ExtruderAxis*&, size_t)> func, const size_t startAt)
	{
		return s_extruderAxes.IterateWhile(func, startAt);
	}

	size_t RemoveExtruderAxis(const size_t index, const bool allFollowing)
	{
		return Remove<ExtruderAxisList, ExtruderAxis>(s_extruderAxes, index, allFollowing);
	}

#define EXTRUDER_AXIS_SETTER(funcName, valType, varName)                                                               \
	bool funcName(size_t index, valType val)                                                                           \
	{                                                                                                                  \
		ExtruderAxis* extruder = GetOrCreateExtruderAxis(index);                                                       \
		if (extruder == nullptr)                                                                                       \
		{                                                                                                              \
			error("Could not get or create extruderAxis %d", index);                                                   \
			return false;                                                                                              \
		}                                                                                                              \
		extruder->varName = val;                                                                                       \
		return true;                                                                                                   \
	}

	EXTRUDER_AXIS_SETTER(SetExtruderPosition, float, position);
	EXTRUDER_AXIS_SETTER(SetExtruderFactor, float, factor);
	EXTRUDER_AXIS_SETTER(SetExtruderStepsPerMm, float, stepsPerMm);
	EXTRUDER_AXIS_SETTER(SetExtruderFilamentDiameter, float, filamentDiameter);
	EXTRUDER_AXIS_SETTER(SetExtruderPressureAdvance, float, pressureAdvance);

	bool SetExtruderFilamentName(size_t index, const char* name)
	{
		ExtruderAxis* extruder = GetOrCreateExtruderAxis(index);
		if (extruder == nullptr)
		{
			error("Could not get or create extruderAxis %d", index);
			return false;
		}
		extruder->filamentName.CopyAndPad(name);
		return true;
	}

	void SetExtrusionRate(float rate)
	{
		sExtrusionRate = rate;
	}

	const float GetExtrusionRate()
	{
		return sExtrusionRate;
	}

	const float GetVolumetricFlow()
	{
		OM::Tool* tool = OM::GetCurrentTool();
		if (tool == nullptr)
		{
			return 0.0f;
		}
		float filamentArea = 0;
		float numExtruders = 0;
		tool->IterateExtruders([&](OM::Move::ExtruderAxis* extruder, size_t index) {
			if (extruder == nullptr)
				return;
			numExtruders++;
			filamentArea += tool->mix[index] * (M_PI * pow(extruder->filamentDiameter / 2, 2));
		});

		if (numExtruders == 0)
		{
			return 0.0f;
		}

		filamentArea /= numExtruders;
		return filamentArea * GetExtrusionRate();
	}
} // namespace OM::Move
