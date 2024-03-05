/*
 * ToolObservers.h
 *
 *  Created on: 19 Dec 2023
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_UI_TOOLOBSERVERS_HPP_
#define JNI_LOGIC_UI_TOOLOBSERVERS_HPP_

#include "Debug.h"

#include "UI/OmObserver.h"
#include "UI/UserInterface.h"
#include "UI/UserInterfaceConstants.h"

#include "ObjectModel/Tool.h"
#include "ObjectModel/Utils.h"

/*
 * These functions are run when the OM field is received.
 * The function takes 2 arguments:
 * 	- val data from the OM where the type is automatically converted based on the chosen macro
 * 	- indices is an array of the indicies for the OM key
 * The _IF_CHANGED suffix only runs the function if the data is different from the previous
 * time function was called. This is unique to each combination of indices.
 */
static UI::Observer<UI::ui_field_update_cb> ToolObserversFields[] = {
	OBSERVER_CHAR("tools^", [](OBSERVER_CHAR_ARGS) { OM::RemoveTool(indices[0], false); }),
	/* Update what heaters are associated with what tool */
	OBSERVER_UINT("tools^:heaters^",
				  [](OBSERVER_UINT_ARGS) {
					  if (!OM::UpdateToolHeater(indices[0], indices[1], (uint8_t)val))
					  {
						  error("Failed to update tool %d heater %d", indices[0], indices[1]);
						  return;
					  }
					  UI::ToolsList::RefreshAllToolLists();
				  }),
	/* Update what extruders are associated with what tool */
	OBSERVER_UINT("tools^:extruders^",
				  [](OBSERVER_UINT_ARGS) {
					  if (!OM::UpdateToolExtruder(indices[0], indices[1], (uint8_t)val))
					  {
						  error("Failed to update tool %d extruder %d", indices[0], indices[1]);
						  return;
					  }
				  }),
	/* Update what fans are associated with what tool */
	OBSERVER_UINT("tools^:fans^",
				  [](OBSERVER_UINT_ARGS) {
					  if (!OM::UpdateToolFan(indices[0], indices[1], (uint8_t)val))
					  {
						  error("Failed to update tool %d fan %d", indices[0], indices[1]);
						  return;
					  }
				  }),
	OBSERVER_INT("tools^:filamentExtruder",
				 [](OBSERVER_INT_ARGS) {
					 if (!OM::UpdateToolFilamentExtruder(indices[0], (int8_t)val))
					 {
						 error("Failed to update tool %d filamentExtruder to %d", indices[0], val);
						 return;
					 }
				 }),
	OBSERVER_INT("tools^:active^",
				 [](OBSERVER_INT_ARGS) {
					 if (!OM::UpdateToolTemp(indices[0], indices[1], val, true))
					 {
						 error("Failed to update tool %d active temperature[%d] to %d", indices[0], indices[1], val);
						 return;
					 }
					 UI::ToolsList::RefreshAllToolLists(false);
				 }),
	/* Update what tool heaters standby temperature */
	OBSERVER_INT("tools^:standby^",
				 [](OBSERVER_INT_ARGS) {
					 if (!OM::UpdateToolTemp(indices[0], indices[1], val, false))
					 {
						 error("Failed to update tool %d standby temperature[%d] to %d", indices[0], indices[1], val);
						 return;
					 }
					 UI::ToolsList::RefreshAllToolLists(false);
				 }),
	OBSERVER_CHAR("tools^:name",
				  [](OBSERVER_CHAR_ARGS) {
					  if (!OM::UpdateToolName(indices[0], val))
					  {
						  error("Failed to update tool %d name to %s", indices[0], val);
						  return;
					  }
				  }),
	OBSERVER_FLOAT("tools^:mix^",
				   [](OBSERVER_FLOAT_ARGS) {
					   if (!OM::UpdateToolMix(indices[0], indices[1], val))
					   {
						   error("Failed to update tool %d mix %d to %.2f", indices[0], indices[1], val);
						   return;
					   }
				   }),
	OBSERVER_CHAR("tools^:state",
				  [](OBSERVER_CHAR_ARGS) {
					  if (!OM::UpdateToolStatus(indices[0], val))
					  {
						  error("Failed to update tool %d name to %s", indices[0], val);
						  return;
					  }
				  }),
};

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
static UI::Observer<UI::ui_array_end_update_cb> ToolObserversArrayEnd[] = {
	OBSERVER_ARRAY_END("tools^",
					   [](OBSERVER_ARRAY_END_ARGS) {
						   if (OM::RemoveTool(indices[0], true))
						   {
						   }
						   UI::ToolsList::RefreshAllToolLists();
					   }),
	OBSERVER_ARRAY_END("tools^:heaters^",
					   [](OBSERVER_ARRAY_END_ARGS) {
						   if (OM::RemoveToolHeaters(indices[0], indices[1]))
						   {
						   }
						   UI::ToolsList::RefreshAllToolLists();
					   }),
	OBSERVER_ARRAY_END("tools^:extruders^",
					   [](OBSERVER_ARRAY_END_ARGS) {
						   if (OM::RemoveToolExtruders(indices[0], indices[1]))
						   {
						   }
					   }),
	OBSERVER_ARRAY_END("tools^:fans^",
					   [](OBSERVER_ARRAY_END_ARGS) {
						   if (OM::RemoveToolFans(indices[0], indices[1]))
						   {
						   }
					   }),
};

#endif /* JNI_LOGIC_UI_TOOLOBSERVERS_HPP_ */
