/*
 * PushObservers.hpp
 *
 *  Created on: 2 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_UI_OBSERVERS_PUSHOBSERVERS_HPP_
#define JNI_LOGIC_UI_OBSERVERS_PUSHOBSERVERS_HPP_

#include "Debug.hpp"
#include <string>
#include "Duet3D/General/String.h"
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

static String<MaxResponseLineLength> sGcodeResponses[MaxResponseLines];
static size_t sGcodeResponseHead = 0;

static UI::Observer<UI::ui_field_update_cb> PushObserversField[] = {
	OBSERVER_CHAR(
		"resp",
		[](OBSERVER_CHAR_ARGS)
		{
			static size_t index = 0;
			static string str;

			str = val;
			String<MaxResponseLineLength> line;
			dbg("resp: %s", val);
			while (str.length() > MaxResponseLineLength)
			{
				line.copy(str.substr(0, MaxResponseLineLength).c_str());
				str = str.substr(MaxResponseLineLength, str.length() - MaxResponseLineLength);
				sGcodeResponses[index] = line;
				index++;
			}
			dbg("resp: adding line to sGcodeResponses[%d]", index);
			line.copy(str.c_str());
			sGcodeResponses[index] = line;
			index++;

		}
	),
	OBSERVER_CHAR(
		"message",
		[](OBSERVER_CHAR_ARGS)
		{
			dbg("message: %s", val);
		}
	),
	OBSERVER_CHAR(
		"seq",
		[](OBSERVER_CHAR_ARGS)
		{
			dbg("seq: %s", val);
		}
	),
};

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
static UI::Observer<UI::ui_array_end_update_cb> PushObserversArrayEnd[] = {
	OBSERVER_ARRAY_END(
		"",
		[](OBSERVER_ARRAY_END_ARGS)
		{
		}
	),
};



#endif /* JNI_LOGIC_UI_OBSERVERS_PUSHOBSERVERS_HPP_ */
