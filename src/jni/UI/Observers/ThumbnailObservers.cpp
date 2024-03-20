/*
 * FanObservers.cpp
 *
 *  Created on: 8 Jan 2024
 *      Author: Andy Everitt
 */
#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "Comm/Communication.h"
#include "Comm/FileInfo.h"
#include "Configuration.h"
#include "UI/OmObserver.h"
#include "UI/UserInterface.h"

/*
 * These functions are run when the OM field is received.
 * The function takes 2 arguments:
 * 	- val data from the OM where the type is automatically converted based on the chosen macro
 * 	- indices is an array of the indicies for the OM key
 * The _IF_CHANGED suffix only runs the function if the data is different from the previous
 * time function was called. This is unique to each combination of indices.
 */
static UI::Observer<UI::ui_field_update_cb> ThumbnailObserversField[] = {
	OBSERVER_CHAR("fileName",
				  [](OBSERVER_CHAR_ARGS) {
					  FILEINFO_CACHE->SetCurrentFileInfo(val);
					  dbg("Receiving file info about %s", FILEINFO_CACHE->GetCurrentFileInfo()->filename.c_str());
				  }),
	OBSERVER_CHAR("lastModified",
				  [](OBSERVER_CHAR_ARGS) {
					  info("lastModified %s", val);
					  Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
					  if (fileInfo == nullptr)
					  {
						  warn("FileInfo not found");
						  return;
					  }
					  fileInfo->lastModified.copy(val);
				  }),
	OBSERVER_UINT("size",
				  [](OBSERVER_UINT_ARGS) {
					  Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
					  if (fileInfo == nullptr)
					  {
						  warn("FileInfo not found");
						  return;
					  }
					  fileInfo->size = val;
				  }),
	OBSERVER_UINT("printTime",
				  [](OBSERVER_UINT_ARGS) {
					  Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
					  if (fileInfo == nullptr)
					  {
						  warn("FileInfo not found");
						  return;
					  }
					  fileInfo->printTime = val;
				  }),
	OBSERVER_UINT("simulatedTime",
				  [](OBSERVER_UINT_ARGS) {
					  Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
					  if (fileInfo == nullptr)
					  {
						  warn("FileInfo not found");
						  return;
					  }
					  fileInfo->printTime = val;
				  }),
	OBSERVER_FLOAT("height",
				   [](OBSERVER_FLOAT_ARGS) {
					   Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
					   if (fileInfo == nullptr)
					   {
						   warn("FileInfo not found");
						   return;
					   }
					   fileInfo->height = val;
				   }),
	OBSERVER_FLOAT("layerHeight",
				   [](OBSERVER_FLOAT_ARGS) {
					   Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
					   if (fileInfo == nullptr)
					   {
						   warn("FileInfo not found");
						   return;
					   }
					   fileInfo->layerHeight = val;
				   }),
	OBSERVER_CHAR("thumbnails^:format",
				  [](OBSERVER_CHAR_ARGS) {
					  verbose("thumbnail format %s", val);
					  Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
					  if (fileInfo == nullptr)
					  {
						  warn("FileInfo not found");
						  return;
					  }
					  Comm::Thumbnail* thumbnail =
						  fileInfo->GetOrCreateThumbnail(indices[0]);
					  if (!thumbnail->meta.SetImageFormat(val))
					  {
						  warn("Thumbnail format invalid");
					  }
				  }),
	OBSERVER_UINT("thumbnails^:height",
				  [](OBSERVER_UINT_ARGS) {
					  verbose("thumbnail height %d", val);
					  Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
					  if (fileInfo == nullptr)
					  {
						  warn("FileInfo not found");
						  return;
					  }
					  fileInfo->GetOrCreateThumbnail(indices[0])->meta.height = val;
				  }),
	OBSERVER_UINT("thumbnails^:offset",
				  [](OBSERVER_UINT_ARGS) {
					  verbose("thumbnail offset %d", val);
					  Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
					  if (fileInfo == nullptr)
					  {
						  warn("FileInfo not found");
						  return;
					  }
					  Comm::Thumbnail* thumbnail =
						  fileInfo->GetOrCreateThumbnail(indices[0]);
					  thumbnail->meta.offset = val;
					  thumbnail->context.next = val;
				  }),
	OBSERVER_UINT("thumbnails^:size",
				  [](OBSERVER_UINT_ARGS) {
					  verbose("thumbnail size %d", val);
					  Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
					  if (fileInfo == nullptr)
					  {
						  warn("FileInfo not found");
						  return;
					  }
					  fileInfo->GetOrCreateThumbnail(indices[0])->meta.size = val;
				  }),
	OBSERVER_UINT("thumbnails^:width",
				  [](OBSERVER_UINT_ARGS) {
					  verbose("thumbnail width %d", val);
					  Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
					  if (fileInfo == nullptr)
					  {
						  warn("FileInfo not found");
						  return;
					  }
					  fileInfo->GetOrCreateThumbnail(indices[0])->meta.width = val;
				  }),
	OBSERVER_CHAR("generatedBy",
				  [](OBSERVER_CHAR_ARGS) {
					  Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
					  if (fileInfo == nullptr)
					  {
						  warn("FileInfo not found");
						  return;
					  }
				  }),
	OBSERVER_CHAR(
		"thumbnail:fileName",
		[](OBSERVER_CHAR_ARGS) {
			Comm::Thumbnail* thumbnail = FILEINFO_CACHE->GetCurrentThumbnail(true);
			decoder->responseType = Comm::JsonDecoder::ResponseType::thumbnail;
			decoder->responseData = thumbnail;
			if (thumbnail == nullptr)
			{
				error("Not expecting to receive thumbnail data");
				return;
			}
			if (!thumbnail->filename.Equals(val))
			{
				warn("M36.1 filename (%s) not the same as what is expected (%s)", val, thumbnail->filename.c_str());
				thumbnail->context.parseErr = -2;
			}
			info("Receiving thumbnail information about %s", thumbnail->filename.c_str());
			FILEINFO_CACHE->ReceivingThumbnailResponse(true);
		}),
	OBSERVER_CHAR("thumbnail:offset",
				  [](OBSERVER_CHAR_ARGS) {
					  Comm::Thumbnail* thumbnail = FILEINFO_CACHE->GetCurrentThumbnail();
					  if (thumbnail == nullptr)
					  {
						  error("Not expecting to receive thumbnail data");
						  return;
					  }

					  if (!Comm::GetUnsignedInteger(val, thumbnail->context.offset))
					  {
						  warn("thumbnail offset error \"%s\"", val);
						  thumbnail->context.parseErr = -4;
						  return;
					  }
					  dbg("thumbnail receive current offset %u.", thumbnail->context.offset);
				  }),
	OBSERVER_CHAR("thumbnail:data",
				  [](OBSERVER_CHAR_ARGS) {
					  Comm::Thumbnail* thumbnail = FILEINFO_CACHE->GetCurrentThumbnail();
					  if (thumbnail == nullptr)
					  {
						  error("Not expecting to receive thumbnail data");
						  return;
					  }

					  verbose("thumbnail data %d", strlen(val));
					  Comm::thumbnailBuf.size = strnlen(val, sizeof(Comm::thumbnailBuf.buffer));
					  memcpy(Comm::thumbnailBuf.buffer, val, Comm::thumbnailBuf.size);
					  thumbnail->context.state = Comm::ThumbnailState::Data;
				  }),
	OBSERVER_CHAR("thumbnail:next",
				  [](OBSERVER_CHAR_ARGS) {
					  Comm::Thumbnail* thumbnail = FILEINFO_CACHE->GetCurrentThumbnail();
					  if (thumbnail == nullptr)
					  {
						  error("Not expecting to receive thumbnail data");
						  return;
					  }

					  if (!Comm::GetUnsignedInteger(val, thumbnail->context.next))
					  {
						  warn("thumbnail next error \"%s\"", val);
						  thumbnail->context.parseErr = -3;
						  return;
					  }
					  verbose("thumbnail receive next offset %d.", thumbnail->context.next);
				  }),
	OBSERVER_CHAR("thumbnail:err",
				  [](OBSERVER_CHAR_ARGS) {
					  Comm::Thumbnail* thumbnail = FILEINFO_CACHE->GetCurrentThumbnail();
					  if (thumbnail == nullptr)
					  {
						  error("Not expecting to receive thumbnail data");
						  return;
					  }

					  if (!Comm::GetInteger(val, thumbnail->context.err))
					  {
						  warn("Failed to parse thumbnail err %s", val);
						  thumbnail->context.parseErr = -1;
					  }
					  info("Thumbnail: offset(%d), next(%d), err(%d), size(%d), parseErr(%d)",
						   thumbnail->context.offset,
						   thumbnail->context.next,
						   thumbnail->context.err,
						   thumbnail->context.size,
						   thumbnail->context.parseErr);
				  }),
};

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
static UI::Observer<UI::ui_array_end_update_cb> ThumbnailObserversArrayEnd[] = {
	OBSERVER_ARRAY_END(
		"thumbnails^",
		[](OBSERVER_ARRAY_END_ARGS) {
			dbg("Thumbnail array end");
			Comm::FileInfo* fileInfo = FILEINFO_CACHE->GetCurrentFileInfo();
			if (fileInfo == nullptr)
				return;
			fileInfo->ClearThumbnails(indices[0]);
			FILEINFO_CACHE->QueueThumbnailRequest(fileInfo->filename.c_str());
			info("FileInfo: filename(%s) thumbnails(%d)", fileInfo->filename.c_str(), fileInfo->GetThumbnailCount());
			for (size_t i = 0; i < fileInfo->GetThumbnailCount(); i++)
			{
				Comm::Thumbnail* thumbnail = fileInfo->GetOrCreateThumbnail(i);
				info("Thumbnail %d: filename(%s) offset(%d) size(%d) width(%d) height(%d) format(%d)",
					 i,
					 thumbnail->filename.c_str(),
					 thumbnail->meta.offset,
					 thumbnail->meta.size,
					 thumbnail->meta.width,
					 thumbnail->meta.height,
					 thumbnail->meta.imageFormat);
			}
			FILEINFO_CACHE->FileInfoRequestComplete();
		}),
};
