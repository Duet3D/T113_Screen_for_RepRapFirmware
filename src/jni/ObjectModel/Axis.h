/*
 * Axis.h
 *
 *  Created on: 17 Feb 2021
 *      Author: manuel
 */

#ifndef SRC_OBJECTMODEL_AXIS_HPP_
#define SRC_OBJECTMODEL_AXIS_HPP_

// #include <cstdint>
#include "Configuration.h"
#include <Duet3D/General/FreelistManager.h>
#include <Duet3D/General/String.h>
#include <Duet3D/General/function_ref.h>
#include <sys/types.h>

namespace OM::Move
{
	enum Workplaces
	{
		G54,
		G55,
		G56,
		G57,
		G58,
		G59,
		G59_1,
		G59_2,
		G59_3,
		MaxTotalWorkplaces
	};

	struct Axis
	{
		void* operator new(size_t) noexcept { return FreelistManager::Allocate<Axis>(); }
		void operator delete(void* p) noexcept { FreelistManager::Release<Axis>(p); }

		uint8_t index;
		float babystep;
		char letter[2];
		float workplaceOffsets[Workplaces::MaxTotalWorkplaces];
		float userPosition;
		float machinePosition;
		uint8_t homed : 1,
			visible : 1,
			slot : 6;

		void Reset();
	};

	struct ExtruderAxis
	{
		void* operator new(size_t) noexcept { return FreelistManager::Allocate<ExtruderAxis>(); }
		void operator delete(void* p) noexcept { FreelistManager::Release<ExtruderAxis>(p); }

		uint8_t index;
		float position;
		float factor;
		float stepsPerMm;
		float filamentDiameter;
		float pressureAdvance;
		String<MAX_FILAMENT_NAME_LENGTH> filamentName;

		void Reset();
	};

	Axis* GetAxis(const size_t index);
	Axis* GetAxisBySlot(const size_t slot);
	Axis* GetOrCreateAxis(const size_t index);
	size_t GetAxisCount();
	bool IterateAxesWhile(function_ref<bool(Axis*&, size_t)> func, const size_t startAt = 0);
	size_t RemoveAxis(const size_t index, const bool allFollowing);

	bool SetBabystepOffset(size_t index, float f);
	bool SetAxisHomedStatus(size_t index, bool homed);
	bool SetAxisLetter(size_t index, char letter);
	bool SetAxisUserPosition(size_t index, float f);
	bool SetAxisMachinePosition(size_t index, float f);
	bool SetAxisVisible(size_t index, bool visible);
	bool SetAxisWorkplaceOffset(size_t axisIndex, size_t workplaceIndex, float offset);
	bool SetCurrentWorkplaceNumber(uint8_t workplaceNumber);
	const uint8_t GetCurrentWorkplaceNumber();

	ExtruderAxis* GetExtruderAxis(const size_t index);
	ExtruderAxis* GetExtruderAxisBySlot(const size_t slot);
	ExtruderAxis* GetOrCreateExtruderAxis(const size_t index);
	size_t GetExtruderAxisCount();
	bool IterateExtruderAxesWhile(function_ref<bool(ExtruderAxis *&, size_t)> func, const size_t startAt = 0);
	size_t RemoveExtruderAxis(const size_t index, const bool allFollowing);

	bool SetExtruderPosition(size_t index, float f);
	bool SetExtruderFactor(size_t index, float f);
	bool SetExtruderStepsPerMm(size_t index, float f);
	bool SetExtruderFilamentDiameter(size_t index, float f);
	bool SetExtruderPressureAdvance(size_t index, float f);
	bool SetExtruderFilamentName(size_t index, const char* name);

	void SetExtrusionRate(float rate);
	const float GetExtrusionRate();
	const float GetVolumetricFlow();
}


#endif /* SRC_OBJECTMODEL_AXIS_HPP_ */
