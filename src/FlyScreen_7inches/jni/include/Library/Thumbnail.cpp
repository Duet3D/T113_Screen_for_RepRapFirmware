#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_VERBOSE
#include "Debug.h"

#include "Library/Thumbnail.h"

extern "C"
{
#include "base64.h"
}

#include "sys/param.h"

#define QOI_IMPLEMENTATION 1
#include "qoi.h"

static int ThumbnailDecodeChunkPng(struct Thumbnail& thumbnail, struct ThumbnailData& data);
static int ThumbnailDecodeChunkQoi(struct Thumbnail& thumbnail, struct ThumbnailData& data);

std::string GetFileName(const char* filepath)
{
	std::string filename = filepath;
	size_t pos = filename.find_last_of("/");
	if (pos != std::string::npos)
	{
		filename = filename.substr(pos + 1);
	}
	return std::string("/tmp/thumbnails/") + filename;
}

bool Thumbnail::New(uint16_t width, uint16_t height, const char* imageFileName)
{
	Close();
	filename = GetFileName(imageFileName);
	switch (imageFormat)
	{
	case ImageFormat::Png:
		return png.New(filename.c_str());
	case ImageFormat::Qoi:
		return bmp.New(width, height, filename.c_str());
	default:
		return false;
	}
}

bool Thumbnail::SetImageFormat(const char* format)
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

bool Thumbnail::Close()
{
	switch (imageFormat)
	{
	case ImageFormat::Png:
		return png.Close();
	case ImageFormat::Qoi:
		return bmp.Close();
	default:
		return false;
	}
}

bool ThumbnailIsValid(struct Thumbnail& thumbnail)
{
	switch (thumbnail.imageFormat)
	{
	case Thumbnail::ImageFormat::Qoi:
	case Thumbnail::ImageFormat::Png:
		break;
	default:
		return false;
	}

	if (thumbnail.height == 0 || thumbnail.width == 0)
	{
		return false;
	}

	return true;
}

bool ThumbnailDataIsValid(struct ThumbnailData &data)
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

int ThumbnailInit(struct Thumbnail &thumbnail)
{
	thumbnail.width = 0;
	thumbnail.height = 0;
	thumbnail.pixel_count = 0;
	thumbnail.imageFormat = Thumbnail::ImageFormat::Invalid;

	return qoi_decode_init(&thumbnail.qoi);
}

int ThumbnailDecodeChunk(struct Thumbnail& thumbnail, struct ThumbnailData& data)
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

	int ret = base64_decode((const char *)data.buffer, data.size, data.buffer);
	if (ret < 0)
	{
		error("decode error %d size %d data\n%s\n", ret, data.size, data.buffer);
		return -3;
	}

	info("*** received size %d, base64 decoded size %d\n", data.size, ret);

	data.size = ret;

	switch (thumbnail.imageFormat)
	{
	case Thumbnail::ImageFormat::Png:
		return ThumbnailDecodeChunkPng(thumbnail, data);
	case Thumbnail::ImageFormat::Qoi:
		return ThumbnailDecodeChunkQoi(thumbnail, data);
	default:
		// Shouldn't get here
		return -4;
	}
}

static int ThumbnailDecodeChunkPng(struct Thumbnail& thumbnail, struct ThumbnailData& data)
{
	thumbnail.png.appendData(data.buffer, data.size);
	return 0;
}

static int ThumbnailDecodeChunkQoi(struct Thumbnail& thumbnail, struct ThumbnailData& data)
{
	int ret;
	int size_done = 0;
	int pixel_decoded = 0;
	rgba_t rgba_buffer[1024];

	do
	{
		info("buffer %08x (size %d, done%d) pixbuf %08x (size %d, decoded %d)\n",
			 data.buffer,
			 data.size,
			 size_done,
			 rgba_buffer,
			 sizeof(rgba_buffer),
			 pixel_decoded);
		ret = qoi_decode_chunked(&thumbnail.qoi,
								 (data.buffer) + size_done,
								 data.size - size_done,
								 rgba_buffer,
								 sizeof(rgba_buffer),
								 &pixel_decoded);
		if (ret < 0)
		{
			error("failed qoi decoding state %d %d.\n", qoi_decode_state_get(&thumbnail.qoi), ret);
			return -5;
		}

		if (thumbnail.qoi.height != thumbnail.height || thumbnail.qoi.width != thumbnail.width)
		{
			error("thumbnail height %d, qoi height %d", thumbnail.height, thumbnail.qoi.height);
			error("thumbnail width %d, qoi width %d", thumbnail.width, thumbnail.qoi.width);
			return -6;
		}

		size_done += ret;

		thumbnail.pixel_count += pixel_decoded;

		info("decoded %d bytes, done %d/%d; decoded %d missing %d(%02x) count %d/%d/%d\n",
			 ret,
			 size_done,
			 data.size,
			 pixel_decoded,
			 thumbnail.qoi.last_bytes_size,
			 thumbnail.qoi.last_bytes[0] & 0xc0,
			 thumbnail.qoi.pixels_count,
			 thumbnail.pixel_count,
			 thumbnail.height * thumbnail.width);

		thumbnail.bmp.appendPixels(rgba_buffer, pixel_decoded);
	} while (size_done < data.size && qoi_decode_state_get(&thumbnail.qoi) == qoi_decoder_body);

	info("done %d/%d pixels %d/%d\n", size_done, data.size, thumbnail.pixel_count, thumbnail.height * thumbnail.width);

	return qoi_decode_state_get(&thumbnail.qoi) != qoi_decoder_done;
}
