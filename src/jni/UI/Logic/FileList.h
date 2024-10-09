/*
 * FileList.h
 *
 *  Created on: 23 Apr 2024
 *      Author: andy
 */

#ifndef JNI_UI_LOGIC_FILELIST_H_
#define JNI_UI_LOGIC_FILELIST_H_

#include "UI/UserInterface.h"

namespace UI::FileList
{
	void Init();

	void SetSelectedFile(const OM::FileSystem::File* file);
	const OM::FileSystem::File* GetSelectedFile();
	ZKTextView* GetThumbnail();

	void ReloadFileList();
	size_t GetFileListCount();
	void SetFileListItem(ZKListView::ZKListItem* pListItem, int index);
	void FileListItemCallback(int index);
	void SetPopupFileInfo();

	void RequestUSBFiles();
	void RefreshFileList();
} // namespace UI::FileList

#endif /* JNI_UI_LOGIC_FILELIST_H_ */
