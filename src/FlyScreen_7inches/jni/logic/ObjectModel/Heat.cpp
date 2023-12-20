/*
 * Heat.cpp
 *
 *  Created on: 19 Dec 2023
 *      Author: Andy Everitt
 */


#include "Heat.hpp"
#include "ListHelpers.hpp"
#include <Duet3D/General/Vector.hpp>
#include <UI/UserInterfaceConstants.hpp>

#define DEBUG (1)
#include "Debug.hpp"

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

		size_t RemoveHeater(const size_t index, const bool allFollowing)
		{
			return Remove<HeaterList, Heater>(heaters, index, allFollowing);
		}
	}
}
