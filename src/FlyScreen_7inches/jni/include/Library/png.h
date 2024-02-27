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
	bool IsOpen() { return m_imageFile != nullptr; }
	bool Close();
	void appendData(unsigned char data[], int size);

  private:
	const char* m_imageFileName;
	FILE* m_imageFile;
};

#endif /* JNI_INCLUDE_LIBRARY_PNG_H_ */
