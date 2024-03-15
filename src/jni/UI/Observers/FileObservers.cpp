/*
 * FileObservers.cpp
 *
 *  Created on: 3 Jan 2024
 *      Author: Andy Everitt
 */
#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "Comm/FileInfo.h"
#include "Hardware/Duet.h"
#include "UI/OmObserver.h"
#include "UI/UserInterface.h"
#include "UI/UserInterfaceConstants.h"
#include "manager/LanguageManager.h"
#include "utils.h"
#include <string>

#include "ObjectModel/Files.h"

/*
 * These functions are run when the OM field is received.
 * The function takes 2 arguments:
 * 	- val data from the OM where the type is automatically converted based on the chosen macro
 * 	- indices is an array of the indicies for the OM key
 * The _IF_CHANGED suffix only runs the function if the data is different from the previous
 * time function was called. This is unique to each combination of indices.
 */
static UI::Observer<UI::ui_field_update_cb> FileObserversField[] = {
	OBSERVER_CHAR("dir",
				  [](OBSERVER_CHAR_ARGS) {
					  OM::FileSystem::SetCurrentDir(val);
					  UI::GetUIControl<ZKListView>(ID_MAIN_FileListView)->setSelection(0);
					  UI::GetUIControl<ZKListView>(ID_MAIN_FileListView)->refreshListView();
					  UI::GetUIControl<ZKTextView>(ID_MAIN_FolderID)
						  ->setText(LANGUAGEMANAGER->getValue("folder") + ": " + OM::FileSystem::GetCurrentDirPath());
					  info("Files: current dir = %s", OM::FileSystem::GetCurrentDirPath().c_str());
					  decoder->responseType = Comm::JsonDecoder::ResponseType::filelist;
					  Comm::JsonDecoder::FileListData* data =
						  new Comm::JsonDecoder::FileListData(OM::FileSystem::GetCurrentDirPath());
					  decoder->responseData = data;
				  }),
	OBSERVER_UINT("first",
				  [](OBSERVER_UINT_ARGS) {
					  static_cast<Comm::JsonDecoder::FileListData*>(decoder->responseData)->first = val;
					  if (val == 0)
					  {
						  OM::FileSystem::ClearFileSystem();
					  }
				  }),
	OBSERVER_CHAR("files^:type",
				  [](OBSERVER_CHAR_ARGS) {
					  dbg("Files: type check val=%s", val);
					  uint32_t index =
						  indices[0] + static_cast<Comm::JsonDecoder::FileListData*>(decoder->responseData)->first;
					  switch (*val)
					  {
					  case 'd':
						  OM::FileSystem::AddFolderAt(index);
						  dbg("Files: folder at index %d", index);
						  break;
					  case 'f':
						  OM::FileSystem::AddFileAt(index);
						  dbg("Files: file at index %d", index);
						  break;
					  }
				  }),
	OBSERVER_CHAR("files^:name",
				  [](OBSERVER_CHAR_ARGS) {
					  dbg("Files: name assignment, val=%s", val);
					  uint32_t index =
						  indices[0] + static_cast<Comm::JsonDecoder::FileListData*>(decoder->responseData)->first;
					  OM::FileSystem::FileSystemItem* item = OM::FileSystem::GetItem(index);
					  if (item == nullptr)
						  return;

					  item->SetName(val);
					  info("Files: item[%d] name=%s", index, item->GetName().c_str());
				  }),
	OBSERVER_UINT("files^:size",
				  [](OBSERVER_UINT_ARGS) {
					  uint32_t index =
						  indices[0] + static_cast<Comm::JsonDecoder::FileListData*>(decoder->responseData)->first;
					  OM::FileSystem::FileSystemItem* item = OM::FileSystem::GetItem(index);
					  if (item == nullptr)
						  return;
					  item->SetSize(val);
				  }),
	OBSERVER_CHAR("files^:date",
				  [](OBSERVER_CHAR_ARGS) {
					  uint32_t index =
						  indices[0] + static_cast<Comm::JsonDecoder::FileListData*>(decoder->responseData)->first;
					  OM::FileSystem::FileSystemItem* item = OM::FileSystem::GetItem(index);
					  if (item == nullptr)
						  return;
					  item->SetDate(val);
				  }),
	OBSERVER_UINT("next",
				  [](OBSERVER_UINT_ARGS) {
					  if (val == 0)
						  return;
					  Comm::duet.RequestFileList(OM::FileSystem::GetCurrentDirPath().c_str(), val);
				  }),
};

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
static UI::Observer<UI::ui_array_end_update_cb> FileObserversArrayEnd[] = {
	OBSERVER_ARRAY_END("files^",
					   [](OBSERVER_ARRAY_END_ARGS) {
						   OM::FileSystem::SortFileSystem();
						   for (size_t i = 0; i < OM::FileSystem::GetItemCount(); i++)
						   {
							   OM::FileSystem::FileSystemItem* item = OM::FileSystem::GetItem(i);
							   if (item == nullptr || item->GetType() == OM::FileSystem::FileSystemItemType::folder)
							   {
								   continue;
							   }
							   if (item->GetPath().find("gcodes") == std::string::npos)
							   {
								   continue;
							   }
							   if (FILEINFO_CACHE->IsThumbnailCached(item->GetPath(), item->GetDate().c_str()))
							   {
								   continue;
							   }
							   FILEINFO_CACHE->QueueThumbnailRequest(item->GetPath());
						   }
						   UI::GetUIControl<ZKListView>(ID_MAIN_FileListView)->refreshListView();
					   }),
};
