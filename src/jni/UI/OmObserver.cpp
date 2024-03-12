/*
 * UserInterface.cpp
 *
 *  Created on: Nov 19, 2023
 *      Author: Loïc & Andy Everitt
 */
#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "OmObserver.h"

#include <ObjectModel/Axis.h>
#include <ObjectModel/Utils.h>


namespace UI
{
	Observer<ui_field_update_cb> *omFieldObserverHead = nullptr;
	Observer<ui_array_end_update_cb> *omArrayEndObserverHead = nullptr;
	ObserverMap<ui_field_update_cb> observerMap;
	ObserverMap<ui_array_end_update_cb> observerMapArrayEnd;
}
