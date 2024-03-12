/*
 * FileInfo.cpp
 *
 *  Created on: 12 Mar 2024
 *      Author: andy
 */

#include "FileInfo.h"

namespace Comm
{
	Thumbnail* FileInfo::GetOrCreateThumbnail(size_t index)
	{
		if (index >= thumbnails.size())
		{
			thumbnails.resize(index + 1);
			thumbnails[index] = new Thumbnail;
		}

		return thumbnails[index];
	}

	size_t FileInfo::ClearThumbnails(size_t fromIndex)
	{
		size_t count = 0;
		for (size_t i = fromIndex; i < thumbnails.size(); ++i)
		{
			count++;
			delete thumbnails[i];
		}
		thumbnails.resize(fromIndex);
		return count;
	}

	FileInfoCache::FileInfoCache() {}

	void FileInfoCache::Spin() {}

	bool FileInfoCache::IsThumbnailCached(const std::string& filepath, uint32_t lastModified) {}

	void FileInfoCache::SetCurrentFileInfo(const std::string& filepath)
	{
		if (m_cache.find(filepath) == m_cache.end())
		{
			m_cache[filepath] = new FileInfo;
		}
		m_currentFileInfo = m_cache[filepath];
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

	void FileInfoCache::ClearCache()
	{
		for (auto& it : m_cache)
		{
			delete it.second;
		}
		m_cache.clear();
	}

	bool FileInfoCache::QueueThumbnailRequest(const std::string& filepath)
	{
		return true;
	}
	bool FileInfoCache::RequestThumbnail(const FileInfo& fileInfo, size_t index)
	{
		return true;
	}
	bool FileInfoCache::ThumbnailRequestInProgress() {}

	Thumbnail* FileInfoCache::GetCurrentThumbnail()
	{
		return m_currentThumbnail;
	}

	bool FileInfoCache::StopThumbnailRequest()
	{
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
} // namespace Comm
