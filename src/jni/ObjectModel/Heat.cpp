/*
 * Heat.cpp
 *
 *  Created on: 19 Dec 2023
 *      Author: Andy Everitt
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_WARN
#include "Heat.h"
#include "ListHelpers.h"
#include "uart/CommDef.h"
#include <Duet3D/General/Vector.h>
#include <UI/UserInterfaceConstants.h>
#include "ObjectModel/Utils.h"

#include "Debug.h"

typedef Vector<OM::Heat::Heater*, MaxHeaters> HeaterList;
static HeaterList heaters;

namespace OM
{
	namespace Heat
	{
		size_t lastHeater = -1;

		void Heater::Reset()
		{
			index = 0;
			activeTemp = 0;
			standbyTemp = 0;
			current = 0;
			avgPwm = 0;
		}

		const char * Heater::GetHeaterStatusStr()
		{
			const HeaterStatusMapEntry key = {"unknown", status};
			const HeaterStatusMapEntry *statusFromMap =
				(HeaterStatusMapEntry *)bsearch(
					&key,
					heaterStatusMap,
					ARRAY_SIZE(heaterStatusMap),
					sizeof(HeaterStatusMapEntry),
					compareValue<HeaterStatusMapEntry>);

			return (statusFromMap != nullptr) ? statusFromMap->key : "unknown";
		}

		void Heater::UpdateTarget(const int32_t temp, const bool active)
		{
			if (active)
			{
				activeTemp = temp;
				return;
			}
			standbyTemp = temp;
		}

		Heater *GetHeater(const size_t index)
		{
			return GetOrCreate<HeaterList, Heater>(heaters, index, false);
		}

		Heater *GetOrCreateHeater(const size_t index)
		{
			return GetOrCreate<HeaterList, Heater>(heaters, index, true);
		}

		bool IterateHeatersWhile(function_ref<bool(Heater *&, size_t)> func, const size_t startAt)
		{
			return heaters.IterateWhile(func, startAt);
		}

		bool UpdateHeaterTarget(const size_t heaterIndex, const int32_t temp, const bool active)
		{
			Heater *heater = GetOrCreateHeater(heaterIndex);

			// If we do not handle this heater back off
			if (heater == nullptr)
			{
				return false;
			}

			heater->UpdateTarget(temp, active);
			return true;
		}

		bool UpdateHeaterTemp(const size_t heaterIndex, const float temp)
		{
			Heater *heater = GetOrCreateHeater(heaterIndex);

			// If we do not handle this heater back off
			if (heater == nullptr)
			{
				return false;
			}

			heater->UpdateTemp(temp);
			return true;
		}

		bool UpdateHeaterStatus(const size_t heaterIndex, const char *statusStr)
		{
			const HeaterStatusMapEntry key = {statusStr, HeaterStatus::off};
			const HeaterStatusMapEntry *statusFromMap =
				(HeaterStatusMapEntry *)bsearch(
					&key,
					heaterStatusMap,
					ARRAY_SIZE(heaterStatusMap),
					sizeof(HeaterStatusMapEntry),
					compareKey<HeaterStatusMapEntry>);
			HeaterStatus status = (statusFromMap != nullptr) ? statusFromMap->val : HeaterStatus::off;
			return UpdateHeaterStatus(heaterIndex, status);
		}

		bool UpdateHeaterStatus(const size_t heaterIndex, HeaterStatus status)
		{
			Heater *heater = GetOrCreateHeater(heaterIndex);

			if (heater == nullptr)
				return false;

			heater->status = status;
			dbg("Heater %d state=%d %s", heaterIndex, heater->status, heater->GetHeaterStatusStr());
			return true;
		}

		size_t RemoveHeater(const size_t index, const bool allFollowing)
		{
			info("Removing heater %d (allFollowing=%s)", index, allFollowing ? "true" : "false");
			return Remove<HeaterList, Heater>(heaters, index, allFollowing);
		}
	}
}
