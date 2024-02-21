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

bool ThumbnailIsValid(struct Thumbnail &thumbnail)
{
	if (thumbnail.imageFormat != Thumbnail::ImageFormat::Qoi)
	{
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

	info("*** received size %d decoded size %d\n", data.size, ret);

	data.size = ret;

	int size_done = 0;
	int pixel_decoded = 0;
	rgba_t rgba_buffer[64];

	do
	{
		info("buffer %08x size %d/%d pixbuf %08x pixbuf size %d decoded %08x\n",
			 data.buffer,
			 data.size,
			 size_done,
			 rgba_buffer,
			 &pixel_decoded);
		ret = qoi_decode_chunked(&thumbnail.qoi, (data.buffer) + size_done, data.size - size_done, rgba_buffer, sizeof(rgba_buffer), &pixel_decoded);
		if (ret < 0)
		{
			error("failed qoi decoding state %d %d.\n", qoi_decode_state_get(&thumbnail.qoi), ret);
			return -4;
		}

		if (thumbnail.qoi.height != thumbnail.height ||
		    thumbnail.qoi.width != thumbnail.width)
		{
			error("thumbnail height %d, qoi height %d", thumbnail.height, thumbnail.qoi.height);
			error("thumbnail width %d, qoi width %d", thumbnail.width, thumbnail.qoi.width);
			return -5;
		}

		size_done += ret;

		thumbnail.bmp.appendPixels(rgba_buffer, pixel_decoded);

		thumbnail.pixel_count += pixel_decoded;

		info("ret %d done %d/%d decoded %d missing %d(%02x) count %d/%d/%d\n",
			 ret,
			 size_done,
			 data.size,
			 pixel_decoded,
			 thumbnail.qoi.last_bytes_size,
			 thumbnail.qoi.last_bytes[0] & 0xc0,
			 thumbnail.qoi.pixels_count,
			 thumbnail.pixel_count,
			 thumbnail.height * thumbnail.width);

	} while (size_done < data.size && qoi_decode_state_get(&thumbnail.qoi) == qoi_decoder_body);

	info("done %d/%d pixels %d/%d\n", size_done, data.size, thumbnail.pixel_count, thumbnail.height * thumbnail.width);

	return qoi_decode_state_get(&thumbnail.qoi) != qoi_decoder_done;
}
