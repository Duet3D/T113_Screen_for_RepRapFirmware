/*
 * bmp.h
 *
 *  Created on: 20 Feb 2024
 *      Author: andy
 */

#ifndef JNI_INCLUDE_LIBRARY_BMP_H_
#define JNI_INCLUDE_LIBRARY_BMP_H_

#include "Pixel.h"
#include <stdio.h>
#include <vector>

class BMP
{
  public:
	BMP();
	BMP(int width, int height, const char* imageFileName);
	~BMP();
	bool New(int width, int height, const char* imageFileName);
	bool Open();
	bool IsOpen() { return m_imageFile != nullptr; }
	bool Close();
	void generateBitmapImage(rgba_t* image);
	void generateBitmapHeaders();
	void appendPixels(rgba_t* pixels, int count);
	void pad();

  private:
	unsigned char* createBitmapFileHeader();
	unsigned char* createBitmapInfoHeader();
	void writeRow(unsigned char* pixels);
	void DeleteBuffer();

	int m_width;
	int m_height;
	const char* m_imageFileName;
	int m_widthInBytes;
	int m_paddingSize;
	int m_stride;
	FILE* m_imageFile;
	int m_pixelIndex;
	rgba_t* m_pixelBuffer;
};

#endif /* JNI_INCLUDE_LIBRARY_BMP_H_ */
