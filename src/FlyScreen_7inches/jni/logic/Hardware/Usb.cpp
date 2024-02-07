/*
 * Usb.cpp
 *
 *  Created on: 6 Feb 2024
 *      Author: andy
 */

#define DEBUG (1)

#include "Usb.h"
#include "UI/UserInterface.h"
#include "utils.h"
#include <fstream>

namespace USB
{
	std::vector<dirent> ListEntriesInDirectory(const std::string& directoryPath)
	{
		std::vector<dirent> dirents;

		// Open the directory
		DIR* dir = opendir(directoryPath.c_str());
		if (dir == nullptr)
		{
			dbg("Error opening directory %s", directoryPath.c_str());
			return dirents;
		}

		// Read the directory entries
		dirent* entry;
		while ((entry = readdir(dir)) != nullptr)
		{
			std::string name = entry->d_name;
			if (name == "." || name == ".." || name == "System Volume Information")
			{
				continue;
			}

			// Add files to the vector
			if (entry->d_type == DT_REG || entry->d_type == DT_DIR)
			{ // Regular file or folder
				dirents.push_back(*entry);
			}
		}

		// Close the directory
		closedir(dir);

		return dirents;
	}

	bool ReadFileContents(const std::string& filePath, std::string& contents)
	{
		std::string fullPath = std::string("/mnt/usb1/") + filePath;

		dbg("Reading file %s", fullPath.c_str());
		std::ifstream file(fullPath.c_str(), std::ios::in | std::ios::ate);

		if (!file.is_open())
		{
			UI::CONSOLE->AddResponse(utils::format("Unable to open file %s", fullPath.c_str()).c_str());
			dbg("Unable to open file %s", fullPath.c_str());
			return false;
		}

		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		contents.resize(size);
		dbg("Reading %d bytes", size);
		if (!file.read(contents.begin(), size))
		{
			UI::CONSOLE->AddResponse(utils::format("Failed to read file %s", fullPath.c_str()).c_str());
			dbg("Failed to read file %s", fullPath.c_str());
			return false;
		}
//		dbg("Read %d bytes", buffer.size());
//		contents = buffer;
		dbg("Copied to string %d", contents.size());
		return true;
	}
} // namespace USB
