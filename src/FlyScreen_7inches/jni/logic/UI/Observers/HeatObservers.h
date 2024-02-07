/*
 * HeatObservers.h
 *
 *  Created on: 19 Dec 2023
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_UI_OBSERVERS_HEATOBSERVERS_HPP_
#define JNI_LOGIC_UI_OBSERVERS_HEATOBSERVERS_HPP_

#include "Debug.h"

#include "ObjectModel/BedOrChamber.h"
#include "ObjectModel/Heat.h"
#include "UI/OmObserver.h"
#include "UI/UserInterfaceConstants.h"
#include "UI/UserInterface.h"



/*
 * These functions are run when the OM field is received.
 * The function takes 2 arguments:
 * 	- val data from the OM where the type is automatically converted based on the chosen macro
 * 	- indices is an array of the indicies for the OM key
 * The _IF_CHANGED suffix only runs the function if the data is different from the previous
 * time function was called. This is unique to each combination of indices.
 */
static UI::Observer<UI::ui_field_update_cb> HeatObserversField[] = {
	OBSERVER_CHAR("heat:heaters^", [](OBSERVER_CHAR_ARGS) { OM::Heat::RemoveHeater(indices[0], false); }),
	/* Update heaters current reading */
	OBSERVER_FLOAT("heat:heaters^:current",
				   [](OBSERVER_FLOAT_ARGS) {
					   if (!OM::Heat::UpdateHeaterTemp(indices[0], val))
					   {
						   error("Failed to update heater temperature; heater %d = %fC", indices[0], val);
						   return;
					   }
					   UI::ToolsList::RefreshAllToolLists(false);
					   mTempGraphPtr->addData(indices[0], val);
				   }), /* Update what tool heaters active temperature */
	OBSERVER_INT("heat:heaters^:active",
				 [](OBSERVER_INT_ARGS) {
					 if (!OM::Heat::UpdateHeaterTarget(indices[0], val, true))
					 {
						 error("Failed to update heater %d active temperature to %d", indices[0], val);
						 return;
					 }
					 UI::ToolsList::RefreshAllToolLists(false);
				 }),
	/* Update what tool heaters standby temperature */
	OBSERVER_INT("heat:heaters^:standby",
				 [](OBSERVER_INT_ARGS) {
					 if (!OM::Heat::UpdateHeaterTarget(indices[0], val, false))
					 {
						 error("Failed to update heater %d standby temperature to %d", indices[0], val);
						 return;
					 }
					 UI::ToolsList::RefreshAllToolLists(false);
				 }),
	OBSERVER_CHAR("heat:heaters^:state", [](OBSERVER_CHAR_ARGS) { OM::Heat::UpdateHeaterStatus(indices[0], val); }),
	OBSERVER_INT("heat:bedHeaters^",
				 [](OBSERVER_INT_ARGS) {
					 if (val > -1)
					 {
						 OM::SetBedHeater(indices[0], val);
						 for (size_t i = OM::lastBed + 1; i < indices[0]; ++i)
						 {
							 OM::RemoveBed(i, false);
						 }
						 OM::lastBed = indices[0];
						 dbg("lastBed=%d", OM::lastBed);
					 }
				 }),
	OBSERVER_INT("heat:chamberHeaters^",
				 [](OBSERVER_INT_ARGS) {
					 if (val > -1)
					 {
						 OM::SetChamberHeater(indices[0], val);
						 for (size_t i = OM::lastChamber + 1; i < indices[0]; ++i)
						 {
							 OM::RemoveChamber(i, false);
						 }
						 OM::lastChamber = indices[0];
						 dbg("lastChamber=%d", OM::lastChamber);
					 }
				 }),
};

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
static UI::Observer<UI::ui_array_end_update_cb> HeatObserversArrayEnd[] = {
	OBSERVER_ARRAY_END(
		"heaters^",
		[](OBSERVER_ARRAY_END_ARGS)
		{
			if (OM::Heat::RemoveHeater(indices[0], true))
				UI::ToolsList::RefreshAllToolLists();
		}),
	OBSERVER_ARRAY_END(
		"heat:bedHeaters^",
		[](OBSERVER_ARRAY_END_ARGS)
		{
			OM::RemoveBed(OM::lastBed + 1, true);
			OM::lastBed = -1;
			UI::ToolsList::RefreshAllToolLists();
		}),
	OBSERVER_ARRAY_END(
		"heat:chamberHeaters^",
		[](OBSERVER_ARRAY_END_ARGS)
		{
			OM::RemoveChamber(OM::lastChamber + 1, true);
			OM::lastChamber = -1;
			UI::ToolsList::RefreshAllToolLists();
		}),
};



#endif /* JNI_LOGIC_UI_OBSERVERS_HEATOBSERVERS_HPP_ */
