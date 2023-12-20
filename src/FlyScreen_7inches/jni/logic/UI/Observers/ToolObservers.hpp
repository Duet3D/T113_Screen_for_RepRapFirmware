/*
 * ToolObservers.hpp
 *
 *  Created on: 19 Dec 2023
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_UI_TOOLOBSERVERS_HPP_
#define JNI_LOGIC_UI_TOOLOBSERVERS_HPP_

#include "ObjectModel/Tool.hpp"
#include "ObjectModel/Utils.hpp"
#include "Debug.hpp"

#include "UI/OmObserver.hpp"
#include "UI/UserInterfaceConstants.hpp"
#include "UI/UserInterface.hpp"



/*
 * These functions are run when the OM field is received.
 * The function takes 2 arguments:
 * 	- val data from the OM where the type is automatically converted based on the chosen macro
 * 	- indices is an array of the indicies for the OM key
 * The _IF_CHANGED suffix only runs the function if the data is different from the previous
 * time function was called. This is unique to each combination of indices.
 */
static UI::Observer<UI::ui_field_update_cb> ToolObserversFields[] = {
	/* Update what heaters are associated with what tool */
	OBSERVER_UINT_IF_CHANGED(
		"tools^:heaters^",
		[](OBSERVER_UINT_ARGS)
		{
			if (!OM::UpdateToolHeater(indices[0], indices[1], (uint8_t)val))
			{
				dbg("Failed to update tool %d heater %d", indices[0], indices[1]);
				return;
			}
		}),
	OBSERVER_CHAR(
		"tools^:name",
		[](OBSERVER_CHAR_ARGS)
		{
			if (!OM::UpdateToolName(indices[0], val))
			{
				dbg("Failed to update tool %d name to %s", indices[0], val);
				return;
			}
		}),
};

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
static UI::Observer<UI::ui_array_end_update_cb> ToolObserversArrayEnd[] = {
	OBSERVER_ARRAY_END(
		"tools^",
		[](OBSERVER_ARRAY_END_ARGS)
		{
			if (OM::RemoveTool(indices[0], true))
				UI::RefreshToolList(mToolListViewPtr);
		}),
	OBSERVER_ARRAY_END(
		"tools^:heaters^",
		[](OBSERVER_ARRAY_END_ARGS)
		{
			if (OM::RemoveToolHeaters(indices[0], indices[1]))
				UI::RefreshToolList(mToolListViewPtr);
		}),
};




#endif /* JNI_LOGIC_UI_TOOLOBSERVERS_HPP_ */
