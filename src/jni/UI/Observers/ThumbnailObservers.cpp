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
#include "UI/OmObserver.h"
#include "UI/UserInterface.h"
#include "UI/UserInterfaceConstants.h"

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
	OBSERVER_CHAR("thumbnails^:format",
				  [](OBSERVER_CHAR_ARGS) {
					  info("thumbnail format %s", val);
					  Comm::Thumbnail* thumbnail =
						  FILEINFO_CACHE->GetCurrentFileInfo()->GetOrCreateThumbnail(indices[0]);
					  if (thumbnail->meta.SetImageFormat(val))
					  {
						  thumbnail->context.state = Comm::ThumbnailState::Header;
					  }
				  }),
	OBSERVER_UINT("thumbnails^:height",
				  [](OBSERVER_UINT_ARGS) {
					  info("thumbnail height %d", val);
					  FILEINFO_CACHE->GetCurrentFileInfo()->GetOrCreateThumbnail(indices[0])->meta.height = val;
				  }),
	OBSERVER_UINT("thumbnails^:offset",
				  [](OBSERVER_UINT_ARGS) {
					  info("thumbnail offset %d", val);
					  FILEINFO_CACHE->GetCurrentFileInfo()->GetOrCreateThumbnail(indices[0])->meta.offset = val;
				  }),
	OBSERVER_UINT("thumbnails^:size",
				  [](OBSERVER_UINT_ARGS) {
					  info("thumbnail size %d", val);
					  FILEINFO_CACHE->GetCurrentFileInfo()->GetOrCreateThumbnail(indices[0])->meta.size = val;
				  }),
	OBSERVER_UINT("thumbnails^:width",
				  [](OBSERVER_UINT_ARGS) {
					  info("thumbnail width %d", val);
					  FILEINFO_CACHE->GetCurrentFileInfo()->GetOrCreateThumbnail(indices[0])->meta.width = val;
				  }),
	// OBSERVER_CHAR("generatedBy",
	// 			  [](OBSERVER_CHAR_ARGS) {
	// 				  // The thumbnail field is not present if no thumbnail is in the file so we need to use this field
	// 				  // which is after the thumbnail key
	// 				  if (Comm::thumbnail.imageFormat == OldThumbnail::ImageFormat::Invalid)
	// 				  {
	// 					  Comm::thumbnailRequestInProgress = false;
	// 				  }
	// 			  }),
	OBSERVER_CHAR(
		"thumbnail:fileName",
		[](OBSERVER_CHAR_ARGS) {
			Comm::Thumbnail* thumbnail = FILEINFO_CACHE->GetCurrentThumbnail();
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
			dbg("Receiving thumbnail information about %s", FILEINFO_CACHE->GetCurrentThumbnail()->filename.c_str());
		}),
	OBSERVER_CHAR("thumbnail:offset",
				  [](OBSERVER_CHAR_ARGS) {
					  Comm::Thumbnail* thumbnail = FILEINFO_CACHE->GetCurrentThumbnail();
					  if (thumbnail == nullptr)
					  {
						  error("Not expecting to receive thumbnail data");
						  return;
					  }

					  uint32_t offset;
					  if (!Comm::GetUnsignedInteger(val, offset))
					  {
						  warn("thumbnail offset error \"%s\"", val);
						  thumbnail->context.parseErr = -4;
						  return;
					  }
					  dbg("thumbnail receive current offset %d.", thumbnail->context.offset);
				  }),
	OBSERVER_CHAR("thumbnail:data",
				  [](OBSERVER_CHAR_ARGS) {
					  Comm::Thumbnail* thumbnail = FILEINFO_CACHE->GetCurrentThumbnail();
					  if (thumbnail == nullptr)
					  {
						  error("Not expecting to receive thumbnail data");
						  return;
					  }

					  info("thumbnail data %d", strlen(val));
					  Comm::thumbnailBuf.size = strnlen(val, sizeof(Comm::thumbnailBuf.buffer));
					  memcpy(Comm::thumbnailBuf.buffer, val, Comm::thumbnailBuf.size);
					  thumbnail->context.state = Comm::ThumbnailState::Data;
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
						  return;
					  }
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
					  dbg("thumbnail receive next offset %d.", thumbnail->context.next);
				  }),
};

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
static UI::Observer<UI::ui_array_end_update_cb> ThumbnailObserversArrayEnd[] = {
	OBSERVER_ARRAY_END(
		"thumbnails^",
		[](OBSERVER_ARRAY_END_ARGS) { FILEINFO_CACHE->GetCurrentFileInfo()->ClearThumbnails(indices[0]); }),
};
