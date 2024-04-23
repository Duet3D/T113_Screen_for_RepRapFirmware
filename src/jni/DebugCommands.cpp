#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "UI/UserInterface.h"

#include "DebugCommands.h"
#include "Hardware/Duet.h"
#include "Hardware/Usb.h"
#include <map>

namespace Debug
{
	static std::map<const char*, DebugCommand*> commandsMap;

	static DebugCommand s_verboseLog("dbg_verbose_log", []() {
		// Create a log file and send it to the Duet
		system("logcat -v threadtime -d *:V > /tmp/DuetScreen_log.txt");
		std::string logs;
		USB::ReadFileContents("/tmp/DuetScreen_log.txt", logs);
		Comm::DUET.UploadFile("/sys/DuetScreen_log.txt", logs);
		system("rm /tmp/DuetScreen_log.txt");
	});

	static DebugCommand s_errorLog("dbg_error_log", []() {
		// Create a log file and send it to the Duet
		system("logcat -v threadtime -d *:W > /tmp/DuetScreen_log.txt");
		std::string logs;
		USB::ReadFileContents("/tmp/DuetScreen_log.txt", logs);
		Comm::DUET.UploadFile("/sys/DuetScreen_error_log.txt", logs);
		system("rm /tmp/DuetScreen_log.txt");
	});

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
