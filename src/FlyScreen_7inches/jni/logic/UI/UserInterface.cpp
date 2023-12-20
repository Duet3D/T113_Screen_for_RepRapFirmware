/*
 * UserInterface.cpp
 *
 *  Created on: 18 Dec 2023
 *      Author: andy
 */

#include "UserInterface.hpp"
#include "UserInterfaceConstants.hpp"

#define DEBUG (1)
#include "Debug.hpp"

namespace UI
{
	ToolsList toolsList;

	void ToolsList::CalculateTotalHeaterCount(
				const bool addTools = true,
				const bool addBeds = true,
				const bool addChambers = true)
	{
		size_t count = 0;
		if (addBeds)
		{
			dbg("Bed count %d", OM::GetBedCount());
			count += OM::GetBedCount();
		}
		if (addChambers)
		{
			dbg("Chamber count %d", OM::GetChamberCount());
			count += OM::GetChamberCount();
		}

		if (addTools)
		{
			OM::IterateToolsWhile([&count](OM::Tool*& tool, size_t)
			{
				const bool hasHeater = tool->heaters[0] != nullptr;
				const bool hasSpindle = tool->spindle != nullptr;
				// Spindle takes precedence
				if (hasSpindle)
				{
					++count;
				}
				else if (hasHeater)
				{
					tool->IterateHeaters([&count, &tool](OM::Heat::Heater*, size_t)
							{
								++count;
							});
				}
				else
				{
					// Hides everything by default
					++count;
				}
				return true;
			});
		}
		if (count > MaxSlots)
		{
			dbg("Total heater items (%d) exceeds MaxSlots (%d)", count, MaxSlots);
			count = MaxSlots;
		}
		totalCount = count;
	}

	int8_t GetToolHeaterIndex(const size_t listIndex, OM::Tool *&tool)
	{
		int8_t count = 0;
		int8_t toolHeaterIndex = -1;
		OM::IterateToolsWhile([&](OM::Tool*& toolIter, size_t)
			{
				if (listIndex < count + toolIter->GetHeaterCount())
				{
					tool = toolIter;
					toolHeaterIndex = listIndex - count;
					return false;
				}
				count += toolIter->GetHeaterCount();
				return true;
			});
		if (toolHeaterIndex > -1)
		{
//			dbg("List index %d is Tool %d toolHeaterIndex %d=%d", listIndex, tool->index, toolHeaterIndex, tool->GetHeater(toolHeaterIndex)->index);
			return toolHeaterIndex;
		}
//		dbg("List index %d is not in tool heaters (total tool heaters %d)", listIndex, count);
		tool = nullptr;
		return count;				// list index is greater than all heaters for tools
	}

	void RefreshToolList(ZKListView *listView, const bool lengthChanged)
	{
		if (lengthChanged)
		{
			toolsList.CalculateTotalHeaterCount();
		}
		listView->refreshListView();
	}
}


