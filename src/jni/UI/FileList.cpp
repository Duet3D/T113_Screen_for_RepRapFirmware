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
#include <activity/mainActivity.h>
#include <comm/FileInfo.h>
#include <comm/Thumbnail.h>
#include <control/ZKListView.h>
#include <control/ZKTextView.h>
#include <manager/LanguageManager.h>
#include <ObjectModel/Files.h>
#include <upgrade/Upgrade.h>
#include <UI/Popup.h>
#include <window/ZKWindow.h>
#include <string>

namespace UI::FileList
{
	void RenderFileListItem(ZKListView::ZKListItem* pListItem, int index)
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
			break;
		}
		pFileDate->setTextTrf("file_date", item->GetDate().c_str());
	}

	void OnFileListItemClick(int index)
	{
		OM::FileSystem::FileSystemItem* item = OM::FileSystem::GetItem(index);
		if (UI::POPUP_WINDOW.IsBlocking())
			return;
		UI::POPUP_WINDOW.Cancel();
		switch (item->GetType())
		{
		case OM::FileSystem::FileSystemItemType::file: {
			OM::FileSystem::File* file = (OM::FileSystem::File*)item;
			UI::SetSelectedFile(file);
			if (OM::FileSystem::IsMacroFolder())
			{
				UI::POPUP_WINDOW.Open([]() { UI::RunSelectedFile(); });
				UI::POPUP_WINDOW.SetTextf(LANGUAGEMANAGER->getValue("run_macro").c_str(), item->GetName().c_str());
			}
			else if (OM::FileSystem::IsUsbFolder())
			{
				std::string ext = OM::FileSystem::GetFileExtension(file->GetName());
				if (ext == "img")
				{
					UI::POPUP_WINDOW.Open([]() { UpgradeFromUSB(UI::GetSelectedFile()->GetPath()); });
					UI::POPUP_WINDOW.SetTextf(LANGUAGEMANAGER->getValue("upgrade_from").c_str(),
											  item->GetName().c_str());
				}
				else
				{
					UI::POPUP_WINDOW.Open([]() { OM::FileSystem::UploadFile(UI::GetSelectedFile()); });
					UI::POPUP_WINDOW.SetTitle(LANGUAGEMANAGER->getValue("upload_file").c_str());
					UI::POPUP_WINDOW.SetText(item->GetName().c_str());
					UI::POPUP_WINDOW.SetTextScrollable(false);
				}
			}
			else
			{
				FILEINFO_CACHE->QueueLargeThumbnailRequest(item->GetPath());
				UI::POPUP_WINDOW.Open([]() {
					UI::RunSelectedFile();
					UI::WINDOW.OpenWindow(UI::GetUIControl<ZKWindow>(ID_MAIN_PrintWindow));
				});
				UI::SetPopupFileInfo();
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
} // namespace UI::FileList
