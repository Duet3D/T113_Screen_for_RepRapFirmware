/*
 * MoveObservers.hpp
 *
 *  Created on: 9 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_UI_OBSERVERS_MOVEOBSERVERS_HPP_
#define JNI_LOGIC_UI_OBSERVERS_MOVEOBSERVERS_HPP_

#include "Debug.hpp"

#include "UI/OmObserver.hpp"
#include "UI/UserInterfaceConstants.hpp"
#include "UI/UserInterface.hpp"
#include "ObjectModel/Axis.hpp"



/*
 * These functions are run when the OM field is received.
 * The function takes 2 arguments:
 * 	- val data from the OM where the type is automatically converted based on the chosen macro
 * 	- indices is an array of the indicies for the OM key
 * The _IF_CHANGED suffix only runs the function if the data is different from the previous
 * time function was called. This is unique to each combination of indices.
 */
static UI::Observer<UI::ui_field_update_cb> MoveObserversField[] = {
	OBSERVER_FLOAT_IF_CHANGED(
		"move:axes^:babystep",
		[](OBSERVER_FLOAT_ARGS)
		{
			OM::Move::SetBabystepOffset(indices[0], val);
		}
	),
	OBSERVER_BOOL_IF_CHANGED(
		"move:axes^:homed",
		[](OBSERVER_BOOL_ARGS)
		{
			OM::Move::SetAxisHomedStatus(indices[0], val);
		}
	),
	OBSERVER_CHAR(
		"move:axes^:letter",
		[](OBSERVER_CHAR_ARGS)
		{
			OM::Move::SetAxisLetter(indices[0], val[0]);
		}
	),
	OBSERVER_FLOAT_IF_CHANGED(
		"move:axes^:machinePosition",
		[](OBSERVER_FLOAT_ARGS)
		{
			OM::Move::SetAxisMachinePosition(indices[0], val);
		}
	),
	OBSERVER_FLOAT_IF_CHANGED(
		"move:axes^:min",
		[](OBSERVER_FLOAT_ARGS)
		{
		}
	),
	OBSERVER_FLOAT_IF_CHANGED(
		"move:axes^:max",
		[](OBSERVER_FLOAT_ARGS)
		{
		}
	),
	OBSERVER_FLOAT_IF_CHANGED(
		"move:axes^:userPosition",
		[](OBSERVER_FLOAT_ARGS)
		{
			OM::Move::SetAxisUserPosition(indices[0], val);
		}
	),
	OBSERVER_BOOL_IF_CHANGED(
		"move:axes^:visible",
		[](OBSERVER_BOOL_ARGS)
		{
			OM::Move::SetAxisVisible(indices[0], val);
		}
	),
	OBSERVER_FLOAT_IF_CHANGED(
		"move:axes^:workplaceOffsets^",
		[](OBSERVER_FLOAT_ARGS)
		{
			OM::Move::SetAxisWorkplaceOffset(indices[0], indices[1], val);
		}
	),
	OBSERVER_FLOAT_IF_CHANGED(
		"move:extruders^:factor",
		[](OBSERVER_FLOAT_ARGS)
		{
		}
	),
	OBSERVER_CHAR(
		"move:kinematics:name",
		[](OBSERVER_CHAR_ARGS)
		{
		}
	),
	OBSERVER_FLOAT_IF_CHANGED(
		"move:speedFactor",
		[](OBSERVER_FLOAT_ARGS)
		{
		}
	),
	OBSERVER_FLOAT_IF_CHANGED(
		"move:workplaceNumber",
		[](OBSERVER_FLOAT_ARGS)
		{
			OM::Move::SetCurrentWorkplaceNumber(val);
		}
	),
};

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
static UI::Observer<UI::ui_array_end_update_cb> MoveObserversArrayEnd[] = {
	OBSERVER_ARRAY_END(
		"move:axes^",
		[](OBSERVER_ARRAY_END_ARGS)
		{
			OM::Move::RemoveAxis(indices[0], true);
			mPrintPositionListPtr->refreshListView();
		}
	),
};



#endif /* JNI_LOGIC_UI_OBSERVERS_MOVEOBSERVERS_HPP_ */
