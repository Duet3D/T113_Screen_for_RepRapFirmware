#ifndef THUMBNAIL_HPP
#define THUMBNAIL_HPP 1

#include "control/ZKBase.h"
#include <cstddef>
#include <sys/types.h>

#include "bmp.h"
#include "png.h"
#include "qoi.h"
#include <string>

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
	std::string filename;

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

bool IsThumbnailCached(const char* filename, bool includeBlank = false);
void SetThumbnail(ZKBase* base, const char* filename);
bool ClearAllCachedThumbnails();
bool DeleteCachedThumbnail(const char* filename);
bool CreateBlankThumbnailCache(const char* filename);

#endif /* ifndef THUMBNAIL_HPP */
