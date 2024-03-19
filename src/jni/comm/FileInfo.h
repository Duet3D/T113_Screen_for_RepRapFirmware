/*
 * FileInfo.h
 *
 *  Created on: 12 Mar 2024
 *      Author: andy
 */

#ifndef JNI_COMM_FILEINFO_H_
#define JNI_COMM_FILEINFO_H_

#include "Thumbnail.h"

#include "Comm/Commands.h"
#include "Configuration.h"
#include "Duet3D/General/String.h"
#include "Duet3D/General/StringRef.h"
#include <list>
#include <map>
#include <stdint.h>
#include <vector>

namespace Comm
{
	struct FileInfo
	{
	  public:
		FileInfo();
		~FileInfo();

		String<MAX_FILENAME_LENGTH> filename;
		uint32_t size;
		String<20> lastModified;
		float height;
		float layerHeight;
		uint32_t printTime;
		std::vector<float> filament;
		String<30> generatedBy;

		Thumbnail* GetThumbnail(size_t index);
		Thumbnail* GetOrCreateThumbnail(size_t index);
		size_t GetThumbnailCount() const { return thumbnails.size(); }
		size_t ClearThumbnails(size_t fromIndex);

		tm GetPrintTime() const;
		std::string GetReadableFileSize() const;

	  private:
		std::vector<Thumbnail*> thumbnails;
	};

	class FileInfoCache
	{
	  public:
		static FileInfoCache* GetInstance()
		{
			static FileInfoCache instance;
			return &instance;
		}

		void Spin(); // processes thumbnail requests

		bool IsThumbnailCached(const std::string& filepath,
							   const char* lastModified); // returns true if a thumbnail for the file is in cache
														  // and lastModified is the same

		void SetCurrentFileInfo(const char* filepath);		// set and return the current file info being received. If
															// file info doesn't exist, it will be created
		FileInfo* GetCurrentFileInfo();						// returns the current file info
		FileInfo* GetFileInfo(const std::string& filepath); // returns the file info for the given gcode file path
		void FileInfoRequestComplete()						// called when the file info request is complete
		{
			m_fileInfoRequestInProgress = false;
		}

		void ClearCache(); // clears the cache

		bool QueueThumbnailRequest(const std::string& filepath); // returns true if the request was queued
		bool RequestThumbnail(FileInfo& fileInfo,
							  size_t index);			 // returns true if a thumbnail request was started
		bool RequestThumbnail(Thumbnail* thumbnail);	 // returns true if a thumbnail request was started
		bool ThumbnailRequestInProgress();				 // returns true if a thumbnail request is in progress
		void ReceivingThumbnailResponse(bool receiving); // set to true when receiving thumbnail data, false when done

		Thumbnail* GetCurrentThumbnail(bool force = false); // returns the current thumbnail or nullptr if not
															// processing thumbnail request. Will return nullptr if you
															// the response has not been received, unless you use force
		bool StopThumbnailRequest();

		void Debug(); // prints debug info

	  private:
		FileInfoCache();

		bool QueueFileInfoRequest(const std::string& filepath); // queues a file info request if not already queued
		void SetCurrentThumbnail(Thumbnail* thumbnail);			// set and return the current thumbnail being received
		Thumbnail* GetNextThumbnail();							// returns the thumbnail for the next queued thumbail
																// request, or nullptr if queue is empty.

		bool m_fileInfoRequestInProgress = false;
		bool m_thumbnailRequestInProgress = false;
		bool m_thumbnailResponseInProgress = false;
		FileInfo* m_currentFileInfo = nullptr; // the file info currently being processed
		Thumbnail* m_currentThumbnail = nullptr;
		std::map<std::string, FileInfo*> m_cache; // cache of file path and their associated file info
		std::list<std::string> m_fileInfoRequestQueue;
		std::list<Thumbnail*> m_thumbnailRequestQueue;
		long long m_lastThumbnailRequestTime = 0;
	};

	extern ThumbnailBuf thumbnailBuf;

} // namespace Comm
#define FILEINFO_CACHE Comm::FileInfoCache::GetInstance()

#endif /* JNI_COMM_FILEINFO_H_ */
