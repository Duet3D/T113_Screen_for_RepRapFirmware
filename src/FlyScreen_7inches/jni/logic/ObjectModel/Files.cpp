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
	static Folder sRootFolder("/");
	static std::string sCurrentDirPath;
	static Folder* spCurrentFolder = &sRootFolder;

	std::string FileSystemItem::GetPath() const
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

	FileSystemItem::~FileSystemItem()
	{
		dbg("Files: destructing file %s", GetPath().c_str());
	}

	File* Folder::GetFile(const std::string& name)
	{
		for (FileSystemItem* item : items_)
		{
			if (item == nullptr)
				continue;
			if (item->GetName() != name)
				continue;
			if (item->GetType() != FileSystemItemType::file)
				continue;
			return (File*)item;
		}
		return nullptr;
	}

	Folder* Folder::GetSubFolder(const std::string& name)
	{
		for (FileSystemItem* item : items_)
		{
			if (item == nullptr)
				continue;
			if (item->GetName() != name)
				continue;
			if (item->GetType() != FileSystemItemType::folder)
				continue;
			return (Folder*)item;
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

	Folder* GetRootFolder()
	{
		return &sRootFolder;
	}

	void SetCurrentDir(const std::string& path)
	{
		std::istringstream stream(path);
		std::string folderName;

//		sRootFolder.Clear();
		spCurrentFolder = &sRootFolder;

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
			spCurrentFolder->Clear();
			dbg("Files: here");
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

