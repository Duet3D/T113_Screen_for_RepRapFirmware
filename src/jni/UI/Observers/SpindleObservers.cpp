/*
 * SpindleObservers.cpp
 *
 *  Created on: 5 Mar 2024
 *      Author: Andy Everitt
 */

#include "Debug.h"

#include "Configuration.h"
#include "UI/OmObserver.h"
#include "UI/UserInterface.h"

#include "ObjectModel/Spindle.h"
#include "ObjectModel/Tool.h"
#include "ObjectModel/Utils.h"

/*
 * These functions are run when the OM field is received.
 * The function takes 2 arguments:
 *  - val data from the OM where the type is automatically converted based on the chosen macro
 *  - indices is an array of the indicies for the OM key
 * The _IF_CHANGED suffix only runs the function if the data is different from the previous
 * time function was called. This is unique to each combination of indices.
 */
static UI::Observer<UI::ui_field_update_cb> SpindleObserversFields[] = {
	OBSERVER_CHAR("spindles^", [](OBSERVER_CHAR_ARGS) { OM::RemoveSpindle(indices[0], false); }),
	OBSERVER_UINT("spindles^:active",
				  [](OBSERVER_UINT_ARGS) {
					  if (!OM::SetSpindleActive(indices[0], val))
					  {
						  error("Failed to update spindle %d active to %u", indices[0], val);
						  return;
					  }
				  }),
	OBSERVER_BOOL("spindles^:canReverse",
				  [](OBSERVER_BOOL_ARGS) {
					  if (!OM::SetSpindleCanReverse(indices[0], val))
					  {
						  error("Failed to update spindle %d canReverse to %d", indices[0], val);
						  return;
					  }
				  }),
	OBSERVER_UINT("spindles^:current",
				  [](OBSERVER_UINT_ARGS) {
					  if (!OM::SetSpindleCurrent(indices[0], val))
					  {
						  error("Failed to update spindle %d current to %u", indices[0], val);
						  return;
					  }
				  }),
	OBSERVER_UINT("spindles^:max",
				  [](OBSERVER_UINT_ARGS) {
					  if (!OM::SetSpindleMax(indices[0], val))
					  {
						  error("Failed to update spindle %d max to %u", indices[0], val);
						  return;
					  }
				  }),
	OBSERVER_UINT("spindles^:min",
				  [](OBSERVER_UINT_ARGS) {
					  if (!OM::SetSpindleMin(indices[0], val))
					  {
						  error("Failed to update spindle %d min to %u", indices[0], val);
						  return;
					  }
				  }),
	OBSERVER_CHAR("spindles^:state",
				  [](OBSERVER_CHAR_ARGS) {
					  if (!OM::SetSpindleState(indices[0], val))
					  {
						  error("Failed to update spindle %d state to %s", indices[0], val);
						  return;
					  }
				  }),
};

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
static UI::Observer<UI::ui_array_end_update_cb> SpindleObserversArrayEnd[] = {
	OBSERVER_ARRAY_END("spindles^",
					   [](OBSERVER_ARRAY_END_ARGS) {
						   if (OM::RemoveSpindle(indices[0], true))
						   {
						   }
					   }),
};
