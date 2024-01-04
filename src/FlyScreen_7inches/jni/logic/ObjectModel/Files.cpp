/*
 * Files.cpp
 *
 *  Created on: 3 Jan 2024
 *      Author: Andy Everitt
 */

#define DEBUG (1)
#include "Debug.hpp"
#include "Files.hpp"
#include <sstream>

namespace OM
{
	static Folder root("/");
	static std::string sCurrentDirPath;
	static Folder* spCurrentFolder = &root;

	std::string FileSystemItem::GetPath()
	{
		FileSystemItem* item = this;
		std::string path;
		bool lastItem = true;
		while (item->pParent_ != nullptr)
		{
			if (!lastItem)
			{
				path.insert(0, "/");
			}
			lastItem = false;
			path.insert(0, item->GetName());
			item = item->pParent_;
		}
		return path;
	}

	File* Folder::GetFile(const std::string& name)
	{
		for (File& file : files_)
		{
			if (file.GetName() == name)
				return &file;
		}
		return nullptr
	}

	Folder* Folder::GetSubFolder(const std::string& name)
	{
		for (Folder& folder : folders_)
		{
			if (folder.GetName() == name)
				return &folder;
		}
		return nullptr;
	}


	File* Folder::GetOrCreateFile(const std::string& name)
	{
		File* file = GetFile(name);
		if (file != nullptr)
			return file;
		return AddFile(name);
	}

	Folder* Folder::GetOrCreateSubFolder(const std::string& name)
	{
		Folder* folder= GetSubFolder(name);
		if (folder != nullptr)
			return folder;
		return AddFolder(name);
	}

	void SetCurrentDir(const std::string& path)
	{
		std::istringstream stream(path);
		std::string folderName;

		sCurrentDirPath = path;
		while (std::getline(stream, folderName, '/'))
		{
			if (folderName.empty())
				break;
//			if (folderName.find(':') != std::string::npos)
//				continue;

			dbg("Files: folderName=%s", folderName.c_str());

			Folder* subFolder = spCurrentFolder->GetSubFolder(folderName);
			if (subFolder == nullptr)
			{
				subFolder = spCurrentFolder->AddFolder(folderName);
			}
			spCurrentFolder = subFolder;
			dbg("Files: current subfolder path = %s", spCurrentFolder->GetPath().c_str());
		}
	}

	std::string& GetCurrentDirPath()
	{
		return sCurrentDirPath;
	}

	Folder* GetCurrentFolder()
	{
		return spCurrentFolder;
	}

} /* namespace OM */

