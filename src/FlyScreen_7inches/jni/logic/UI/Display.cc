/*
 * Display.cc
 *
 *  Created on: Dec 14, 2023
 *      Author: Andy Everitt
 */

#pragma once
#include "Communication.hpp"
#include "UI/UserInterface.hpp"
#define DEBUG (1)
#include "Debug.hpp"


static UI::Element elements[] = {
	ELEMENT_FLOAT(
			"heat:heaters^:current",
			[](const float val, const size_t indices[])
			{
				mTempGraphPtr->addData(indices[0], val);
				LOGD("current temperature heater %d = %fC", indices[0], val);
			}
	),
	ELEMENT_FLOAT(
			"heat:heaters^:current",
			[](const float val, const size_t indices[])
			{
				dbg("test");
			})
};
