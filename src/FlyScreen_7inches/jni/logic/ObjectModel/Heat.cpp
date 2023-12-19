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
		void Heater::Reset()
		{
			index = 0;
			activeTemp = 0;
			standbyTemp = 0;
			current = 0;
			avgPwm = 0;
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

		size_t RemoveHeater(const size_t index, const bool allFollowing)
		{
			return Remove<HeaterList, Heater>(heaters, index, allFollowing);
		}
	}
}
