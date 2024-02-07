/*
 * HttpObservers.h
 *
 *  Created on: 30 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_UI_OBSERVERS_HTTPOBSERVERS_H_
#define JNI_LOGIC_UI_OBSERVERS_HTTPOBSERVERS_H_

#include "Debug.h"

#include "Hardware/Duet.h"
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
static UI::Observer<UI::ui_field_update_cb> HttpObserversField[] = {
    OBSERVER_INT("sessionKey",
                  [](OBSERVER_INT_ARGS) {
                       Comm::duet.SetSessionKey(val);
                  }),
};


#endif /* JNI_LOGIC_UI_OBSERVERS_HTTPOBSERVERS_H_ */
