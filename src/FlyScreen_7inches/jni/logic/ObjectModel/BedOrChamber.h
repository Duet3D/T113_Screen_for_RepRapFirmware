/*
 * BedOrChamber.h
 *
 *  Created on: 17 Feb 2021
 *      Author: manuel
 */

#ifndef SRC_OBJECTMODEL_BEDORCHAMBER_HPP_
#define SRC_OBJECTMODEL_BEDORCHAMBER_HPP_


//#include <cstdint>
#include <sys/types.h>
#include <Duet3D/General/FreelistManager.h>
#include <Duet3D/General/function_ref.h>

namespace OM
{
	enum class BedOrChamberStatus
	{
		off = 0,
		standby,
		active,
		fault,
		tuning,
		offline
	};


	struct BedOrChamberStatusMapEntry
	{
		const char* key;
		const BedOrChamberStatus val;
	};

	// This table has to be kept in alphabetical order of the keys
	const BedOrChamberStatusMapEntry bedOrChamberStatusMap[] =
	{
		{ "active",		BedOrChamberStatus::active },
		{ "fault",		BedOrChamberStatus::fault },
		{ "off",	 	BedOrChamberStatus::off },
		{ "offline",	BedOrChamberStatus::offline },
		{ "standby",	BedOrChamberStatus::standby },
		{ "tuning",		BedOrChamberStatus::tuning },
	};

	struct BedOrChamber
	{
		void* operator new(size_t) noexcept { return FreelistManager::Allocate<BedOrChamber>(); }
		void operator delete(void* p) noexcept { FreelistManager::Release<BedOrChamber>(p); }

		// Index within configured heaters
		uint8_t index;
		// Id of heater
		int8_t heater;
		BedOrChamberStatus status;
		// Slot for display on panel
		uint8_t slot;

		void Reset();
		bool SetBedTemp(const int32_t temp, const bool active);
		bool SetChamberTemp(const int32_t temp, const bool active);
		bool ToggleBedState();
		bool ToggleChamberState();
	};

	typedef BedOrChamber Bed;
	typedef BedOrChamber Chamber;

	Bed* GetBedBySlot(const size_t index);
	Bed* GetBed(const size_t index);
	Bed* GetOrCreateBed(const size_t index);
	Bed* GetFirstBed();
	size_t GetBedCount();
	bool IterateBedsWhile(function_ref<bool(Bed*&, size_t)> func, const size_t startAt = 0);
	size_t RemoveBed(const size_t index, const bool allFollowing);

	Chamber* GetChamberBySlot(const size_t index);
	Chamber* GetChamber(const size_t index);
	Chamber* GetOrCreateChamber(const size_t index);
	Chamber* GetFirstChamber();
	size_t GetChamberCount();
	bool IterateChambersWhile(function_ref<bool(Chamber*&, size_t)> func, const size_t startAt = 0);
	size_t RemoveChamber(const size_t index, const bool allFollowing);
	bool SetBedHeater(const uint8_t bedIndex, const int8_t heaterNumber);
	bool SetChamberHeater(const uint8_t chamberIndex, const int8_t heaterNumber);

	extern int8_t lastBed;
	extern int8_t lastChamber;

}

#endif /* SRC_OBJECTMODEL_BEDORCHAMBER_HPP_ */
