/*
 * MoveObservers.cpp
 *
 *  Created on: 9 Jan 2024
 *      Author: Andy Everitt
 */
#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "Configuration.h"
#include "UI/OmObserver.h"
#include "UI/UserInterface.h"

#include "ObjectModel/Axis.h"

/*
 * These functions are run when the OM field is received.
 * The function takes 2 arguments:
 * 	- val data from the OM where the type is automatically converted based on the chosen macro
 * 	- indices is an array of the indicies for the OM key
 * The _IF_CHANGED suffix only runs the function if the data is different from the previous
 * time function was called. This is unique to each combination of indices.
 */
static UI::Observer<UI::ui_field_update_cb> MoveObserversField[] = {
	OBSERVER_FLOAT("move:axes^:babystep",
				   [](OBSERVER_FLOAT_ARGS) {
					   if (!OM::Move::SetBabystepOffset(indices[0], val))
					   {
						   error("Failed to set axis[%d]->babystep = %f", indices[0], val);
						   return;
					   }
				   }),
	OBSERVER_BOOL("move:axes^:homed",
				  [](OBSERVER_BOOL_ARGS) {
					  if (!OM::Move::SetAxisHomedStatus(indices[0], val))
					  {
						  error("Failed to set axis[%d]->homed = %d", indices[0], val);
						  return;
					  }
				  }),
	OBSERVER_CHAR("move:axes^:letter",
				  [](OBSERVER_CHAR_ARGS) {
					  if (!OM::Move::SetAxisLetter(indices[0], val[0]))
					  {
						  error("Failed to set axis[%d]->letter = %s", indices[0], val);
						  return;
					  }
				  }),
	OBSERVER_FLOAT("move:axes^:machinePosition",
				   [](OBSERVER_FLOAT_ARGS) {
					   if (!OM::Move::SetAxisMachinePosition(indices[0], val))
					   {
						   error("Failed to set axis[%d]->machinePosition = %f", indices[0], val);
						   return;
					   }
				   }),
	OBSERVER_FLOAT("move:axes^:min",
				   [](OBSERVER_FLOAT_ARGS) {
					   if (!OM::Move::SetAxisMinPosition(indices[0], val))
					   {
						   error("Failed to set axis[%d]->min = %f", indices[0], val);
						   return;
					   }
				   }),
	OBSERVER_FLOAT("move:axes^:max",
				   [](OBSERVER_FLOAT_ARGS) {
					   if (!OM::Move::SetAxisMaxPosition(indices[0], val))
					   {
						   error("Failed to set axis[%d]->max = %f", indices[0], val);
						   return;
					   }
				   }),
	OBSERVER_FLOAT("move:axes^:userPosition",
				   [](OBSERVER_FLOAT_ARGS) {
					   if (!OM::Move::SetAxisUserPosition(indices[0], val))
					   {
						   error("Failed to set axis[%d]->userPosition = %f", indices[0], val);
						   return;
					   }
				   }),
	OBSERVER_BOOL("move:axes^:visible",
				  [](OBSERVER_BOOL_ARGS) {
					  if (!OM::Move::SetAxisVisible(indices[0], val))
					  {
						  error("Failed to set axis[%d]->visible = %d", indices[0], val);
						  return;
					  }
				  }),
	OBSERVER_FLOAT("move:axes^:workplaceOffsets^",
				   [](OBSERVER_FLOAT_ARGS) {
					   if (!OM::Move::SetAxisWorkplaceOffset(indices[0], indices[1], val))
					   {
						   error("Failed to set axis[%d]->workplaceOffset[%d] = %f", indices[0], indices[1], val);
						   return;
					   }
				   }),
	OBSERVER_FLOAT("move:extruders^:factor",
				   [](OBSERVER_FLOAT_ARGS) {
					   if (!OM::Move::SetExtruderFactor(indices[0], val))
					   {
						   error("Failed to set extruderAxis[%d]->factor = %f", indices[0], val);
					   }
				   }),
	OBSERVER_FLOAT("move:extruders^:filamentDiameter",
				   [](OBSERVER_FLOAT_ARGS) {
					   if (!OM::Move::SetExtruderFilamentDiameter(indices[0], val))
					   {
						   error("Failed to set extruderAxis[%d]->filamentDiameter = %f", indices[0], val);
					   }
				   }),
	OBSERVER_CHAR("move:extruders^:filament",
				  [](OBSERVER_CHAR_ARGS) {
					  if (!OM::Move::SetExtruderFilamentName(indices[0], val))
					  {
						  error("Failed to set extruderAxis[%d]->filamentName = %s", indices[0], val);
					  }
					  UI::GetUIControl<ZKListView>(ID_MAIN_ExtrudeToolList)->refreshListView();
				  }),
	OBSERVER_FLOAT("move:extruders^:position",
				   [](OBSERVER_FLOAT_ARGS) {
					   if (!OM::Move::SetExtruderPosition(indices[0], val))
					   {
						   error("Failed to set extruderAxis[%d]->position = %f", indices[0], val);
					   }
				   }),
	OBSERVER_FLOAT("move:extruders^:pressureAdvance",
				   [](OBSERVER_FLOAT_ARGS) {
					   if (!OM::Move::SetExtruderPressureAdvance(indices[0], val))
					   {
						   error("Failed to set extruderAxis[%d]->pressureAdvance = %f", indices[0], val);
					   }
				   }),
	OBSERVER_FLOAT("move:extruders^:stepsPerMm",
				   [](OBSERVER_FLOAT_ARGS) {
					   if (!OM::Move::SetExtruderStepsPerMm(indices[0], val))
					   {
						   error("Failed to set extruderAxis[%d]->stepsPerMm = %f", indices[0], val);
					   }
				   }),
	OBSERVER_CHAR("move:kinematics:name", [](OBSERVER_CHAR_ARGS) {}),
	OBSERVER_FLOAT("move:speedFactor",
				   [](OBSERVER_FLOAT_ARGS) {
					   int factor = (int)(val * 100);
					   UI::GetUIControl<ZKTextView>(ID_MAIN_PrintSpeedFactor)->setTextf("%d %%", factor);
					   UI::GetUIControl<ZKSeekBar>(ID_MAIN_PrintSpeedMultiplierBar)->setMax(factor + 100);
				   }),
	OBSERVER_UINT("move:workplaceNumber",
				  [](OBSERVER_UINT_ARGS) {
					  if (!OM::Move::SetCurrentWorkplaceNumber((uint8_t)val))
					  {
						  error("Failed to set workplace number = %d", val);
						  return;
					  }
				  }),
	OBSERVER_FLOAT("move:currentMove:requestedSpeed",
				   [](OBSERVER_FLOAT_ARGS) {
					   UI::GetUIControl<ZKTextView>(ID_MAIN_PrintRequestedSpeed)->setTextTrf("requested_speed", val);
				   }),
	OBSERVER_FLOAT(
		"move:currentMove:topSpeed",
		[](OBSERVER_FLOAT_ARGS) { UI::GetUIControl<ZKTextView>(ID_MAIN_PrintTopSpeed)->setTextTrf("top_speed", val); }),
	OBSERVER_FLOAT(
		"move:currentMove:extrusionRate",
		[](OBSERVER_FLOAT_ARGS) {
			OM::Move::SetExtrusionRate(val);
			float volumetricFlow = OM::Move::GetVolumetricFlow();
			UI::GetUIControl<ZKTextView>(ID_MAIN_PrintVolFlow)->setTextTrf("volumetric_flow", volumetricFlow);
		}),
};

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
static UI::Observer<UI::ui_array_end_update_cb> MoveObserversArrayEnd[] = {
	OBSERVER_ARRAY_END("move:axes^",
					   [](OBSERVER_ARRAY_END_ARGS) {
						   OM::Move::RemoveAxis(indices[0], true);
						   UI::GetUIControl<ZKListView>(ID_MAIN_PrintPositionList)->refreshListView();
						   UI::GetUIControl<ZKListView>(ID_MAIN_AxisControlListView)->refreshListView();

						   OM::Move::IterateAxesWhile(
							   [](OM::Move::Axis*& axis, size_t) {
								   if (axis->letter[0] == 'Z')
								   {
									   UI::GetUIControl<ZKTextView>(ID_MAIN_PrintBabystepCurrentOffset)
										   ->setTextTrf("babystepping_offset", axis->babystep);
									   return false;
								   }
								   return true;
							   },
							   0);
					   }),
	OBSERVER_ARRAY_END("move:extruders^",
					   [](OBSERVER_ARRAY_END_ARGS) {
						   OM::Move::RemoveExtruderAxis(indices[0], true);
						   UI::GetUIControl<ZKListView>(ID_MAIN_PrintExtruderPositionList)->refreshListView();
					   }),
};
