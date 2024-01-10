/*
 * UserInterface.cpp
 *
 *  Created on: Nov 19, 2023
 *      Author: Loïc & Andy Everitt
 */

#include <ObjectModel/Axis.h>
#include <ObjectModel/Utils.h>
#include "OmObserver.h"

#define DEBUG (1)
#include "Debug.h"

namespace UI
{
	Observer<ui_field_update_cb> *omFieldObserverHead = nullptr;
	Observer<ui_array_end_update_cb> *omArrayEndObserverHead = nullptr;
	ObserverMap<ui_field_update_cb> observerMap;
	ObserverMap<ui_array_end_update_cb> observerMapArrayEnd;
}
