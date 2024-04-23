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
	static std::string s_currentDirPath;
	static std::vector<FileSystemItem*> s_items;
	static bool s_inMacroFolder = false;
	static bool s_usbFolder = false;
	static ZKTextView* s_folderId = nullptr;
	static ZKListView* s_listView = nullptr;

	std::string FileSystemItem::GetPath() const
	{
		if (s_currentDirPath.empty())
			return m_name;
		return s_currentDirPath + "/" + m_name;
	}

	std::string FileSystemItem::GetReadableSize() const
	{
		const char* sizes[] = {"B", "KB", "MB", "GB", "TB"};
		int order = 0;

		if (m_size == 0)
			return "0 B";

		double len = double(m_size);
		while (len >= 1024 && order < 4)
		{
			order++;
			len /= 1024;
		}

		return utils::format("%.2f %s", len, sizes[order]);
	}

	void FileSystemItem::SetName(const std::string name)
	{
		m_name = name.c_str();
		switch (m_type)
		{
		case FileSystemItemType::file:
			verbose("Files: set file name to %s", m_name.c_str());
			break;
		case FileSystemItemType::folder:
			verbose("Files: set folder name to %s", m_name.c_str());
			break;
		}
	}

	FileSystemItem::~FileSystemItem()
	{
		dbg("Files: destructing item %s", GetPath().c_str());
	}

	void Init(ZKTextView* folderId, ZKListView* listView)
	{
		s_folderId = folderId;
		s_listView = listView;
	}

	ZKListView* GetListView()
	{
		return s_listView;
	}

	File* AddFileAt(const size_t index)
	{
		if (index < s_items.size())
		{
			dbg("Deleting item[%d]", index);
			delete s_items[index];
			s_items[index] = nullptr;
		}
		File* file = new File();
		s_items.insert(s_items.begin() + index, file);
		return file;
	}

	Folder* AddFolderAt(const size_t index)
	{
		if (index < s_items.size())
		{
			dbg("Deleting item[%d]", index);
			delete s_items[index];
			s_items[index] = nullptr;
		}
		Folder* folder = new Folder();
		s_items.insert(s_items.begin() + index, folder);
		return folder;
	}

	const size_t GetItemCount()
	{
		return s_items.size();
	}

	const std::vector<FileSystemItem*>& GetItems()
	{
		return s_items;
	}

	FileSystemItem* GetItem(const size_t index)
	{
		if (index >= GetItemCount())
			return nullptr;
		return s_items[index];
	}

	File* GetFile(const std::string& name)
	{
		for (FileSystemItem* item : s_items)
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
		for (FileSystemItem* item : s_items)
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
		s_currentDirPath = path;
		info("Files: current directory = %s", s_currentDirPath.c_str());
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
		auto first = s_items.begin();
		auto last = s_items.end();
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
			return s_currentDirPath;

		std::string path;
		size_t i = s_currentDirPath.find_last_of('/');

		if (i == std::string::npos)
			return "";
		return s_currentDirPath.substr(0, i);
	}

	std::string GetCurrentDirName()
	{
		std::string path;
		size_t i = s_currentDirPath.find_last_of('/');

		if (i == std::string::npos)
			return s_currentDirPath;
		return s_currentDirPath.substr(i, s_currentDirPath.size() - i);
	}

	std::string& GetCurrentDirPath()
	{
		return s_currentDirPath;
	}

	bool IsInSubFolder()
	{
		if (IsUsbFolder())
		{
			return s_currentDirPath.empty() ? false : true;
		}

		size_t count = 0;
		for (auto c : s_currentDirPath)
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
		s_usbFolder = false;
		s_inMacroFolder = path.find("macro") != std::string::npos;
		Comm::DUET.RequestFileList(path.c_str());
	}

	void RequestUsbFiles(const std::string& path)
	{
		ClearFileSystem();
		s_usbFolder = true;
		s_currentDirPath = path;
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
		s_folderId->setText(LANGUAGEMANAGER->getValue("usb") + ": " + OM::FileSystem::GetCurrentDirPath());
		s_listView->refreshListView();
	}

	bool IsMacroFolder()
	{
		return s_inMacroFolder;
	}

	bool IsUsbFolder()
	{
		return s_usbFolder;
	}

	void RunFile(const File* file)
	{
		if (s_inMacroFolder)
			RunMacro(file->GetPath());
		else
			StartPrint(file->GetPath());
	}

	void RunMacro(const std::string& path)
	{
		Comm::DUET.SendGcodef("M98 P\"%s\"\n", path.c_str());
	}

	void UploadFile(const File* file)
	{
		std::string contents;
		if (!USB::ReadUsbFileContents(file->GetPath(), contents))
			return;
		Comm::DUET.UploadFile(utils::format("/gcodes/%s", file->GetName().c_str()).c_str(), contents);
	}

	void StartPrint(const std::string& path)
	{
		Comm::DUET.SendGcodef("M23 \"%s\"\n", path.c_str());
		Comm::DUET.SendGcode("M24\n");
	}

	void ResumePrint()
	{
		Comm::DUET.SendGcode("M24\n");
	}

	void PausePrint()
	{
		Comm::DUET.SendGcode("M25\n");
	}

	void StopPrint()
	{
		Comm::DUET.SendGcode("M0\n");
	}

	void ClearFileSystem()
	{
		info("Files: clearing items");
		for (auto item : s_items)
		{
			if (item == nullptr)
				continue;
			dbg("Files: deleting item %s", item->GetName().c_str());
			delete item;
		}
		s_items.clear();
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
