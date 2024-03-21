/*
 * Heat.h
 *
 *  Created on: 19 Dec 2023
 *      Author: Andy Everitt
 */

#ifndef JNI_OBJECTMODEL_HEAT_HPP_
#define JNI_OBJECTMODEL_HEAT_HPP_

#include "Configuration.h"
#include "Sensor.h"
#include <Duet3D/General/Bitmap.h>
#include <Duet3D/General/FreelistManager.h>
#include <Duet3D/General/String.h>
#include <Duet3D/General/StringRef.h>
#include <Duet3D/General/function_ref.h>
#include <sys/types.h>

namespace OM
{
	namespace Heat
	{
		// Status that a tool may report to us. Must be in alphabetical order.
		enum class HeaterStatus
		{
			active = 0,
			fault,
			off,
			offline,
			standby,
			tuning,
		};

		struct HeaterStatusMapEntry
		{
			const char* key;
			HeaterStatus val;
		};

		// This table must be kept in case-insensitive alphabetical order of the search string.
		const HeaterStatusMapEntry heaterStatusMap[] =
		{
			{ "active",		HeaterStatus::active },
			{ "fault",		HeaterStatus::fault },
			{ "off",	 	HeaterStatus::off },
			{ "offline",	HeaterStatus::offline },
			{ "standby",	HeaterStatus::standby },
			{ "tuning",		HeaterStatus::tuning },
		};

		struct Heater
		{
			void* operator new(size_t) noexcept { return FreelistManager::Allocate<Heater>(); }
			void operator delete(void* p) noexcept { FreelistManager::Release<Heater>(p); }

			uint8_t index;	// This is the heater number
			int32_t activeTemp;
			int32_t standbyTemp;
			float current;
			float avgPwm;
			HeaterStatus status;
			AnalogSensor* sensor;

			void Reset();
			int32_t GetTemperature();
			int32_t GetHeaterTarget(const bool active);
			const char * GetHeaterStatusStr();
			void UpdateTarget(const int32_t temp, const bool active);
			void UpdateTemp(const float temp) { current = temp; }
		};

		Heater* GetHeater(const size_t heaterIndex);
		Heater* GetOrCreateHeater(const size_t heaterIndex);
		bool IterateHeatersWhile(function_ref<bool(Heater*&, size_t)> func, const size_t startAt = 0);
		bool UpdateHeaterTarget(const size_t heaterIndex, const int32_t temp, const bool active);
		bool UpdateHeaterTemp(const size_t heaterIndex, const float temp);
		bool UpdateHeaterStatus(const size_t heaterIndex, HeaterStatus status);
		bool UpdateHeaterStatus(const size_t heaterIndex, const char *status);
		bool UpdateHeaterSensor(const size_t heaterIndex, const size_t sensorIndex);
		size_t RemoveHeater(const size_t index, const bool allFollowing);

		extern size_t lastHeater;
	}
}

#endif /* JNI_OBJECTMODEL_HEAT_HPP_ */
