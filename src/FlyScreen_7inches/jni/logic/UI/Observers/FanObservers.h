/*
 * FanObservers.h
 *
 *  Created on: 8 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_UI_OBSERVERS_FANOBSERVERS_HPP_
#define JNI_LOGIC_UI_OBSERVERS_FANOBSERVERS_HPP_

#include "Debug.h"

#include "UI/OmObserver.h"
#include "UI/UserInterfaceConstants.h"
#include "UI/UserInterface.h"
#include "ObjectModel/Fan.h"


/*
 * These functions are run when the OM field is received.
 * The function takes 2 arguments:
 * 	- val data from the OM where the type is automatically converted based on the chosen macro
 * 	- indices is an array of the indicies for the OM key
 * The _IF_CHANGED suffix only runs the function if the data is different from the previous
 * time function was called. This is unique to each combination of indices.
 */
static UI::Observer<UI::ui_field_update_cb> FanObserversField[] = {
	OBSERVER_CHAR(
		"fans^",
		[](OBSERVER_CHAR_ARGS)
		{
			OM::RemoveFan(indices[0], false);
			mPrintFanListPtr->refreshListView();
		}),
	OBSERVER_FLOAT(
		"fans^:actualValue",
		[](OBSERVER_FLOAT_ARGS)
		{
			if (!OM::UpdateFanActualVal(indices[0], val))
			{
				dbg("Failed to update fan %d actualValue to %.2f", indices[0], val);
				return;
			}
			mPrintFanListPtr->refreshListView();
		}
	),
	OBSERVER_FLOAT(
		"fans^:requestedValue",
		[](OBSERVER_FLOAT_ARGS)
		{
			if (!OM::UpdateFanRequestedVal(indices[0], val))
			{
				dbg("Failed to update fan %d requestedValue to %.2f", indices[0], val);
				return;
			}
			mPrintFanListPtr->refreshListView();
		}
	),
	OBSERVER_INT(
		"fans^:rpm",
		[](OBSERVER_INT_ARGS)
		{
			if (!OM::UpdateFanRpm(indices[0], val))
			{
				dbg("Failed to update fan %d rpm to %d", indices[0], val);
				return;
			}
			mPrintFanListPtr->refreshListView();
		}
	),
};

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
static UI::Observer<UI::ui_array_end_update_cb> FanObserversArrayEnd[] = {
	OBSERVER_ARRAY_END(
		"fans^",
		[](OBSERVER_ARRAY_END_ARGS)
		{
			if (OM::RemoveFan(indices[0], true))
			{
				mPrintFanListPtr->refreshListView();
			}
		}),
};



#endif /* JNI_LOGIC_UI_OBSERVERS_FANOBSERVERS_HPP_ */
