/*
 * DebugCommands.h
 *
 *  Created on: 14 Mar 2024
 *      Author: andy
 */

#ifndef JNI_DEBUGCOMMANDS_H_
#define JNI_DEBUGCOMMANDS_H_

#include "std_fixed/functional.h"

namespace Debug
{
	struct DebugCommand
	{
		const char* id;
		function<void(void)> callback;

		DebugCommand(const char* id, function<void(void)> callback);
	};

	void CreateCommand(const char* id, function<void(void)> callback);
	DebugCommand* GetCommand(const char* id);
	DebugCommand* GetCommandByIndex(size_t index);
	size_t GetCommandCount();
} // namespace Debug

#endif /* JNI_DEBUGCOMMANDS_H_ */
