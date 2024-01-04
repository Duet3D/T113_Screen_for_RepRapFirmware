/*
 * Files.cpp
 *
 *  Created on: 3 Jan 2024
 *      Author: Andy Everitt
 */

#define DEBUG (1)
#include "Debug.hpp"
#include "Files.hpp"
#include "Hardware/SerialIo.hpp"

namespace OM
{
	static std::string sCurrentDirPath;
	static std::vector<FileSystemItem*> sItems;
	static bool sInMacroFolder = false;

	std::string FileSystemItem::GetPath() const
	{
		return sCurrentDirPath + "/" + name_;
	}

	void FileSystemItem::SetName(const std::string name)
	{
		name_ = name.c_str();
#ifdef DEBUG
		switch (type_)
		{
		case FileSystemItemType::file:
			dbg("Files: set file name to %s", name_.c_str());
			break;
		case FileSystemItemType::folder:
			dbg("Files: set folder name to %s", name_.c_str());
			break;
		}
#endif
	}

	FileSystemItem::~FileSystemItem()
	{
		dbg("Files: destructing item %s", GetPath().c_str());
	}

	File* AddFileAt(const size_t index)
	{
		if (index < sItems.size())
		{
			dbg("Deleting item[%d]", index);
			delete sItems[index];
			sItems[index] = nullptr;
		}
		File* file = new File();
		sItems.insert(sItems.begin() + index, file);
		return file;
	}

	Folder* AddFolderAt(const size_t index)
	{
		if (index < sItems.size())
		{
			dbg("Deleting item[%d]", index);
			delete sItems[index];
			sItems[index] = nullptr;
		}
		Folder* folder = new Folder();
		sItems.insert(sItems.begin() + index, folder);
		return folder;
	}

	const size_t GetItemCount()
	{
		return sItems.size();
	}

	FileSystemItem* GetItem(const size_t index)
	{
		if (index >= GetItemCount())
			return nullptr;
		return sItems[index];
	}

	File* GetFile(const std::string& name)
	{
		for (FileSystemItem* item : sItems)
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

	Folder* GetSubFolder(const std::string& name)
	{
		for (FileSystemItem* item : sItems)
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

	void SetCurrentDir(const std::string& path)
	{
		ClearFileSystem();
		sCurrentDirPath = path;
		dbg("Files: current directory = %s", sCurrentDirPath.c_str());
	}

	std::string GetParentDirPath()
	{
		std::string path;
		size_t i = sCurrentDirPath.find_last_of('/');

		if (i == std::string::npos)
			return sCurrentDirPath;
		return sCurrentDirPath.substr(0, i);
	}

	std::string GetCurrentDirName()
	{
		std::string path;
		size_t i = sCurrentDirPath.find_last_of('/');

		if (i == std::string::npos)
			return sCurrentDirPath;
		return sCurrentDirPath.substr(i, sCurrentDirPath.size() - i);
	}

	std::string& GetCurrentDirPath()
	{
		return sCurrentDirPath;
	}

	bool IsInSubFolder()
	{
		size_t count = 0;
		for (auto c : sCurrentDirPath)
		{
			if (c == '/')
				count++;
		}
		dbg("Files: %d", count);
		return count > 1;
	}

	void RequestFiles(const std::string& path)
	{
		sInMacroFolder = path.find("macro") != std::string::npos;
		SerialIo::Sendf("M20 S3 P\"%s\"\n", path.c_str());
	}

	void RunFile(const std::string& path)
	{
		if (sInMacroFolder)
			RunMacro(path);
		else
			StartPrint(path);
	}

	void RunMacro(const std::string& path)
	{
		SerialIo::Sendf("M98 P\"%s\"\n", path.c_str());
	}

	void StartPrint(const std::string& path)
	{
		SerialIo::Sendf("M23 \"%s\"\n", path.c_str());
		SerialIo::Sendf("M24\n");
	}

	void ResumePrint()
	{
		SerialIo::Sendf("M24\n");
	}

	void PausePrint()
	{
		SerialIo::Sendf("M25\n");
	}

	void StopPrint()
	{
		SerialIo::Sendf("M0\n");
	}

	void ClearFileSystem()
	{
		dbg("Files: clearing items");
		for (auto item : sItems)
		{
			if (item == nullptr)
				continue;
			dbg("Files: deleting item %s", item->GetName().c_str());
			delete item;
		}
		sItems.clear();
	}


} /* namespace OM */

