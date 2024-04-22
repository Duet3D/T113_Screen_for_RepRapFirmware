/*
 * ObjectCancel.cpp
 *
 *  Created on: 19 Apr 2024
 *      Author: andy
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "ObjectCancel.h"

namespace UI
{
	std::string GetObjectCancelXAxisText(int index)
	{
		static ZKListView* axisList = UI::GetUIControl<ZKListView>(ID_MAIN_ObjectCancelXAxis);

		if (axisList == nullptr)
		{
			error("Failed to get axis list");
			return "";
		}

		OM::Move::Axis* axis = OM::Move::GetAxisByLetter('X');
		if (axis == nullptr)
		{
			error("Failed to get axis");
			return "";
		}

		float min = axis->minPosition;
		float max = axis->maxPosition;

		return utils::format("%.0f", min + (max - min) * (float)index / (axisList->getCols() - 1));
	}

	std::string GetObjectCancelYAxisText(int index)
	{
		static ZKListView* axisList = UI::GetUIControl<ZKListView>(ID_MAIN_ObjectCancelYAxis);

		if (axisList == nullptr)
		{
			error("Failed to get axis list");
			return "";
		}

		OM::Move::Axis* axis = OM::Move::GetAxisByLetter('Y');
		if (axis == nullptr)
		{
			error("Failed to get axis");
			return "";
		}

		float min = axis->minPosition;
		float max = axis->maxPosition;

		return utils::format("%.0f", max - (max - min) * (float)index / (axisList->getRows() - 1));
	}

} // namespace UI
