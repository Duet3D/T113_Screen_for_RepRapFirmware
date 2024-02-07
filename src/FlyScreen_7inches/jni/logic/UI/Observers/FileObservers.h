/*
 * FileObservers.h
 *
 *  Created on: 3 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_UI_OBSERVERS_FILEOBSERVERS_HPP_
#define JNI_LOGIC_UI_OBSERVERS_FILEOBSERVERS_HPP_

#include "Debug.h"

#include <string>
#include "ObjectModel/Files.h"
#include "UI/OmObserver.h"
#include "UI/UserInterfaceConstants.h"
#include "UI/UserInterface.h"
#include "manager/LanguageManager.h"



/*
 * These functions are run when the OM field is received.
 * The function takes 2 arguments:
 * 	- val data from the OM where the type is automatically converted based on the chosen macro
 * 	- indices is an array of the indicies for the OM key
 * The _IF_CHANGED suffix only runs the function if the data is different from the previous
 * time function was called. This is unique to each combination of indices.
 */
static UI::Observer<UI::ui_field_update_cb> FileObserversField[] = {
	OBSERVER_CHAR(
		"dir",
		[](OBSERVER_CHAR_ARGS)
		{
			OM::FileSystem::SetCurrentDir(val);
			mFileListViewPtr->setSelection(0);
			mFileListViewPtr->refreshListView();
			mFolderIDPtr->setText(LANGUAGEMANAGER->getValue("folder") + ": " + OM::FileSystem::GetCurrentDirPath());
			info("Files: current dir = %s", OM::FileSystem::GetCurrentDirPath().c_str());
		}
	),
	OBSERVER_CHAR(
		"files^:type",
		[](OBSERVER_CHAR_ARGS)
		{
			dbg("Files: type check val=%s", val);
			switch (*val)
			{
			case 'd':
				OM::FileSystem::AddFolderAt(indices[0]);
				dbg("Files: folder at index %d", indices[0]);
				break;
			case 'f':
				OM::FileSystem::AddFileAt(indices[0]);
				dbg("Files: file at index %d", indices[0]);
				break;
			}
		}
	),
	OBSERVER_CHAR(
		"files^:name",
		[](OBSERVER_CHAR_ARGS)
		{
			dbg("Files: name assignment, val=%s", val);
			OM::FileSystem::FileSystemItem* item = OM::FileSystem::GetItem(indices[0]);
			if (item == nullptr)
				return;

			item->SetName(val);
			info("Files: item[%d] name=%s", indices[0], item->GetName().c_str());
		}
	),
	OBSERVER_UINT(
		"files^:size",
		[](OBSERVER_UINT_ARGS)
		{
			OM::FileSystem::FileSystemItem* item = OM::FileSystem::GetItem(indices[0]);
			if (item == nullptr)
				return;
			item->SetSize(val);
		}
	),
	OBSERVER_CHAR(
		"files^:date",
		[](OBSERVER_CHAR_ARGS)
		{
			OM::FileSystem::FileSystemItem* item = OM::FileSystem::GetItem(indices[0]);
			if (item == nullptr)
				return;
			item->SetDate(val);
		}
	),
};

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
static UI::Observer<UI::ui_array_end_update_cb> FileObserversArrayEnd[] = {
	OBSERVER_ARRAY_END(
		"files^",
		[](OBSERVER_ARRAY_END_ARGS)
		{
			OM::FileSystem::SortFileSystem();
			mFileListViewPtr->refreshListView();
		}
	),
};



#endif /* JNI_LOGIC_UI_OBSERVERS_FILEOBSERVERS_HPP_ */
