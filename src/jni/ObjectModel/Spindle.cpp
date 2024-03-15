/*
 * Spindle.cpp
 *
 *  Created on: 17 Feb 2021
 *      Author: manuel
 */

#include "Spindle.h"
#include "Configuration.h"
#include "ListHelpers.h"
#include "ObjectModel/Utils.h"
#include "uart/CommDef.h"
#include <Duet3D/General/Vector.h>

typedef Vector<OM::Spindle*, MAX_SLOTS> SpindleList;
static SpindleList spindles;

namespace OM
{
	void Spindle::Reset()
	{
		index = 0;
		active = 0;
		current = 0;
		max = 10000;
		min = 60;
		state = SpindleState::stopped;
		canReverse = false;
	}

	const char* Spindle::GetStateStr()
	{
		const SpindleStateMapEntry key = {"unknown", state};
		const SpindleStateMapEntry* stateFromMap = (SpindleStateMapEntry*)bsearch(&key,
																				  spindleStateMap,
																				  ARRAY_SIZE(spindleStateMap),
																				  sizeof(SpindleStateMapEntry),
																				  compareValue<SpindleStateMapEntry>);

		return (stateFromMap != nullptr) ? stateFromMap->key : "unknown";
	}

	Spindle* GetSpindle(const size_t index)
	{
		return GetOrCreate<SpindleList, Spindle>(spindles, index, false);
	}

	Spindle* GetOrCreateSpindle(const size_t index)
	{
		return GetOrCreate<SpindleList, Spindle>(spindles, index, true);
	}

	size_t RemoveSpindle(const size_t index, const bool allFollowing)
	{
		return Remove<SpindleList, Spindle>(spindles, index, allFollowing);
	}

#define SPINDLE_SETTER(funcName, valType, varName)                                                                     \
	bool funcName(size_t index, valType val)                                                                           \
	{                                                                                                                  \
		if (index >= MAX_SLOTS)                                                                                        \
		{                                                                                                              \
			error("spindle[%d] greater than MAX_SLOTS", index);                                                        \
			return false;                                                                                              \
		}                                                                                                              \
		Spindle* spindle = GetOrCreateSpindle(index);                                                                  \
		if (spindle == nullptr)                                                                                        \
		{                                                                                                              \
			error("Could not get or create spindle %d", index);                                                        \
			return false;                                                                                              \
		}                                                                                                              \
		spindle->varName = val;                                                                                        \
		return true;                                                                                                   \
	}

	bool SetSpindleState(size_t index, const char* statusStr)
	{
		if (index >= MAX_SLOTS)
		{
			error("spindle[%d] greater than MAX_SLOTS", index);
			return false;
		}

		Spindle* spindle = GetOrCreateSpindle(index);

		// If we do not handle this spindle back off
		if (spindle == nullptr)
		{
			return false;
		}

		const SpindleStateMapEntry key = {statusStr, SpindleState::stopped};
		const SpindleStateMapEntry* stateFromMap = (SpindleStateMapEntry*)bsearch(&key,
																				  spindleStateMap,
																				  ARRAY_SIZE(spindleStateMap),
																				  sizeof(SpindleStateMapEntry),
																				  compareKey<SpindleStateMapEntry>);
		SpindleState state = (stateFromMap != nullptr) ? stateFromMap->val : SpindleState::stopped;
		spindle->state = state;
		return true;
	}

	SPINDLE_SETTER(SetSpindleActive, uint32_t, active)
	SPINDLE_SETTER(SetSpindleCanReverse, bool, canReverse)
	SPINDLE_SETTER(SetSpindleCurrent, uint32_t, current)
	SPINDLE_SETTER(SetSpindleMax, uint32_t, max)
	SPINDLE_SETTER(SetSpindleMin, uint32_t, min)
}
