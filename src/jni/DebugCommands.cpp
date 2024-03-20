#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "DebugCommands.h"
#include "UI/UserInterface.h"
#include <map>

namespace Debug
{
	static std::map<const char*, DebugCommand*> commandsMap;

	void CreateCommand(const char* id, function<void(void)> callback)
	{
		new DebugCommand(id, callback);
	}

	DebugCommand::DebugCommand(const char* id, function<void(void)> callback) : id(id), callback(callback)
	{
		commandsMap[id] = this;
	}

	DebugCommand* GetCommand(const char* id)
	{
		if (commandsMap.find(id) == commandsMap.end())
		{
			warn("id %s not found", id);
			return nullptr;
		}
		return commandsMap[id];
	}

	DebugCommand* GetCommandByIndex(size_t index)
	{
		if (index >= commandsMap.size())
		{
			warn("Index out of range");
			return nullptr;
		}
		auto it = commandsMap.begin();
		for (size_t i = 0; i < index; i++)
		{
			it++;
		}
		return it->second;
	}

	size_t GetCommandCount()
	{
		return commandsMap.size();
	}
} // namespace Debug
