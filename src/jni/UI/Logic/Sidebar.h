/*
 * Sidebar.h
 *
 *  Created on: 24 April 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_UI_LOGIC_SIDEBAR_H_
#define JNI_UI_LOGIC_SIDEBAR_H_

#include "UI/UserInterface.h"

namespace UI::Sidebar
{
	void Init();

	void OpenMacros();
	void OpenConsole();
	void EStop();

	void UpdateTemperatureSnapshot();
} // namespace UI::Sidebar

#endif /* JNI_UI_LOGIC_SIDEBAR_H_ */