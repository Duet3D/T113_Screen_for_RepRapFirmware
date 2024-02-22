#ifndef THUMBNAIL_HPP
#define THUMBNAIL_HPP 1

#include <sys/types.h>
#include <cstddef>

#include "bmp.h"
#include "qoi.h"

struct Thumbnail
{
	uint16_t width;
	uint16_t height;

	uint32_t pixel_count;

	enum ImageFormat {
		Invalid = 0,
		Qoi,
	} imageFormat;

	qoi_desc qoi;
	BMP bmp;
};

struct ThumbnailData
{
	uint16_t size;
	unsigned char buffer[2048];
};

typedef bool (*ThumbnailProcessCb)(const struct Thumbnail &thumbnail, uint32_t pixels_offset, const rgba_t *pixels, size_t pixels_count);

bool ThumbnailIsValid(struct Thumbnail &thumbnail);
bool ThumbnailDataIsValid(struct ThumbnailData &data);

int ThumbnailInit(struct Thumbnail &thumbnail);
int ThumbnailDecodeChunk(struct Thumbnail& thumbnail, struct ThumbnailData& data);

#endif /* ifndef THUMBNAIL_HPP */
