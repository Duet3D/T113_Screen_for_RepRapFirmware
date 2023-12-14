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
	{
		"heat:heaters^:current",
		[](const char *data, const size_t arrayIndices[])
		{
			float fval;
			if (Comm::GetFloat(data, fval))
			{
				mTempGraphPtr->addData(arrayIndices[0], fval);
			}
		}
	},
};
