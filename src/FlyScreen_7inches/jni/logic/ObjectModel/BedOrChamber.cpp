/*
 * BedOrChamber.cpp
 *
 *  Created on: 17 Feb 2021
 *      Author: manuel
 */

#include "BedOrChamber.hpp"
#include "ListHelpers.hpp"
#include <Duet3D/General/String.h>
#include <Duet3D/General/Vector.hpp>
#include "ObjectModel/Heat.hpp"
#include <UI/UserInterfaceConstants.hpp>
#include "Hardware/SerialIo.hpp"

#define DEBUG (1)
#include "Debug.hpp"

typedef Vector<OM::Bed*, MaxSlots> BedList;
typedef Vector<OM::Chamber*, MaxSlots> ChamberList;

static BedList beds;
static ChamberList chambers;


namespace OM
{
	int8_t lastBed = -1;
	int8_t lastChamber = -1;

	void BedOrChamber::Reset()
	{
		index = 0;
		heater = -1;
		heaterStatus = HeaterStatus::off;
		slot = MaxSlots;
	}

	bool BedOrChamber::SetBedTemp(const int32_t temp, const bool active)
	{
		Heat::Heater* pheater = Heat::GetHeater(heater);
		if (pheater == nullptr)
			return false;

		String<MaxCommandLength> command;
		command.catf("M140 P%d %s%d", index, active ? "S" : "R", temp);

		SerialIo::Sendf(command.c_str());
		return true;
	}

	bool BedOrChamber::SetChamberTemp(const int32_t temp, const bool active)
	{
		Heat::Heater* pheater = Heat::GetHeater(heater);
		if (pheater == nullptr)
			return false;

		String<MaxCommandLength> command;
		command.catf("M141 P%d %s%d", index, active ? "S" : "R", temp);

		SerialIo::Sendf(command.c_str());
		return true;
	}

	Bed* GetBed(const size_t index)
	{
		return GetOrCreate<BedList, Bed>(beds, index, false);
	}

	Bed* GetOrCreateBed(const size_t index)
	{
		return GetOrCreate<BedList, Bed>(beds, index, true);
	}

	Bed* GetFirstBed()
	{
		return Find<BedList, Bed>(beds, [](Bed* bed)
		{	return bed->heater > -1;});
	}

	size_t GetBedCount()
	{
		return beds.Size();
	}

	bool IterateBedsWhile(function_ref<bool(Bed*&, size_t)> func,
			const size_t startAt)
	{
		return beds.IterateWhile(func, startAt);
	}

	size_t RemoveBed(const size_t index, const bool allFollowing)
	{
		dbg("Removing bed %d allFollowing=%s", index, allFollowing? "true" : "false");
		return Remove<BedList, Bed>(beds, index, allFollowing);
	}

	Chamber* GetChamber(const size_t index)
	{
		return GetOrCreate<ChamberList, Chamber>(chambers, index, false);
	}

	Chamber* GetOrCreateChamber(const size_t index)
	{
		return GetOrCreate<ChamberList, Chamber>(chambers, index, true);
	}

	Chamber* GetFirstChamber()
	{
		return Find<ChamberList, Chamber>(chambers, [](Chamber* chamber)
		{	return chamber->heater > -1;});
	}

	size_t GetChamberCount()
	{
		return chambers.Size();
	}

	bool IterateChambersWhile(function_ref<bool(Chamber*&, size_t)> func,
			const size_t startAt)
	{
		return chambers.IterateWhile(func, startAt);
	}

	size_t RemoveChamber(const size_t index, const bool allFollowing)
	{
		return Remove<ChamberList, Chamber>(chambers, index, allFollowing);
	}

	bool SetBedHeater(const uint8_t bedIndex, const int8_t heaterNumber)
	{
		auto bed = OM::GetOrCreateBed(bedIndex);
		if (bed == nullptr)
		{
			dbg("Failed to get or create bed %d", bedIndex);
			return false;
		}
		bed->heater = heaterNumber;
		dbg("Created bed %d, heater number %d", bedIndex, heaterNumber);
		return true;

	}

	bool SetChamberHeater(const uint8_t chamberIndex, const int8_t heaterNumber)
	{
		auto chamber = OM::GetOrCreateChamber(chamberIndex);
		if (chamber == nullptr)
		{
			return false;
		}
		chamber->heater = heaterNumber;
		return true;
	}
}
