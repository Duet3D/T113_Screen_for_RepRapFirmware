/*
 * FanObservers.cpp
 *
 *  Created on: 8 Jan 2024
 *      Author: Andy Everitt
 */
#include "Debug.h"

#include "Comm/Communication.h"
#include "Library/Thumbnail.h"
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
					  Comm::thumbnailRequestInProgress = true;
					  Comm::thumbnailContext.filename.copy(val);
				  }),
	OBSERVER_CHAR("thumbnails^:format",
				  [](OBSERVER_CHAR_ARGS) {
					  info("thumbnail format %s", val);
					  if (Comm::thumbnail.SetImageFormat(val))
					  {
						  Comm::thumbnailContext.state = Comm::ThumbnailState::Header;
					  }
				  }),
	OBSERVER_UINT("thumbnails^:height",
				  [](OBSERVER_UINT_ARGS) {
					  info("thumbnail height %d", val);
					  Comm::thumbnail.height = val;
				  }),
	OBSERVER_UINT("thumbnails^:offset",
				  [](OBSERVER_UINT_ARGS) {
					  info("thumbnail offset %d", val);
					  Comm::thumbnailContext.next = val;
				  }),
	OBSERVER_UINT("thumbnails^:size",
				  [](OBSERVER_UINT_ARGS) {
					  info("thumbnail size %d", val);
					  Comm::thumbnailContext.size = val;
				  }),
	OBSERVER_UINT("thumbnails^:width",
				  [](OBSERVER_UINT_ARGS) {
					  info("thumbnail width %d", val);
					  Comm::thumbnail.width = val;
				  }),
	OBSERVER_CHAR("generatedBy",
				  [](OBSERVER_CHAR_ARGS) {
					  // The thumbnail field is not present if no thumbnail is in the file so we need to use this field
					  // which is after the thumbnail key
					  if (Comm::thumbnail.imageFormat == Thumbnail::ImageFormat::Invalid)
					  {
						  Comm::thumbnailRequestInProgress = false;
					  }
				  }),
	OBSERVER_CHAR("thumbnail:data",
				  [](OBSERVER_CHAR_ARGS) {
					  Comm::thumbnailRequestInProgress = true;
					  if (Comm::stopThumbnailRequest)
					  {
						  warn("Thumbnail request cancelled");
						  Comm::thumbnailContext.state = Comm::ThumbnailState::Init;
						  Comm::thumbnailRequestInProgress = false;
						  return;
					  }
					  info("thumbnail data %d", strlen(val));
					  Comm::thumbnailData.size = std::min(strlen(val), sizeof(Comm::thumbnailData.buffer));
					  memcpy(Comm::thumbnailData.buffer, val, Comm::thumbnailData.size);
					  Comm::thumbnailContext.state = Comm::ThumbnailState::Data;
				  }),
	OBSERVER_CHAR("thumbnail:err",
				  [](OBSERVER_CHAR_ARGS) {
					  if (!Comm::GetInteger(val, Comm::thumbnailContext.err))
					  {
						  warn("Failed to parse thumbnail err %s", val);
						  Comm::thumbnailContext.parseErr = -1;
						  return;
					  }
				  }),
	OBSERVER_CHAR("thumbnail:fileName",
				  [](OBSERVER_CHAR_ARGS) {
					  if (!Comm::thumbnailContext.filename.Equals(val))
					  {
						  warn("Thumbnail filename not the same as previous M36 request (%s) \"%s\"",
							   Comm::thumbnailContext.filename.c_str(),
							   val);
						  Comm::thumbnailContext.parseErr = -2;
						  return;
					  }
				  }),
	OBSERVER_CHAR("thumbnail:next",
				  [](OBSERVER_CHAR_ARGS) {
					  if (!Comm::GetUnsignedInteger(val, Comm::thumbnailContext.next))
					  {
						  warn("thumbnail next error \"%s\"", val);
						  Comm::thumbnailContext.parseErr = -3;
						  return;
					  }
					  dbg("thumbnail receive next offset %d.", Comm::thumbnailContext.next);
				  }),
	OBSERVER_CHAR("thumbnail:offset",
				  [](OBSERVER_CHAR_ARGS) {
					  if (!Comm::GetUnsignedInteger(val, Comm::thumbnailContext.offset))
					  {
						  warn("thumbnail offset error \"%s\"", val);
						  Comm::thumbnailContext.parseErr = -4;
						  return;
					  }
					  dbg("thumbnail receive current offset %d.", Comm::thumbnailContext.offset);
				  }),
};

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
static UI::Observer<UI::ui_array_end_update_cb> ThumbnailObserversArrayEnd[] = {};
