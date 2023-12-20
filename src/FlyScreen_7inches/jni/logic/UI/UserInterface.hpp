/*
 * MainMenuFunc.hpp
 *
 *  Created on: 18 Dec 2023
 *      Author: Andy E
 */

#ifndef JNI_LOGIC_UI_USERINTERFACE_HPP_
#define JNI_LOGIC_UI_USERINTERFACE_HPP_

#include <sys/types.h>
#include "control/ZKListView.h"
#include "window/ZKWindow.h"
#include "ObjectModel/Tool.hpp"
#include "ObjectModel/BedOrChamber.hpp"

namespace UI
{
	struct NumPadData
	{
		size_t toolIndex;
		size_t toolHeaterIndex;
		bool active;
	};

	class ToolsList
	{
	public:
		ToolsList() :
				totalCount(0)
		{
		}
		void CalculateTotalHeaterCount(const bool addTools = true,
				const bool addBeds = true, const bool addChambers = true);
		size_t GetTotalHeaterCount(const bool calculate, const bool addTools = true,
				const bool addBeds = true, const bool addChambers = true)
		{
			if (calculate) CalculateTotalHeaterCount(addTools, addBeds, addChambers);
			return totalCount;
		}
		void OpenNumPad(ZKWindow *numPadWindow, const size_t toolIndex, const size_t toolHeaterIndex, const bool active);
		void CloseNumPad(ZKWindow *numPadWindow);
		bool SendTempTarget(int32_t target);
	private:
		size_t totalCount;
		NumPadData numPadData;
	};

	extern ToolsList toolsList;

	int8_t GetToolHeaterIndex(const size_t listIndex, OM::Tool *&tool);
	void RefreshToolList(ZKListView *listView, const bool lengthChanged = true);

}

#endif /* JNI_LOGIC_UI_USERINTERFACE_HPP_ */
