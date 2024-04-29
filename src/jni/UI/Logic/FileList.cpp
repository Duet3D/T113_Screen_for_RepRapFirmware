/*
 * FileList.cpp
 *
 *  Created on: 23 Apr 2024
 *      Author: andy
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "FileList.h"
#include <ObjectModel/Files.h>
#include <UI/Popup.h>
#include <activity/mainActivity.h>
#include <comm/FileInfo.h>
#include <comm/Thumbnail.h>
#include <control/ZKListView.h>
#include <control/ZKTextView.h>
#include <manager/LanguageManager.h>
#include <string>
#include <upgrade/Upgrade.h>
#include <window/ZKWindow.h>

namespace UI::FileList
{
	static ZKTextView* s_folderId = nullptr;
	static ZKListView* s_listView = nullptr;
	static ZKTextView* s_thumbnail;
	static const OM::FileSystem::File* s_selectedFile;

	void Init()
	{
		info("Initialising FileList UI...");
		s_folderId = UI::GetUIControl<ZKTextView>(ID_MAIN_FolderID);
		s_listView = UI::GetUIControl<ZKListView>(ID_MAIN_FileListView);
		s_thumbnail = UI::GetUIControl<ZKTextView>(ID_MAIN_PopupImage);
	}

	void SetSelectedFile(const OM::FileSystem::File* file)
	{
		s_selectedFile = file;
	}

	const OM::FileSystem::File* GetSelectedFile()
	{
		return s_selectedFile;
	}

	static void RunSelectedFile()
	{
		return OM::FileSystem::RunFile(s_selectedFile);
	}

	ZKTextView* GetThumbnail()
	{
		return s_thumbnail;
	}

	void ReloadFileList()
	{
		UI::POPUP_WINDOW.Close();
		FILEINFO_CACHE->ClearCache();
		OM::FileSystem::ClearFileSystem();
		OM::FileSystem::RequestFiles(OM::FileSystem::GetCurrentDirPath());
	}

	size_t GetFileListCount()
	{
		return OM::FileSystem::GetItemCount();
	}

	void SetFileListItem(ZKListView::ZKListItem* pListItem, int index)
	{
		ZKListView::ZKListSubItem* pFileName = pListItem->findSubItemByID(ID_MAIN_FileNameSubItem);
		ZKListView::ZKListSubItem* pFileType = pListItem->findSubItemByID(ID_MAIN_FileTypeSubItem);
		ZKListView::ZKListSubItem* pFileSize = pListItem->findSubItemByID(ID_MAIN_FileSizeSubItem);
		ZKListView::ZKListSubItem* pFileDate = pListItem->findSubItemByID(ID_MAIN_FileDateSubItem);
		ZKListView::ZKListSubItem* pFileThumbnail = pListItem->findSubItemByID(ID_MAIN_FileThumbnailSubItem);

		OM::FileSystem::FileSystemItem* item = OM::FileSystem::GetItem(index);
		if (item == nullptr)
			return;
		//	dbg("Files: settings list item %d name to %s", index, item->GetName().c_str());
		pFileName->setText(item->GetName());
		switch (item->GetType())
		{
		case OM::FileSystem::FileSystemItemType::file: {
			pListItem->setSelected(false);
			pFileType->setTextTr("file");
			pFileSize->setTextTrf("file_size", item->GetReadableSize().c_str());
			if (IsThumbnailCached(item->GetPath().c_str()))
			{
				SetThumbnail(pFileThumbnail, item->GetPath().c_str());
			}
			else
			{
				pFileThumbnail->setBackgroundPic("");
			}
			break;
		}
		case OM::FileSystem::FileSystemItemType::folder:
			pListItem->setSelected(true);
			pFileType->setTextTr("folder");
			pFileSize->setText("");
			pFileThumbnail->setBackgroundPic("");

			pFileName->setSelected(true);
			pFileType->setSelected(true);
			pFileSize->setSelected(true);
			pFileDate->setSelected(true);
			pFileThumbnail->setSelected(true);
			break;
		}
		pFileDate->setTextTrf("file_date", item->GetDate().c_str());
	}

	void FileListItemCallback(int index)
	{
		OM::FileSystem::FileSystemItem* item = OM::FileSystem::GetItem(index);
		if (UI::POPUP_WINDOW.IsBlocking())
			return;
		UI::POPUP_WINDOW.Cancel();
		switch (item->GetType())
		{
		case OM::FileSystem::FileSystemItemType::file: {
			OM::FileSystem::File* file = (OM::FileSystem::File*)item;
			SetSelectedFile(file);
			if (OM::FileSystem::IsMacroFolder())
			{
				UI::POPUP_WINDOW.Open([]() { RunSelectedFile(); });
				UI::POPUP_WINDOW.SetTextf(LANGUAGEMANAGER->getValue("run_macro").c_str(), item->GetName().c_str());
			}
			else if (OM::FileSystem::IsUsbFolder())
			{
				std::string ext = OM::FileSystem::GetFileExtension(file->GetName());
				if (ext == "img")
				{
					UI::POPUP_WINDOW.Open([]() { UpgradeFromUSB(GetSelectedFile()->GetPath()); });
					UI::POPUP_WINDOW.SetTextf(LANGUAGEMANAGER->getValue("upgrade_from").c_str(),
											  item->GetName().c_str());
				}
				else
				{
					UI::POPUP_WINDOW.Open([]() { OM::FileSystem::UploadFile(GetSelectedFile()); });
					UI::POPUP_WINDOW.SetTitle(LANGUAGEMANAGER->getValue("upload_file").c_str());
					UI::POPUP_WINDOW.SetText(item->GetName().c_str());
					UI::POPUP_WINDOW.SetTextScrollable(false);
				}
			}
			else
			{
				FILEINFO_CACHE->QueueLargeThumbnailRequest(item->GetPath());
				UI::POPUP_WINDOW.Open([]() {
					RunSelectedFile();
					UI::WINDOW.OpenWindow(UI::GetUIControl<ZKWindow>(ID_MAIN_PrintWindow));
				});
				SetPopupFileInfo();
			}
			UI::POPUP_WINDOW.CancelTimeout();
			break;
		}
		case OM::FileSystem::FileSystemItemType::folder:
			if (OM::FileSystem::IsUsbFolder())
			{
				OM::FileSystem::RequestUsbFiles(item->GetPath());
				break;
			}
			OM::FileSystem::RequestFiles(item->GetPath());
			break;
		}
	}

	void SetPopupFileInfo()
	{
		const OM::FileSystem::File* item = GetSelectedFile();
		if (item == nullptr)
			return;

		Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetFileInfo(item->GetPath());
		info("Setting file info popup for \"%s\"", item->GetPath().c_str());

		float height = 0;
		float layerHeight = 0;
		tm printTime;
		printTime.tm_hour = printTime.tm_min = printTime.tm_sec = 0;
		std::string generatedBy = "";
		if (fileInfo != nullptr)
		{
			height = fileInfo->height;
			layerHeight = fileInfo->layerHeight;
			printTime = fileInfo->GetPrintTime();
			generatedBy = fileInfo->generatedBy.c_str();
		}
		UI::POPUP_WINDOW.SetTitle(item->GetName());
		UI::POPUP_WINDOW.SetTextf(LANGUAGEMANAGER->getValue("file_info").c_str(),
								  height,
								  layerHeight,
								  printTime.tm_hour,
								  printTime.tm_min,
								  printTime.tm_sec,
								  generatedBy.c_str());
		UI::POPUP_WINDOW.SetTextScrollable(false);
		if (IsThumbnailCached(item->GetPath().c_str()) || IsThumbnailCached(Comm::largeThumbnailFilename))
		{
			UI::POPUP_WINDOW.SetImage(GetThumbnailPath(item->GetPath().c_str()).c_str());
		}
		else
		{
			UI::POPUP_WINDOW.ShowImage(false);
		}
		UI::POPUP_WINDOW.SetOkBtnText(LANGUAGEMANAGER->getValue("start_print").c_str());
		UI::POPUP_WINDOW.CancelTimeout();
	}

	void RequestUSBFiles()
	{
		OM::FileSystem::RequestUsbFiles("");
	}

	void RefreshFileList()
	{
		s_listView->refreshListView();
		s_listView->setSelection(0);
		s_folderId->setText(LANGUAGEMANAGER->getValue(OM::FileSystem::IsUsbFolder() ? "usb" : "folder") + ": " +
							OM::FileSystem::GetCurrentDirPath());
	}
} // namespace UI::FileList
