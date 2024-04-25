/*
 * Console.cpp
 *
 *  Created on: 24 April 2024
 *      Author: Andy Everitt
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "Console.h"
#include "DebugCommands.h"
#include "Hardware/Duet.h"
#include "UI/Gcodes.h"

namespace UI::ConsoleWindow
{
	void SetConsoleListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		pListItem->setText(UI::CONSOLE.GetItem(index).c_str());
	}

	size_t GetGcodeListCount()
	{
		return sizeof(s_gcode) / sizeof(gcode);
	}

	void SetGcodeListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		pListItem->setTextTr(s_gcode[index].displayText);
	}

	void ConsoleInputCallback(const std::string& input)
	{
		UI::CONSOLE.AddCommand(input);
		Comm::DUET.SendGcode(input.c_str());
	}

	void SendConsoleInput()
	{
		static ZKEditText* s_consoleInput = UI::GetUIControl<ZKEditText>(ID_MAIN_ConsoleInput);
		UI::CONSOLE.AddCommand(s_consoleInput->getText());
		Comm::DUET.SendGcode(s_consoleInput->getText().c_str());
	}

	void ClearConsole()
	{
		UI::CONSOLE.Clear();
	}

	void OpenDebugCommands()
	{
		UI::WINDOW.OpenOverlay(ID_MAIN_DebugWindow);
	}

	size_t GetDebugCommandsCount()
	{
		return Debug::GetCommandCount();
	}

	void SetDebugCommandsListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		Debug::DebugCommand* command = Debug::GetCommandByIndex(index);
		if (command == nullptr)
		{
			pListItem->setText("");
			return;
		}
		pListItem->setTextTr(command->id);
	}

	void DebugCommandCallback(const int index)
	{
		Debug::DebugCommand* command = Debug::GetCommandByIndex(index);
		if (command == nullptr)
		{
			return;
		}
		command->callback();
	}

} // namespace UI::ConsoleWindow
