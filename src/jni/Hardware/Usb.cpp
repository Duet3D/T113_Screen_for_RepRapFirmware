/*
 * Usb.cpp
 *
 *  Created on: 6 Feb 2024
 *      Author: andy
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG

#include "UI/UserInterface.h"
#include "Usb.h"
#include "sys/stat.h"
#include "utils/utils.h"
#include <fstream>

namespace USB
{
	std::vector<FileInfo> ListEntriesInDirectory(const std::string& directoryPath)
	{
		std::vector<FileInfo> files;

		// Open the directory
		DIR* dir = opendir(directoryPath.c_str());
		if (dir == nullptr)
		{
			error("Error opening directory %s", directoryPath.c_str());
			return files;
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
				FileInfo info;
				struct stat sb;
				if (stat((directoryPath + "/" + entry->d_name).c_str(), &sb) == -1)
				{
					error("Failed to get file stats for %s", (directoryPath + entry->d_name).c_str());
				}
				strncpy(info.d_name, entry->d_name, 256);
				info.d_type = entry->d_type;
				info.st_size = sb.st_size;
				info.st_blksize = sb.st_blksize;
				info.st_blocks = sb.st_blocks;
				info.st_atim = sb.st_atim;
				info.st_ctim = sb.st_ctim;
				info.st_mtim = sb.st_mtim;
				files.push_back(info);
			}
		}

		// Close the directory
		closedir(dir);

		return files;
	}

	bool ReadUsbFileContents(const std::string& filePath, std::string& contents)
	{
	    std::string fullPath;
	    if (filePath.rfind("/mnt/usb1") == 0)
	    {
	        fullPath = filePath;
	    }
	    else
	    {
	        fullPath = std::string("/mnt/usb1/") + filePath;
	    }

		return ReadFileContents(fullPath, contents);
	}

	bool ReadFileContents(const std::string& filePath, std::string& contents)
	{
		info("Reading file %s", filePath.c_str());
		std::ifstream file(filePath.c_str(), std::ios::in | std::ios::ate);

		if (!file.is_open())
		{
			UI::CONSOLE->AddResponse(utils::format("Unable to open file %s", filePath.c_str()).c_str());
			error("Unable to open file %s", filePath.c_str());
			return false;
		}

		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		contents.resize(size);
		dbg("Reading %ld bytes", size);
		if (!file.read(contents.begin(), size))
		{
			UI::CONSOLE->AddResponse(utils::format("Failed to read file %s", filePath.c_str()).c_str());
			error("Failed to read file %s", filePath.c_str());
			return false;
		}
		dbg("Read %d bytes", contents.size());
		return true;
	}
} // namespace USB
