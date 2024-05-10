/*
 * Sensor.cpp
 *
 *  Created on: 20 Mar 2024
 *      Author: andy
 */

#include "Debug.h"

#include "ListHelpers.h"
#include "Sensor.h"
#include "utils/TimeHelper.h"
#include <Duet3D/General/Vector.h>

typedef Vector<OM::AnalogSensor*, MAX_SENSORS> AnalogSensorList;
typedef Vector<OM::Endstop*, MAX_ENDSTOPS> EndstopList;

static AnalogSensorList s_analogSensors;
static EndstopList s_endstops;

namespace OM
{
	void AnalogSensor::Reset()
	{
		index = 0;
		lastReading = 0.0;
		lastReadingTime = 0;
		name.Clear();
	}

	void Endstop::Reset()
	{
		index = 0;
		triggered = false;
	}

	const size_t GetAnalogSensorCount()
	{
		return s_analogSensors.Size();
	}

	AnalogSensor* GetAnalogSensor(const size_t index, const bool silent)
	{
		return GetOrCreate<AnalogSensorList, AnalogSensor>(s_analogSensors, index, false, silent);
	}

	AnalogSensor* GetOrCreateAnalogSensor(const size_t index)
	{
		return GetOrCreate<AnalogSensorList, AnalogSensor>(s_analogSensors, index, true);
	}

	AnalogSensor* GetAnalogSensorBySlot(const size_t index)
	{
		if (index >= s_analogSensors.Size())
			return nullptr;

		return s_analogSensors[index];
	}

	size_t RemoveAnalogSensor(const size_t index, const bool allFollowing)
	{
		info("Removing analogSensor %d (allFollowing=%s)", index, allFollowing ? "true" : "false");
		return Remove<AnalogSensorList, AnalogSensor>(s_analogSensors, index, allFollowing);
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
		sensor->lastReadingTime = TimeHelper::getCurrentTime();
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
		return s_endstops.Size();
	}

	Endstop* GetEndstop(const size_t index)
	{
		return GetOrCreate<EndstopList, Endstop>(s_endstops, index, false);
	}

	Endstop* GetOrCreateEndstop(const size_t index)
	{
		return GetOrCreate<EndstopList, Endstop>(s_endstops, index, true);
	}

	size_t RemoveEndstop(const size_t index, const bool allFollowing)
	{
		info("Removing endstop %d (allFollowing=%s)", index, allFollowing ? "true" : "false");
		return Remove<EndstopList, Endstop>(s_endstops, index, allFollowing);
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
