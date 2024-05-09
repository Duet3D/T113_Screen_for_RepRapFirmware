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
	static ConsoleMode s_consoleMode = ConsoleMode::Duet;

	size_t GetConsoleListCount()
	{
		return UI::CONSOLE.GetItemCount();
	}

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

	void SetConsoleMode(const ConsoleMode mode)
	{
		info("Setting console mode to %d", (int)mode);
		switch (mode)
		{
		case ConsoleMode::Duet:
			UI::GetUIControl<ZKTextView>(ID_MAIN_ConsoleHeader)->setTextTr("console");
			break;
		case ConsoleMode::System:
			UI::GetUIControl<ZKTextView>(ID_MAIN_ConsoleHeader)->setTextTr("console_system");
			break;
		default:
			break;
		}
		s_consoleMode = mode;
	}

	void ConsoleInputCallback(const std::string& input)
	{
		if (input.empty())
		{
			return;
		}
		SendConsoleInput();
	}

	void SendConsoleInput()
	{
		static ZKEditText* s_consoleInput = UI::GetUIControl<ZKEditText>(ID_MAIN_ConsoleInput);
		UI::CONSOLE.AddCommand(s_consoleInput->getText());
		switch (s_consoleMode)
		{
		case ConsoleMode::Duet:
		{
			Comm::DUET.SendGcode(s_consoleInput->getText().c_str());
			break;
		}
		case ConsoleMode::System:
		{
			std::string response = utils::exec(s_consoleInput->getText().c_str());
			for (auto& line : utils::splitString(response, "\n"))
			{
				UI::CONSOLE.AddResponse(line.c_str());
			}
			break;
		}
		default:
			error("Unknown console mode: %d", (int)s_consoleMode);
			break;
		}
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
