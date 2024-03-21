/*
 * ObjectModel.cpp
 *
 *  Created on: 7 Sep 2020
 *      Author: manuel
 */
#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "UI/UserInterface.h"

#include "Axis.h"
#include "BedOrChamber.h"
#include "Configuration.h"
#include "DebugCommands.h"
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

	static Debug::DebugCommand log_OM("dbg_log_OM", []() {
		UI::CONSOLE->AddResponse("ObjectModel:");
		UI::CONSOLE->AddResponse("  Axes:");
		for (size_t i = 0; i < MAX_TOTAL_AXES; ++i)
		{
			Move::Axis* axis = Move::GetAxis(i);
			if (axis != nullptr)
			{
				UI::CONSOLE->AddResponse(
					utils::format("    [%d]: (%s), babystep(%.3f), user(%.2f), machine(%.2f), homed(%d)",
								  axis->index,
								  axis->letter,
								  axis->babystep,
								  axis->userPosition,
								  axis->machinePosition,
								  axis->homed)
						.c_str());
			}
		}
		UI::CONSOLE->AddResponse("  Extruders:");
		for (size_t i = 0; i < MAX_TOTAL_AXES; ++i)
		{
			Move::ExtruderAxis* extruder = Move::GetExtruderAxis(i);
			if (extruder != nullptr)
			{
				UI::CONSOLE->AddResponse(
					utils::format("    [%d]: pos(%.2f), f(%.2f), spmm(%.1f), pa(%.2f), fil(%.2f, %s)",
								  extruder->index,
								  extruder->position,
								  extruder->factor,
								  extruder->stepsPerMm,
								  extruder->pressureAdvance,
								  extruder->filamentDiameter,
								  extruder->filamentName.c_str())
						.c_str());
			}
		}

		UI::CONSOLE->AddResponse("");
	});
}
