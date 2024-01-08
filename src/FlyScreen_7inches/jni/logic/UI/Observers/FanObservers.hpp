/*
 * FanObservers.hpp
 *
 *  Created on: 8 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_UI_OBSERVERS_FANOBSERVERS_HPP_
#define JNI_LOGIC_UI_OBSERVERS_FANOBSERVERS_HPP_

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
static UI::Observer<UI::ui_field_update_cb> FanObserversField[] = {
	OBSERVER_UINT_IF_CHANGED(
		"",
		[](OBSERVER_UINT_ARGS)
		{
		}
	),
};

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
static UI::Observer<UI::ui_array_end_update_cb> FanObserversArrayEnd[] = {
	OBSERVER_ARRAY_END(
		"",
		[](OBSERVER_ARRAY_END_ARGS)
		{
		}
	),
};



#endif /* JNI_LOGIC_UI_OBSERVERS_FANOBSERVERS_HPP_ */
