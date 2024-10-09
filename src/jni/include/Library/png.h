/*
 * bmp.h
 *
 *  Created on: 20 Feb 2024
 *      Author: andy
 */

#ifndef JNI_INCLUDE_LIBRARY_PNG_H_
#define JNI_INCLUDE_LIBRARY_PNG_H_

#include "Pixel.h"
#include <stdio.h>
#include <vector>

class PNG
{
  public:
	PNG();
	PNG(const char* imageFileName);
	~PNG();
	bool New(const char* imageFileName);
	bool Open();
	bool IsOpen()const { return m_imageFile != nullptr; }
	bool Close();
	size_t appendData(unsigned char data[], int size);
	size_t dataSize = 0;

  private:
	const char* m_imageFileName;
	FILE* m_imageFile;
};

#endif /* JNI_INCLUDE_LIBRARY_PNG_H_ */
