/*
 * Axis.cpp
 *
 *  Created on: 17 Feb 2021
 *      Author: manuel
 */

#define DEBUG (1)
#include "Debug.hpp"

#include "Axis.hpp"
#include "ListHelpers.hpp"
#include <Duet3D/General/Vector.hpp>
#include <UI/UserInterfaceConstants.hpp>



typedef Vector<OM::Move::Axis*, MaxTotalAxes> AxisList;
static AxisList axes;
static uint8_t currentWorkplaceNumber = OM::Move::Workplaces::MaxTotalWorkplaces;

namespace OM::Move
{
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
		slot = MaxSlots;
	}

	Axis* GetAxis(const size_t index)
	{
		// dbg("Axis index %d / max %d\n", index, MaxTotalAxes);
		if (index >= MaxTotalAxes)
		{
			return nullptr;
		}
		return GetOrCreate<AxisList, Axis>(axes, index, false);
	}

	Axis* GetOrCreateAxis(const size_t index)
	{
		// dbg("Axis index %d / max %d\n", index, MaxTotalAxes);
		if (index >= MaxTotalAxes)
		{
			return nullptr;
		}
		return GetOrCreate<AxisList, Axis>(axes, index, true);
	}

	size_t GetAxisCount()
	{
		return axes.Size();
	}

	bool IterateAxesWhile(function_ref<bool(Axis*&, size_t)> func, const size_t startAt)
	{
		return axes.IterateWhile(func, startAt);
	}

	size_t RemoveAxis(const size_t index, const bool allFollowing)
	{
		return Remove<AxisList, Axis>(axes, index, allFollowing);
	}

#define AXIS_SETTER(funcName, valType, varName) \
	bool funcName(size_t index, valType val) { \
		if (index >= MaxTotalAxes) { \
			dbg("axis[%d] greater than MaxTotalAxes", index); \
			return false; \
		} \
		Axis *axis = GetOrCreateAxis(index); \
		if (axis == nullptr) { \
			dbg("Could not get or create axis %d", index); \
			return false; \
		} \
		dbg("Set axis->%s", #varName); \
		axis->varName = val; \
		return true; \
	}

	AXIS_SETTER(SetBabystepOffset, float, babystep);
	// Update the homed status of the specified axis. If the axis is -1 then it represents the "all homed" status.
	AXIS_SETTER(SetAxisHomedStatus, bool, homed);
	AXIS_SETTER(SetAxisLetter, char, letter[0]);
	AXIS_SETTER(SetAxisUserPosition, float, userPosition);
	AXIS_SETTER(SetAxisMachinePosition, float, machinePosition);
	AXIS_SETTER(SetAxisVisible, bool, visible);

	bool SetAxisWorkplaceOffset(size_t axisIndex, size_t workplaceIndex, float offset)
	{
		if (axisIndex >= MaxTotalAxes || workplaceIndex >= OM::Move::Workplaces::MaxTotalWorkplaces)
			return false;
		Axis *axis = GetOrCreateAxis(axisIndex);
		if (axis == nullptr)
			return false;
		axis->workplaceOffsets[workplaceIndex] = offset;
		return true;
	}

	bool SetCurrentWorkplaceNumber(uint8_t workplaceNumber)
	{
		if (currentWorkplaceNumber == workplaceNumber || workplaceNumber >= Workplaces::MaxTotalWorkplaces)
		{
			return false;
		}
		currentWorkplaceNumber = workplaceNumber;
		return true;
	}

	const uint8_t GetCurrentWorkplaceNumber()
	{
		return currentWorkplaceNumber;
	}

}
