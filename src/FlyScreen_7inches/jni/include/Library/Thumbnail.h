#ifndef THUMBNAIL_HPP
#define THUMBNAIL_HPP 1

#include <sys/types.h>
#include <cstddef>

#include "bmp.h"
#include "png.h"
#include "qoi.h"

struct Thumbnail
{
	uint16_t width;
	uint16_t height;

	uint32_t pixel_count;

	enum ImageFormat {
		Invalid = 0,
		Qoi,
		Png,
	} imageFormat;

	qoi_desc qoi;
	PNG png;
	BMP bmp;

	bool New(uint16_t width, uint16_t height, const char* imageFileName);
	bool SetImageFormat(const char* format);
	bool Close();
};

struct ThumbnailData
{
	uint16_t size;
	unsigned char buffer[4096]; // 1024 bytes for UART, 2600 bytes for network, some extra since we have lots of memory
};

typedef bool (*ThumbnailProcessCb)(const struct Thumbnail &thumbnail, uint32_t pixels_offset, const rgba_t *pixels, size_t pixels_count);

bool ThumbnailIsValid(struct Thumbnail &thumbnail);
bool ThumbnailDataIsValid(struct ThumbnailData &data);

int ThumbnailInit(struct Thumbnail &thumbnail);
int ThumbnailDecodeChunk(struct Thumbnail& thumbnail, struct ThumbnailData& data);

#endif /* ifndef THUMBNAIL_HPP */
