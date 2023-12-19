/*
 * HeatObservers.hpp
 *
 *  Created on: 19 Dec 2023
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_UI_OBSERVERS_HEATOBSERVERS_HPP_
#define JNI_LOGIC_UI_OBSERVERS_HEATOBSERVERS_HPP_

#include "Debug.hpp"

#include "ObjectModel/BedOrChamber.hpp"
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
static UI::Element<UI::ui_field_update_cb> HeatObserversField[] = {
	/* Update heaters current reading */
	ELEMENT_FLOAT(
			"heat:heaters^:current",
			[](ELEMENT_FLOAT_ARGS)
			{
//				mTempGraphPtr->addData(indices[0], val);
				dbg("current temperature heater %d = %fC", indices[0], val);
			}),
	ELEMENT_INT(
			"heat:bedHeaters^",
			[](ELEMENT_INT_ARGS)
			{
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
	ELEMENT_INT(
			"heat:chamberHeaters^",
			[](ELEMENT_INT_ARGS)
			{
				if (val > -1)
				{
					OM::SetChamberHeater(indices[0], val);
					for (size_t i = OM::lastChamber+ 1; i < indices[0]; ++i)
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
static UI::Element<UI::ui_array_end_update_cb> HeatObserversArrayEnd[] = {
	ELEMENT_ARRAY_END(
			"heat:bedHeaters^",
			[](ELEMENT_ARRAY_END_ARGS)
			{
				OM::RemoveBed(OM::lastBed + 1, true);
				UI::RefreshToolList(mToolListViewPtr);
			}
	),
	ELEMENT_ARRAY_END(
			"heat:chamberHeaters^",
			[](ELEMENT_ARRAY_END_ARGS)
			{
				OM::RemoveChamber(OM::lastChamber + 1, true);
				UI::RefreshToolList(mToolListViewPtr);
			}
	),
};



#endif /* JNI_LOGIC_UI_OBSERVERS_HEATOBSERVERS_HPP_ */
