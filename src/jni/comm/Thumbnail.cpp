#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#include "Debug.h"

#define QOI_IMPLEMENTATION 1
#include "Thumbnail.h"

extern "C"
{
#include "Library/base64.h"
}

#include "sys/param.h"
#include <sys/stat.h>

#include "utils.h"

namespace Comm
{
	ThumbnailBuf thumbnailBuf;

	static std::string GetFileName(const char* filepath)
	{
		std::string filename = filepath;
		size_t pos = filename.find_last_of("/");
		if (pos != std::string::npos)
		{
			filename = filename.substr(pos + 1);
		}
		return std::string("/tmp/thumbnails/") + filename;
	}

	bool ThumbnailImage::New(ThumbnailMeta& meta, const char* filename)
	{
		Close();
		imageFilename = GetFileName(filename);
		switch (meta.imageFormat)
		{
		case ThumbnailMeta::ImageFormat::Png:
			return png.New(imageFilename.c_str());
		case ThumbnailMeta::ImageFormat::Qoi:
			return bmp.New(meta.width, meta.height, imageFilename.c_str());
		default:
			return false;
		}
	}

	bool ThumbnailMeta::SetImageFormat(const char* format)
	{
		if (strcmp(format, "qoi") == 0)
		{
			imageFormat = ImageFormat::Qoi;
			return true;
		}
		else if (strcmp(format, "png") == 0)
		{
			imageFormat = ImageFormat::Png;
			return true;
		}
		else
		{
			imageFormat = ImageFormat::Invalid;
			return false;
		}
	}

	bool ThumbnailImage::Close()
	{
		return png.Close() && bmp.Close();
	}

} // namespace Comm

bool ThumbnailIsValid(Comm::Thumbnail& thumbnail)
{
	switch (thumbnail.meta.imageFormat)
	{
	case Comm::ThumbnailMeta::ImageFormat::Qoi:
	case Comm::ThumbnailMeta::ImageFormat::Png:
		break;
	default:
		return false;
	}

	if (thumbnail.meta.height == 0 || thumbnail.meta.width == 0)
	{
		return false;
	}

	return true;
}

bool ThumbnailDataIsValid(Comm::ThumbnailBuf& data)
{
	if (data.size == 0)
	{
		return false;
	}

	if (!data.buffer)
	{
		return false;
	}

	return true;
}

int ThumbnailInit(Comm::Thumbnail& thumbnail)
{
	thumbnail.meta.width = 0;
	thumbnail.meta.height = 0;
	thumbnail.image.pixel_count = 0;
	thumbnail.meta.imageFormat = Comm::ThumbnailMeta::ImageFormat::Invalid;

	return qoi_decode_init(&thumbnail.image.qoi);
}

static int ThumbnailDecodeChunkPng(Comm::Thumbnail& thumbnail, Comm::ThumbnailBuf& data)
{
	size_t ret = thumbnail.image.png.appendData(data.buffer, data.size);
	info("done %d/%d %s\n", ret, data.size, thumbnail.filename.c_str());
	return 0;
}

static int ThumbnailDecodeChunkQoi(Comm::Thumbnail& thumbnail, Comm::ThumbnailBuf& data)
{
	int ret;
	int size_done = 0;
	int pixel_decoded = 0;
	rgba_t rgba_buffer[1024];

	do
	{
		dbg("buffer %08x (size %d, done%d) pixbuf %08x (size %d, decoded %d)\n",
			data.buffer,
			data.size,
			size_done,
			rgba_buffer,
			sizeof(rgba_buffer),
			pixel_decoded);
		ret = qoi_decode_chunked(&thumbnail.image.qoi,
								 (data.buffer) + size_done,
								 data.size - size_done,
								 rgba_buffer,
								 sizeof(rgba_buffer),
								 &pixel_decoded);
		if (ret < 0)
		{
			error("failed qoi decoding state %d %d.\n", qoi_decode_state_get(&thumbnail.image.qoi), ret);
			return -5;
		}

		if (thumbnail.image.qoi.height != thumbnail.meta.height || thumbnail.image.qoi.width != thumbnail.meta.width)
		{
			error("thumbnail height %d, qoi height %d", thumbnail.meta.height, thumbnail.image.qoi.height);
			error("thumbnail width %d, qoi width %d", thumbnail.meta.width, thumbnail.image.qoi.width);
			return -6;
		}

		size_done += ret;

		thumbnail.image.pixel_count += pixel_decoded;

		dbg("decoded %d bytes, done %d/%d; decoded %d missing %d(%02x) count %d/%d/%d\n",
			ret,
			size_done,
			data.size,
			pixel_decoded,
			thumbnail.image.qoi.last_bytes_size,
			thumbnail.image.qoi.last_bytes[0] & 0xc0,
			thumbnail.image.qoi.pixels_count,
			thumbnail.image.pixel_count,
			thumbnail.meta.height * thumbnail.meta.width);

		thumbnail.image.bmp.appendPixels(rgba_buffer, pixel_decoded);
	} while (size_done < data.size && qoi_decode_state_get(&thumbnail.image.qoi) == qoi_decoder_body);

	info("done %d/%d pixels %d/%d %s",
		 size_done,
		 data.size,
		 thumbnail.image.pixel_count,
		 thumbnail.meta.height * thumbnail.meta.width,
		 thumbnail.filename.c_str());

	return qoi_decode_state_get(&thumbnail.image.qoi) != qoi_decoder_done;
}

int ThumbnailDecodeChunk(Comm::Thumbnail& thumbnail, Comm::ThumbnailBuf& data)
{
	if (!ThumbnailIsValid(thumbnail))
	{
		error("meta invalid.\n");
		return -1;
	}

	if (!ThumbnailDataIsValid(data))
	{
		error("data invalid.\n");
		return -2;
	}

	int ret = base64_decode((const char*)data.buffer, data.size, data.buffer);
	if (ret < 0)
	{
		error("decode error %d size %d data\n%s\n", ret, data.size, data.buffer);
		return -3;
	}

	dbg("*** received size %d, base64 decoded size %d\n", data.size, ret);

	data.size = ret;

	switch (thumbnail.meta.imageFormat)
	{
	case Comm::ThumbnailMeta::ImageFormat::Png:
		return ThumbnailDecodeChunkPng(thumbnail, data);
	case Comm::ThumbnailMeta::ImageFormat::Qoi:
		return ThumbnailDecodeChunkQoi(thumbnail, data);
	default:
		// Shouldn't get here
		return -4;
	}
}

bool IsThumbnailCached(const char* filename, bool includeBlank)
{
	std::string thumbnailPath = utils::format("/tmp/thumbnails/%s", filename);
	struct stat sb;
	if (system(utils::format("test -f \"%s\"", thumbnailPath.c_str()).c_str()) == 0)
	{
		if (stat(thumbnailPath.c_str(), &sb) == -1)
		{
			// File doesn't exist
			return false;
		}
		if (!includeBlank && sb.st_size <= 1)
		{
			// File exists but is empty
			return false;
		}
		return true;
	}
	return false;
}

void SetThumbnail(ZKBase* base, const char* filename)
{
	std::string thumbnailPath = utils::format("/tmp/thumbnails/%s", filename);
	base->setBackgroundPic(thumbnailPath.c_str());
}

bool ClearAllCachedThumbnails()
{
	info("Clearing all cached thumbnails");
	return system("rm -f /tmp/thumbnails/*") == 0;
}

bool DeleteCachedThumbnail(const char* filename)
{
	info("Deleting thumbnail for %s", filename);
	return system(utils::format("rm -f \"/tmp/thumbnails/%s\"", filename).c_str()) == 0;
}

bool CreateBlankThumbnailCache(const char* filename)
{
	info("Creating blank thumbnail for %s", filename);
	return system(utils::format("echo \"\" > \"/tmp/thumbnails/%s\"", filename).c_str()) == 0;
}
