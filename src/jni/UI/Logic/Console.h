/*
 * Console.h
 *
 *  Created on: 24 April 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_UI_LOGIC_CONSOLE_H_
#define JNI_UI_LOGIC_CONSOLE_H_

#include "UI/UserInterface.h"

namespace UI::ConsoleWindow
{
	void SetConsoleListItem(ZKListView::ZKListItem* pListItem, const int index);

	size_t GetGcodeListCount();
	void SetGcodeListItem(ZKListView::ZKListItem* pListItem, const int index);

	void ConsoleInputCallback(const std::string& input);
	void SendConsoleInput();
	void ClearConsole();

	void OpenDebugCommands();
	size_t GetDebugCommandsCount();
	void SetDebugCommandsListItem(ZKListView::ZKListItem* pListItem, const int index);
	void DebugCommandCallback(const int index);
} // namespace UI::ConsoleWindow

#endif /* JNI_UI_LOGIC_CONSOLE_H_ */