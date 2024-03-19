/*
 * FileInfo.cpp
 *
 *  Created on: 12 Mar 2024
 *      Author: andy
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"
#include "DebugCommands.h"

#include "FileInfo.h"

#include "Configuration.h"
#include "Hardware/Duet.h"
#include "UI/UserInterface.h"
#include "utils.h"
#include <utils/TimeHelper.h>

namespace Comm
{
	FileInfo::FileInfo() : size(0), height(0), layerHeight(0), printTime(0)
	{
		dbg("Created new fileinfo");
	}

	FileInfo::~FileInfo()
	{
		dbg("Deleted fileinfo %s", filename.c_str());
		for (auto& thumbnail : thumbnails)
		{
			if (thumbnail == nullptr)
				continue;
			delete thumbnail;
		}
	}

	Thumbnail* FileInfo::GetThumbnail(size_t index)
	{
		if (index >= thumbnails.size())
		{
			return nullptr;
		}

		return thumbnails[index];
	}

	Thumbnail* FileInfo::GetOrCreateThumbnail(size_t index)
	{
		if (index >= thumbnails.size())
		{
			thumbnails.resize(index + 1);
			thumbnails[index] = new Thumbnail(filename.GetRef());
		}

		return thumbnails[index];
	}

	size_t FileInfo::ClearThumbnails(size_t fromIndex)
	{
		size_t count = 0;
		for (size_t i = fromIndex; i < thumbnails.size(); ++i)
		{
			if (thumbnails[i] == nullptr)
				continue;
			delete thumbnails[i];
			count++;
		}
		thumbnails.resize(fromIndex);
		return count;
	}

	tm FileInfo::GetPrintTime() const
	{
		tm time;
		uint32_t seconds = printTime;
		time.tm_hour = static_cast<int>(seconds / 3600);
		time.tm_min = static_cast<int>((seconds - time.tm_hour * 3600) / 60);
		time.tm_sec = static_cast<int>(seconds - time.tm_hour * 3600 - time.tm_min * 60);
		dbg("Print time (%d): %d:%02d:%02d", printTime, time.tm_hour, time.tm_min, time.tm_sec);
		return time;
	}

	FileInfoCache::FileInfoCache() {}

	void FileInfoCache::Spin()
	{
		// Update status message
		UI::GetUIControl<ZKTextView>(ID_MAIN_FileListInfo)
			->setTextTrf("file_cache_state",
						 m_fileInfoRequestQueue.size(),
						 m_cache.size(),
						 m_currentFileInfoRequest.c_str(),
						 m_currentThumbnail == nullptr
							 ? ""
							 : utils::format("%u%% %s",
											 std::min(100u,
													  (100 * (std::max(m_currentThumbnail->meta.offset,
																	   m_currentThumbnail->context.offset) -
															  m_currentThumbnail->meta.offset)) /
														  m_currentThumbnail->meta.size),
											 m_currentThumbnail->filename.c_str())
								   .c_str());

		if (m_currentThumbnail != nullptr &&
			TimeHelper::getCurrentTime() > m_lastThumbnailRequestTime + THUMBNAIL_REQUEST_TIMEOUT)
		{
			Thumbnail* thumbnail = m_currentThumbnail;
			m_currentThumbnail = nullptr;
			warn("Thumbnail request timed out for %s", thumbnail->filename.c_str());
			DeleteCachedThumbnail(thumbnail->filename.c_str());
			QueueThumbnailRequest(thumbnail->filename.c_str());
		}

		if (!m_fileInfoRequestInProgress && m_currentFileInfo != nullptr)
		{
			m_currentFileInfoRequest.clear();
			m_currentFileInfo = nullptr;
		}

		if (m_currentThumbnail == nullptr)
		{
			m_thumbnailRequestInProgress = false;
		}

		if (m_thumbnailRequestInProgress)
		{
			if (m_currentThumbnail->context.parseErr != 0 || m_currentThumbnail->context.err != 0)
			{
				warn("Thumbnail request failed for %s, parseErr(%d), err(%d)",
					 m_currentThumbnail->filename.c_str(),
					 m_currentThumbnail->context.parseErr,
					 m_currentThumbnail->context.err);
				DeleteCachedThumbnail(m_currentThumbnail->filename.c_str());
				m_thumbnailRequestInProgress = false;
				m_currentThumbnail = nullptr;
				return;
			}

			// Check if the request is done
			switch (m_currentThumbnail->context.state)
			{
			case ThumbnailState::Init:
				m_currentThumbnail = nullptr;
				m_thumbnailRequestInProgress = false;
				m_thumbnailResponseInProgress = false;
				break;
			case ThumbnailState::Data:
			case ThumbnailState::DataWait:
				verbose("Thumbnail request in progress for %s, state=%d",
						m_currentThumbnail->filename.c_str(),
						m_currentThumbnail->context.state);
				return;
			case ThumbnailState::DataRequest:
				m_lastThumbnailRequestTime = TimeHelper::getCurrentTime();
				duet.RequestThumbnail(m_currentThumbnail->filename.c_str(), m_currentThumbnail->context.next);
				m_currentThumbnail->context.state = ThumbnailState::DataWait;
				return;
			case ThumbnailState::Cached:
				m_thumbnailRequestInProgress = false;
				m_currentThumbnail = nullptr;
				break;
			default:
				break;
			}
		}

		if (m_queuedLargeThumbnail != nullptr)
		{
			if (RequestThumbnail(m_queuedLargeThumbnail))
			{
				m_queuedLargeThumbnail = nullptr;
				return;
			}
		}

		if (!m_fileInfoRequestQueue.empty() && !m_fileInfoRequestInProgress)
		{
			std::string filepath = m_fileInfoRequestQueue.front();
			m_fileInfoRequestQueue.pop_front();
			m_fileInfoRequestInProgress = true;
			m_currentFileInfoRequest = filepath;
			duet.RequestFileInfo(filepath.c_str());
			return;
		}

		if (m_thumbnailRequestQueue.empty())
		{
			// dbg("Request queue is empty");
			return;
		}

		dbg("Processing thumbnail request queue");
		Thumbnail* thumbnail = m_thumbnailRequestQueue.front();
		m_thumbnailRequestQueue.pop_front();

		RequestThumbnail(thumbnail);
	}

	bool FileInfoCache::IsThumbnailCached(const std::string& filepath, const char* lastModified)
	{
		// Does a thumbnail file exist in the file system?
		if (!::IsThumbnailCached(filepath.c_str(), false))
		{
			dbg("Thumbnail file for %s does not exist", filepath.c_str());
			return false;
		}

		// Do we have a cache for the files meta data?
		if (m_cache.find(filepath) == m_cache.end())
		{
			dbg("No file info cached for %s", filepath.c_str());
			return false;
		}

		FileInfo* fileInfo = m_cache[filepath];

		// Is the last modified time the same?
		if (!fileInfo->lastModified.Equals(lastModified))
		{
			dbg("Last modified time for %s does not match", filepath.c_str());
			return false;
		}

		return true;
	}

	void FileInfoCache::SetCurrentFileInfo(const char* filepath)
	{
		if (m_cache.find(filepath) == m_cache.end())
		{
			m_cache[filepath] = new FileInfo;
		}
		m_currentFileInfo = m_cache[filepath];
		m_currentFileInfo->filename.copy(filepath);
		// TODO do we need to clear the info here?
	}

	FileInfo* FileInfoCache::GetCurrentFileInfo()
	{
		return m_currentFileInfo;
	}

	FileInfo* FileInfoCache::GetFileInfo(const std::string& filepath)
	{
		if (m_cache.find(filepath) == m_cache.end())
		{
			return nullptr;
		}
		return m_cache[filepath];
	}

	void FileInfoCache::FileInfoRequestComplete()
	{
		m_fileInfoRequestInProgress = false;
		if (m_currentFileInfo == nullptr)
			return;

		const OM::FileSystem::File* file = UI::GetSelectedFile();
		if (file == nullptr)
		{
			return;
		}
		if (file->GetPath() == m_currentFileInfo->filename.c_str())
		{
			UI::SetPopupFileInfo();
			QueueLargeThumbnailRequest(file->GetPath());
		}
	}

	void FileInfoCache::ClearCache()
	{
		info("Clearing file info cache");
		m_currentThumbnail = nullptr;
		m_currentFileInfo = nullptr;

		for (auto& it : m_cache)
		{
			if (it.second == nullptr)
				continue;
			delete it.second;
		}
		m_cache.clear();
		m_fileInfoRequestQueue.clear();
		m_thumbnailRequestQueue.clear();
		ClearAllCachedThumbnails();
		info("Cache cleared");
	}

	bool FileInfoCache::QueueFileInfoRequest(const std::string& filepath, bool next)
	{
		for (auto& queuedPath : m_fileInfoRequestQueue)
		{
			if (queuedPath == filepath)
			{
				if (!next)
					return false;
				m_fileInfoRequestQueue.remove(queuedPath);
				break;
			}
		}
		info("Queueing file info request for %s", filepath.c_str());
		if (next)
		{
			m_fileInfoRequestQueue.push_front(filepath);
		}
		else
		{
			m_fileInfoRequestQueue.push_back(filepath);
		}
		return true;
	}

	bool FileInfoCache::QueueThumbnailRequest(const std::string& filepath)
	{
		if (m_currentThumbnail != nullptr && m_currentThumbnail->filename.Equals(filepath.c_str()))
		{
			warn("Thumbnail request for %s already in progress", filepath.c_str());
			return false;
		}

		FileInfo* fileInfo = GetFileInfo(filepath);
		if (fileInfo == nullptr)
		{
			QueueFileInfoRequest(filepath);
			return false;
		}

		Thumbnail* largestValidThumbnail = nullptr;
		size_t largestSize = 0;
		for (size_t i = 0; i < fileInfo->GetThumbnailCount(); i++)
		{
			Thumbnail* thumbnail = fileInfo->GetThumbnail(i);
			if (thumbnail == nullptr)
				continue;
			if (thumbnail->meta.size <= MAX_THUMBNAIL_CACHE_SIZE && thumbnail->meta.size > largestSize)
			{
				largestValidThumbnail = thumbnail;
				largestSize = thumbnail->meta.size;
			}
		}
		if (largestValidThumbnail == nullptr)
		{
			warn("No valid thumbnail found for %s", filepath.c_str());
			return false;
		}
		m_thumbnailRequestQueue.push_back(largestValidThumbnail);
		return true;
	}

	bool FileInfoCache::QueueLargeThumbnailRequest(const std::string& filepath)
	{
		m_queuedLargeThumbnail = nullptr;
		DeleteCachedThumbnail(largeThumbnailFilename);

		FileInfo* fileInfo = GetFileInfo(filepath);
		if (fileInfo == nullptr)
		{
			QueueFileInfoRequest(filepath, true);
			return false;
		}

		Thumbnail* largestThumbnail = nullptr;
		size_t largestSize = 0;
		for (size_t i = 0; i < fileInfo->GetThumbnailCount(); i++)
		{
			Thumbnail* thumbnail = fileInfo->GetThumbnail(i);
			if (thumbnail == nullptr)
				continue;
			if (thumbnail->meta.size > largestSize)
			{
				largestThumbnail = thumbnail;
				largestSize = thumbnail->meta.size;
			}
		}
		if (largestThumbnail == nullptr)
		{
			warn("No valid thumbnail found for %s", filepath.c_str());
			return false;
		}
		m_queuedLargeThumbnail = largestThumbnail;
		return true;
	}

	bool FileInfoCache::RequestThumbnail(FileInfo& fileInfo, size_t index)
	{
		if (index >= fileInfo.GetThumbnailCount())
		{
			warn("Thumbnail index %d out of range for %s", index, fileInfo.filename.c_str());
			return false;
		}

		Thumbnail* thumbnail = fileInfo.GetThumbnail(index);

		return RequestThumbnail(thumbnail);
	}

	bool FileInfoCache::RequestThumbnail(Thumbnail* thumbnail)
	{
		if (thumbnail == nullptr)
		{
			warn("Thumbnail is null");
			return false;
		}

		thumbnail->context.Init();
		if (thumbnail->filename.IsEmpty() || thumbnail->meta.offset == 0)
		{
			warn("Not enough information to request thumbnail for %s", thumbnail->filename.c_str());
			return false;
		}

		if (!ThumbnailIsValid(*thumbnail))
		{
			error("thumbnail meta invalid.\n");
			return false;
		}

		const char* filename =
			thumbnail->meta.size <= MAX_THUMBNAIL_CACHE_SIZE ? thumbnail->filename.c_str() : largeThumbnailFilename;

		if (!thumbnail->image.New(thumbnail->meta, filename))
		{
			error("Failed to create thumbnail file %s.", filename);
			return false;
		}

		m_currentThumbnail = thumbnail;
		m_thumbnailRequestInProgress = true;
		thumbnail->context.state = ThumbnailState::DataWait;
		m_lastThumbnailRequestTime = TimeHelper::getCurrentTime();
		duet.RequestThumbnail(thumbnail->filename.c_str(), thumbnail->meta.offset);
		return true;
	}

	bool FileInfoCache::ThumbnailRequestInProgress()
	{
		return m_thumbnailRequestInProgress;
	}

	void FileInfoCache::ReceivingThumbnailResponse(bool receiving)
	{
		dbg("%d", receiving);
		m_thumbnailResponseInProgress = receiving;
	}

	Thumbnail* FileInfoCache::GetCurrentThumbnail(bool force)
	{
		if (!m_thumbnailResponseInProgress && !force)
		{
			return nullptr;
		}
		return m_currentThumbnail;
	}

	bool FileInfoCache::StopThumbnailRequest(bool largeOnly)
	{
		if (largeOnly && m_currentThumbnail != nullptr && m_currentThumbnail->meta.size <= MAX_THUMBNAIL_CACHE_SIZE)
		{
			return false;
		}
		m_currentThumbnail = nullptr;
		return true;
	}

	void FileInfoCache::SetCurrentThumbnail(Thumbnail* thumbnail)
	{
		m_currentThumbnail = thumbnail;
	}

	Thumbnail* FileInfoCache::GetNextThumbnail()
	{
		return nullptr;
	}

	void FileInfoCache::Debug()
	{
		dbg("File info cache debug");
		UI::CONSOLE->AddResponse("File info cache:");
		for (auto& it : GetInstance()->m_cache)
		{
			FileInfo* fileInfo = it.second;
			if (fileInfo == nullptr)
				continue;
			UI::CONSOLE->AddResponse(utils::format("  File %s:", fileInfo->filename.c_str()).c_str());
			UI::CONSOLE->AddResponse(utils::format("    size: %u", fileInfo->size).c_str());
			UI::CONSOLE->AddResponse(utils::format("    lastModified: %s", fileInfo->lastModified.c_str()).c_str());
			UI::CONSOLE->AddResponse(utils::format("    height: %.3f", fileInfo->height).c_str());
			UI::CONSOLE->AddResponse(utils::format("    layerHeight: %.3f", fileInfo->layerHeight).c_str());
			UI::CONSOLE->AddResponse(utils::format("    thumbnails: %u", fileInfo->GetThumbnailCount()).c_str());

			for (size_t i = 0; i < fileInfo->GetThumbnailCount(); i++)
			{
				Thumbnail* thumbnail = fileInfo->GetThumbnail(i);
				if (thumbnail == nullptr)
					continue;
				UI::CONSOLE->AddResponse(utils::format("    Thumbnail %u:", i).c_str());
				UI::CONSOLE->AddResponse("      meta:");
				UI::CONSOLE->AddResponse(utils::format("        filename: %s", thumbnail->filename.c_str()).c_str());
				UI::CONSOLE->AddResponse(
					utils::format("        width(%u), height(%u), format(%d), offset(%u), size(%u)",
								  thumbnail->meta.width,
								  thumbnail->meta.height,
								  thumbnail->meta.imageFormat,
								  thumbnail->meta.offset,
								  thumbnail->meta.size)
						.c_str());
				UI::CONSOLE->AddResponse("      context:");
				UI::CONSOLE->AddResponse(
					utils::format("        err(%d), parseErr(%d), size(%u), offset(%u), next(%u), state(%d)",
								  thumbnail->context.err,
								  thumbnail->context.parseErr,
								  thumbnail->context.size,
								  thumbnail->context.offset,
								  thumbnail->context.next,
								  thumbnail->context.state)
						.c_str());
			}
		}

		UI::CONSOLE->AddResponse("  File info request queue:");
		for (auto& filename : m_fileInfoRequestQueue)
		{
			UI::CONSOLE->AddResponse(utils::format("    %s", filename.c_str()).c_str());
		}

		UI::CONSOLE->AddResponse("  Thumbnail request queue:");
		for (auto& thumbnail : m_thumbnailRequestQueue)
		{
			UI::CONSOLE->AddResponse(
				utils::format(
					"    %dx%d %s", thumbnail->meta.width, thumbnail->meta.height, thumbnail->filename.c_str())
					.c_str());
		}

		FileInfo* fileInfo = GetInstance()->GetCurrentFileInfo();
		Thumbnail* thumbnail = GetInstance()->GetCurrentThumbnail();
		UI::CONSOLE->AddResponse(
			utils::format("  Current FileInfo: %s",
						  fileInfo == nullptr ? "null" : GetInstance()->m_currentFileInfo->filename.c_str())
				.c_str());
		UI::CONSOLE->AddResponse(
			utils::format("  Current Thumbnail: %s",
						  thumbnail == nullptr ? "null" : GetInstance()->m_currentThumbnail->filename.c_str())
				.c_str());
		UI::CONSOLE->AddLineBreak();
	}

	static Debug::DebugCommand dbg_file_info_cache("dbg_file_info_cache",
												   []() { FileInfoCache::GetInstance()->Debug(); });
} // namespace Comm
