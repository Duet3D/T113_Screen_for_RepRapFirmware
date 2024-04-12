/*
 * Files.cpp
 *
 *  Created on: 3 Jan 2024
 *      Author: Andy Everitt
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "Files.h"

#include "Comm/Communication.h"
#include "Hardware/Duet.h"
#include "Hardware/Usb.h"
#include "UI/UserInterface.h"
#include <algorithm>

namespace OM::FileSystem
{
	static std::string sCurrentDirPath;
	static std::vector<FileSystemItem*> sItems;
	static bool sInMacroFolder = false;
	static bool sUsbFolder = false;
	static ZKTextView* sFolderId = nullptr;
	static ZKListView* sListView = nullptr;

	std::string FileSystemItem::GetPath() const
	{
		if (sCurrentDirPath.empty())
			return name_;
		return sCurrentDirPath + "/" + name_;
	}

	std::string FileSystemItem::GetReadableSize() const
	{
		const char* sizes[] = {"B", "KB", "MB", "GB", "TB"};
		int order = 0;

		if (size_ == 0)
			return "0 B";

		double len = double(size_);
		while (len >= 1024 && order < 4)
		{
			order++;
			len /= 1024;
		}

		return utils::format("%.2f %s", len, sizes[order]);
	}

	void FileSystemItem::SetName(const std::string name)
	{
		name_ = name.c_str();
#if DEBUG_LEVEL > 2
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

	void Init(ZKTextView* folderId, ZKListView* listView)
	{
		sFolderId = folderId;
		sListView = listView;
	}

	ZKListView* GetListView()
	{
		return sListView;
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

	const std::vector<FileSystemItem*>& GetItems()
	{
		return sItems;
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
		sCurrentDirPath = path;
		info("Files: current directory = %s", sCurrentDirPath.c_str());
	}

	struct
	{
		bool operator()(FileSystemItem* L, FileSystemItem* R)
		{
			if (L->GetType() == R->GetType())
				return L->GetDate() > R->GetDate();
			return L->GetType() < R->GetType();
		}
	} SortItem;

	void SortFileSystem()
	{
		auto first = sItems.begin();
		auto last = sItems.end();
		FileSystemItem** temp;
		--last;
		while (last - first > 0)
		{
			temp = last;
			while (temp != first)
			{
				if (SortItem(*temp, *(temp - 1)))
				{
					std::iter_swap(temp - 1, temp);
				}
				--temp;
			}
			++first;
		}
	}

	std::string GetParentDirPath()
	{
		if (!IsInSubFolder())
			return sCurrentDirPath;

		std::string path;
		size_t i = sCurrentDirPath.find_last_of('/');

		if (i == std::string::npos)
			return "";
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
		if (IsUsbFolder())
		{
			return sCurrentDirPath.empty() ? false : true;
		}

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
		ClearFileSystem();
		sUsbFolder = false;
		sInMacroFolder = path.find("macro") != std::string::npos;
		Comm::duet.RequestFileList(path.c_str());
	}

	void RequestUsbFiles(const std::string& path)
	{
		ClearFileSystem();
		sUsbFolder = true;
		sCurrentDirPath = path;
		std::vector<USB::FileInfo> files = USB::ListEntriesInDirectory(std::string("/mnt/usb1/") + path);
		size_t index = 0;
		for (auto& fileInfo : files)
		{
			if (fileInfo.d_type == DT_DIR)
			{
				Folder* folder = AddFolderAt(index);
				folder->SetName(fileInfo.d_name);
			}
			else if (fileInfo.d_type == DT_REG)
			{
				File* file = AddFileAt(index);
				file->SetName(fileInfo.d_name);
				file->SetSize(fileInfo.st_size);
				file->SetDate(utils::format("%d", fileInfo.st_mtim));
			}
			index++;
		}
		sFolderId->setText(LANGUAGEMANAGER->getValue("usb") + ": " + OM::FileSystem::GetCurrentDirPath());
		sListView->refreshListView();
	}

	bool IsMacroFolder()
	{
		return sInMacroFolder;
	}

	bool IsUsbFolder()
	{
		return sUsbFolder;
	}

	void RunFile(const File* file)
	{
		if (sInMacroFolder)
			RunMacro(file->GetPath());
		else
			StartPrint(file->GetPath());
	}

	void RunMacro(const std::string& path)
	{
		Comm::duet.SendGcodef("M98 P\"%s\"\n", path.c_str());
	}

	void UploadFile(const File* file)
	{
		std::string contents;
		if (!USB::ReadUsbFileContents(file->GetPath(), contents))
			return;
		Comm::duet.UploadFile(utils::format("/gcodes/%s", file->GetName().c_str()).c_str(), contents);
	}

	void StartPrint(const std::string& path)
	{
		Comm::duet.SendGcodef("M23 \"%s\"\n", path.c_str());
		Comm::duet.SendGcode("M24\n");
	}

	void ResumePrint()
	{
		Comm::duet.SendGcode("M24\n");
	}

	void PausePrint()
	{
		Comm::duet.SendGcode("M25\n");
	}

	void StopPrint()
	{
		Comm::duet.SendGcode("M0\n");
	}

	void ClearFileSystem()
	{
		info("Files: clearing items");
		for (auto item : sItems)
		{
			if (item == nullptr)
				continue;
			dbg("Files: deleting item %s", item->GetName().c_str());
			delete item;
		}
		sItems.clear();
		UI::SetSelectedFile(nullptr);
	}

	std::string GetFileExtension(const std::string& filename)
	{
		size_t dot = filename.find_last_of('.');
		if (dot != std::string::npos)
		{
			return filename.substr(dot + 1);
		}
		return "";
	}
} // namespace OM::FileSystem
