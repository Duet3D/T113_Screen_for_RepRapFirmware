/*
 * StateObservers.h
 *
 *  Created on: 9 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_UI_OBSERVERS_STATEOBSERVERS_HPP_
#define JNI_LOGIC_UI_OBSERVERS_STATEOBSERVERS_HPP_

#include "Debug.h"

#include "UI/OmObserver.h"
#include "UI/UserInterfaceConstants.h"
#include "UI/UserInterface.h"
#include "ObjectModel/PrinterStatus.h"



/*
 * These functions are run when the OM field is received.
 * The function takes 2 arguments:
 * 	- val data from the OM where the type is automatically converted based on the chosen macro
 * 	- indices is an array of the indicies for the OM key
 * The _IF_CHANGED suffix only runs the function if the data is different from the previous
 * time function was called. This is unique to each combination of indices.
 */
static UI::Observer<UI::ui_field_update_cb> StateObserversField[] = {
	OBSERVER_CHAR(
		"state:status",
		[](OBSERVER_CHAR_ARGS)
		{
			OM::SetStatus(val);
			switch (OM::GetStatus())
			{
			case OM::PrinterStatus::printing:
			case OM::PrinterStatus::simulating:
				mPrintPauseBtnPtr->setVisible(true);
				mPrintResumeBtnPtr->setVisible(false);
				mPrintCancelBtnPtr->setVisible(false);
				break;
			case OM::PrinterStatus::paused:
			case OM::PrinterStatus::pausing:
			case OM::PrinterStatus::resuming:
				mPrintPauseBtnPtr->setVisible(false);
				mPrintResumeBtnPtr->setVisible(true);
				mPrintCancelBtnPtr->setVisible(true);
				break;
			case OM::PrinterStatus::idle:
			case OM::PrinterStatus::configuring:
			case OM::PrinterStatus::connecting:
				mPrintFileNamePtr->setText("");
				mPrintElapsedTimePtr->setText("Elapsed:");
				mPrintEstimatedTimePtr->setText("Estimated:");
				mPrintPauseBtnPtr->setVisible(false);
				mPrintResumeBtnPtr->setVisible(false);
				mPrintCancelBtnPtr->setVisible(false);
				break;
			default:
				break;
			}

			mStatusTextPtr->setText(val);
		}
	),
};

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
static UI::Observer<UI::ui_array_end_update_cb> StateObserversArrayEnd[] = {
//	OBSERVER_ARRAY_END(
//		"",
//		[](OBSERVER_ARRAY_END_ARGS)
//		{
//		}
//	),
};



#endif /* JNI_LOGIC_UI_OBSERVERS_STATEOBSERVERS_HPP_ */
