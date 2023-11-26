/*
 * UserInterface.hpp
 *
 *  Created on: Nov 19, 2023
 *      Author: Loïc
 */

#ifndef JNI_LOGIC_UI_USERINTERFACE_HPP_
#define JNI_LOGIC_UI_USERINTERFACE_HPP_


#include <ObjectModel/BedOrChamber.hpp>
#include <ObjectModel/PrinterStatus.hpp>
#include <ObjectModel/Spindle.hpp>
#include <ObjectModel/Tool.hpp>
#include <UI/Display.hpp>
#include <Duet3D/General/String.h>
#include <Duet3D/General/StringFunctions.h>



namespace UI
{
	static FloatField *controlTabAxisPos[MaxDisplayableAxes];

	extern void ShowAxis(size_t axis, bool b, const char* axisLetter = nullptr);

	extern void UpdateAxisPosition(size_t axis, float fval);

	extern void UpdateGeometry(unsigned int p_numAxes, bool p_isDelta);

	extern void SetBabystepOffset(size_t index, float f);
	extern void SetAxisLetter(size_t index, char l);
	extern void SetAxisVisible(size_t index, bool v);
	extern void SetAxisWorkplaceOffset(size_t axisIndex, size_t workplaceIndex, float offset);
	extern void SetCurrentWorkplaceNumber(uint8_t workplaceNumber);
}

#endif /* JNI_LOGIC_UI_USERINTERFACE_HPP_ */
