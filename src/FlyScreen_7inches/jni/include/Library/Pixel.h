/*
 * Pixel.h
 *
 *  Created on: 20 Feb 2024
 *      Author: andy
 */

#ifndef JNI_INCLUDE_LIBRARY_PIXEL_H_
#define JNI_INCLUDE_LIBRARY_PIXEL_H_

typedef union {
	struct
	{
		unsigned char b, g, r, a;
	} rgba;
	unsigned int v;
} rgba_t;

#endif /* JNI_INCLUDE_LIBRARY_PIXEL_H_ */
