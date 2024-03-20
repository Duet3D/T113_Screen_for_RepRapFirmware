/*
 * Sensor.cpp
 *
 *  Created on: 20 Mar 2024
 *      Author: andy
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "ListHelpers.h"
#include "Sensor.h"
#include <Duet3D/General/Vector.h>

typedef Vector<OM::AnalogSensor*, MAX_SENSORS> AnalogSensorList;
typedef Vector<OM::Endstop*, MAX_ENDSTOPS> EndstopList;

static AnalogSensorList analogSensors;
static EndstopList endstops;

namespace OM
{
	void AnalogSensor::Reset()
	{
		index = 0;
		lastReading = 0.0;
		name.Clear();
	}

	void Endstop::Reset()
	{
		index = 0;
		triggered = false;
	}

	const size_t GetAnalogSensorCount()
	{
		return analogSensors.Size();
	}

	AnalogSensor* GetAnalogSensor(const size_t index)
	{
		return GetOrCreate<AnalogSensorList, AnalogSensor>(analogSensors, index, false);
	}

	AnalogSensor* GetOrCreateAnalogSensor(const size_t index)
	{
		return GetOrCreate<AnalogSensorList, AnalogSensor>(analogSensors, index, true);
	}

	AnalogSensor* GetAnalogSensorBySlot(const size_t index)
	{
		if (index >= analogSensors.Size())
			return nullptr;

		return analogSensors[index];
	}

	size_t RemoveAnalogSensor(const size_t index, const bool allFollowing)
	{
		info("Removing analogSensor %d (allFollowing=%s)", index, allFollowing ? "true" : "false");
		return Remove<AnalogSensorList, AnalogSensor>(analogSensors, index, allFollowing);
	}

	bool UpdateAnalogSensorReading(const size_t index, const float reading)
	{
		AnalogSensor* const sensor = GetOrCreateAnalogSensor(index);
		if (sensor == nullptr)
		{
			warn("Failed to get or create analog sensor %d", index);
			return false;
		}

		sensor->lastReading = reading;
		return true;
	}

	bool UpdateAnalogSensorName(const size_t index, const char* name)
	{
		AnalogSensor* const sensor = GetOrCreateAnalogSensor(index);
		if (sensor == nullptr)
		{
			warn("Failed to get or create analog sensor %d", index);
			return false;
		}

		sensor->name.copy(name);
		if (sensor->name.IsEmpty())
		{
			sensor->name.copy("Heater ");
			sensor->name.catf("%d", index);
		}
		return true;
	}

	const size_t GetEndstopCount()
	{
		return endstops.Size();
	}

	Endstop* GetEndstop(const size_t index)
	{
		return GetOrCreate<EndstopList, Endstop>(endstops, index, false);
	}

	Endstop* GetOrCreateEndstop(const size_t index)
	{
		return GetOrCreate<EndstopList, Endstop>(endstops, index, true);
	}

	size_t RemoveEndstop(const size_t index, const bool allFollowing)
	{
		info("Removing endstop %d (allFollowing=%s)", index, allFollowing ? "true" : "false");
		return Remove<EndstopList, Endstop>(endstops, index, allFollowing);
	}

	bool UpdateEndstopTriggered(const size_t index, const bool triggered)
	{
		Endstop* const endstop = GetOrCreateEndstop(index);
		if (endstop == nullptr)
		{
			warn("Failed to get or create endstop %d", index);
			return false;
		}

		endstop->triggered = triggered;
		return true;
	}
} // namespace OM
