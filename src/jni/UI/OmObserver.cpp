/*
 * UserInterface.cpp
 *
 *  Created on: Nov 19, 2023
 *      Author: Loïc & Andy Everitt
 */
#include "Debug.h"

#include "OmObserver.h"

#include <ObjectModel/Axis.h>
#include <ObjectModel/Utils.h>


namespace UI
{
	Observer<ui_field_update_cb>* g_omFieldObserverHead = nullptr;
	Observer<ui_array_end_update_cb>* g_omArrayEndObserverHead = nullptr;
	ObserverMap<ui_field_update_cb> g_observerMap;
	ObserverMap<ui_array_end_update_cb> g_observerMapArrayEnd;
}
