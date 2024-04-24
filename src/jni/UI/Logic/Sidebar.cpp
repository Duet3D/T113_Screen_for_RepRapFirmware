/*
 * Sidebar.cpp
 *
 *  Created on: 24 April 2024
 *      Author: Andy Everitt
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "UI/UserInterface.h"

#include "Hardware/Duet.h"
#include "ObjectModel/Files.h"
#include "Sidebar.h"

namespace UI::Sidebar
{
	void OpenMacros()
	{
		static ZKWindow* window = UI::GetUIControl<ZKWindow>(ID_MAIN_FilesWindow);

		info("Sidebar: Open Macros");
		OM::FileSystem::RequestFiles("0:/macros");
		UI::WINDOW.OpenWindow(window);
	}

	void OpenConsole()
	{
		static ZKWindow* window = UI::GetUIControl<ZKWindow>(ID_MAIN_ConsoleWindow);

		info("Sidebar: Open Console");
		UI::WINDOW.OpenWindow(window);
	}

	void EStop()
	{
		warn("EStop Pressed!");
		UI::POPUP_WINDOW.Close();
		Comm::DUET.SendGcode("M112 ;"
							 "\xF0"
							 "\x0F");
		warn("Emergency Stop sent to Duet");
		Thread::sleep(1000);
		Comm::DUET.SendGcode("M999");
		warn("Restart sent to Duet");
	}
} // namespace UI::Sidebar
