/*
 * SensorObservers.cpp
 *
 *  Created on: 20 March 2024
 *      Author: Andy Everitt
 */
#include "Debug.h"

#include "Configuration.h"
#include "UI/OmObserver.h"
#include "UI/UserInterface.h"

#include "ObjectModel/Sensor.h"
#include "UI/Logic/HomeScreen.h"

/*
 * These functions are run when the OM field is received.
 * The function takes 2 arguments:
 * 	- val data from the OM where the type is automatically converted based on the chosen macro
 * 	- indices is an array of the indicies for the OM key
 * The _IF_CHANGED suffix only runs the function if the data is different from the previous
 * time function was called. This is unique to each combination of indices.
 */
static UI::Observer<UI::ui_field_update_cb> SensorObserversField[] = {
	OBSERVER_CHAR("sensors:analog^", [](OBSERVER_CHAR_ARGS) { OM::RemoveAnalogSensor(indices[0], false); }),
	/* Update heaters current reading */
	OBSERVER_FLOAT("sensors:analog^:lastReading",
				   [](OBSERVER_FLOAT_ARGS) {
					   if (!OM::UpdateAnalogSensorReading(indices[0], val))
					   {
						   error("Failed to update sensor reading; sensor %d = %f", indices[0], val);
						   return;
					   }
				   }), /* Update what tool heaters active temperature */
	OBSERVER_CHAR("sensors:analog^:name",
				  [](OBSERVER_CHAR_ARGS) {
					  if (!OM::UpdateAnalogSensorName(indices[0], val))
					  {
						  error("Failed to update sensor name; sensor %d = %s", indices[0], val);
						  return;
					  }
					  UI::HomeScreen::RefreshTemperatureGraph();
				  }),
	OBSERVER_CHAR("sensors:endstops^", [](OBSERVER_CHAR_ARGS) { OM::RemoveEndstop(indices[0], false); }),
	OBSERVER_BOOL("sensors:endstops^:triggered",
				  [](OBSERVER_BOOL_ARGS) {
					  if (!OM::UpdateEndstopTriggered(indices[0], val))
					  {
						  error("Failed to update endstop triggered; endstop %d = %d", indices[0], val);
						  return;
					  }
				  }),
};

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
static UI::Observer<UI::ui_array_end_update_cb> SensorObserversArrayEnd[] = {
	OBSERVER_ARRAY_END("sensors:analog^",
					   [](OBSERVER_ARRAY_END_ARGS) {
						   if (OM::RemoveAnalogSensor(indices[0], true))
						   {
						   }
					   }),
	OBSERVER_ARRAY_END("sensors:endstops^",
					   [](OBSERVER_ARRAY_END_ARGS) {
						   if (OM::RemoveEndstop(indices[0], true))
						   {
						   }
					   }),
};
