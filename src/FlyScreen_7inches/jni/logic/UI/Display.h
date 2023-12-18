/*
 * Display.cc
 *
 *  Created on: Dec 14, 2023
 *      Author: Andy Everitt
 */

#pragma once
#include "Communication.hpp"
#include "UI/UserInterface.hpp"
#include "UI/UserInterfaceConstants.hpp"
#include "ObjectModel/Tool.hpp"
#include "ObjectModel/Utils.hpp"
#define DEBUG (0)
#include "Debug.hpp"


static UI::Element elements[] = {
	/* Update what heaters are associated with what tool */
	ELEMENT_UINT_IF_CHANGED(
			"tools^:heaters^",
			[](ELEMENT_UINT_ARGS)
			{
				if (!OM::UpdateToolHeater(indices[0], indices[1], (uint8_t)val))
				{
					dbg("Failed to update tool %d heater %d", indices[0], indices[1]);
					return;
				}
			}
	),
	/* Update what tool heaters active temperature */
	ELEMENT_INT_IF_CHANGED(
			"tools^:active^",
			[](ELEMENT_INT_ARGS)
			{
				if (!OM::UpdateToolTemp(indices[0], indices[1], val, true))
				{
					dbg("Failed to update tool %d heater %d active temperature to %d", indices[0], indices[1], val);
					return;
				}
			}
	),
	/* Update what tool heaters standby temperature */
	ELEMENT_INT_IF_CHANGED(
			"tools^:standby^",
			[](ELEMENT_INT_ARGS)
			{
				if (!OM::UpdateToolTemp(indices[0], indices[1], val, false))
				{
					dbg("Failed to update tool %d heater %d standby temperature to %d", indices[0], indices[1], val);
					return;
				}
			}
	),
	ELEMENT_FLOAT(
			"heat:heaters^:current",
			[](ELEMENT_FLOAT_ARGS)
			{
				mTempGraphPtr->addData(indices[0], val);
				dbg("current temperature heater %d = %fC", indices[0], val);
			}
	),
};
