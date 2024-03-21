/*
 * ObjectModel.cpp
 *
 *  Created on: 7 Sep 2020
 *      Author: manuel
 */
#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "Axis.h"
#include "BedOrChamber.h"
#include "ListHelpers.h"
#include "Spindle.h"
#include "Tool.h"
#include "Utils.h"

namespace OM
{
	void RemoveAll()
	{
		Move::RemoveAxis(0, true);
		Move::RemoveExtruderAxis(0, true);
		RemoveBed(0, true);
		RemoveChamber(0, true);
		RemoveFan(0, true);
		Heat::RemoveHeater(0, true);
		RemoveAnalogSensor(0, true);
		RemoveEndstop(0, true);
		RemoveSpindle(0, true);
		RemoveTool(0, true);
	}
}
