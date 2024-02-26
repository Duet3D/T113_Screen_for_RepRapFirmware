/*
 * FanObservers.h
 *
 *  Created on: 8 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_UI_OBSERVERS_THUMBNAILOBSERVERS_HPP_
#define JNI_LOGIC_UI_OBSERVERS_THUMBNAILOBSERVERS_HPP_

#include "Debug.h"

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
	OBSERVER_CHAR("fileName", [](OBSERVER_CHAR_ARGS) { thumbnailContext.filename.copy(val); }),
	OBSERVER_CHAR("thumbnails^:format",
				  [](OBSERVER_CHAR_ARGS) {
					  info("thumbnail format %s", data);
					  thumbnail.imageFormat = Thumbnail::ImageFormat::Invalid;
					  if (strcmp(data, "qoi") == 0)
					  {
						  thumbnail.imageFormat = Thumbnail::ImageFormat::Qoi;

						  thumbnailContext.state = ThumbnailState::Header;
					  }
				  }),
	OBSERVER_UINT("thumbnails^:height",
				  [](OBSERVER_UINT_ARGS) {
					  info("thumbnail height %d", val);
					  thumbnail.height = val;
				  }),
	OBSERVER_UINT("thumbnails^:offset",
				  [](OBSERVER_UINT_ARGS) {
					  info("thumbnail offset %d", val);
					  thumbnailContext.next = val;
				  }),
	OBSERVER_UINT("thumbnails^:size",
				  [](OBSERVER_UINT_ARGS) {
					  info("thumbnail size %d", val);
					  thumbnailContext.size = val;
				  }),
	OBSERVER_UINT("thumbnails^:width",
				  [](OBSERVER_UINT_ARGS) {
					  info("thumbnail width %d", val);
					  thumbnail.width = val;
				  }),
	OBSERVER_CHAR("thumbnail:data",
				  [](OBSERVER_CHAR_ARGS) {
					  info("thumbnail data %d", strlen(val));
					  thumbnailData.size = std::min(strlen(val), sizeof(thumbnailData.buffer));
					  memcpy(thumbnailData.buffer, val, thumbnailData.size);
					  thumbnailContext.state = ThumbnailState::Data;
				  }),
	OBSERVER_CHAR("thumbnail:err",
				  [](OBSERVER_CHAR_ARGS) {
					  if (!GetInteger(val, thumbnailContext.err))
					  {
						  warn("Failed to parse thumbnail err %s", val);
						  thumbnailContext.parseErr = -1;
					  }
				  }),
	OBSERVER_CHAR("thumbnail:fileName",
				  [](OBSERVER_CHAR_ARGS) {
					  if (!thumbnailContext.filename.Equals(val))
					  {
						  warn("Thumbnail filename not the same as previous M36 request (%s) \"%s\"",
							   thumbnailContext.filename.c_str(),
							   val);
						  thumbnailContext.parseErr = -2;
					  }
				  }),
	OBSERVER_CHAR("thumbnail:next",
				  [](OBSERVER_CHAR_ARGS) {
					  if (!GetUnsignedInteger(val, thumbnailContext.next))
					  {
						  warn("thumbnail next error \"%s\"", val);
						  thumbnailContext.parseErr = -3;
						  break;
					  }
					  dbg("thumbnail receive next offset %d.", thumbnailContext.next);
				  }),
};

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
static UI::Observer<UI::ui_array_end_update_cb> ThumbnailObserversArrayEnd[] = {
	OBSERVER_ARRAY_END("thumbnails^",
					   [](OBSERVER_ARRAY_END_ARGS) {
						   if (OM::RemoveThumbnail(indices[0], true))
						   {
							   mPrintThumbnailListPtr->refreshListView();
						   }
					   }),
};

#endif /* JNI_LOGIC_UI_OBSERVERS_THUMBNAILOBSERVERS_HPP_ */
