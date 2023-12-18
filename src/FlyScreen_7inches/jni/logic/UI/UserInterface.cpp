/*
 * UserInterface.cpp
 *
 *  Created on: Nov 19, 2023
 *      Author: Loïc & Andy Everitt
 */

#include <UI/UserInterface.hpp>

#include <ObjectModel/Axis.hpp>
#include <ObjectModel/Utils.hpp>

#define DEBUG (1)
#include "Debug.hpp"

namespace UI
{
	Element<ui_field_update_cb> *omFieldElementHead = nullptr;
	Element<ui_array_end_update_cb> *omArrayEndElementHead = nullptr;
	ElementMap<ui_field_update_cb> elementMap;
	ElementMap<ui_array_end_update_cb> elementMapArrayEnd;
}
