/*
 * Debug.h
 *
 *  Created on: Nov 19, 2023
 *      Author: Loïc
 */

#ifndef JNI_LOGIC_DEBUG_HPP_
#define JNI_LOGIC_DEBUG_HPP_

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 5
#endif

#include "utils/Log.h"
#define __dbg(level, fmt, args...)                                                                                     \
	do                                                                                                                 \
	{                                                                                                                  \
		__android_log_print(level, __FILE__, "%s(%d): " fmt, __FUNCTION__, __LINE__, ##args);                          \
	} while (0)

#if DEBUG_LEVEL <= 2
#define verbose(fmt, args...) __dbg(ANDROID_LOG_VERBOSE, fmt, ##args)
#else
#define verbose(fmt, args...)
#endif

#if DEBUG_LEVEL <= 3
#define dbg(fmt, args...) __dbg(ANDROID_LOG_DEBUG, fmt, ##args)
#else
#define dbg(fmt, args...)
#endif

#if DEBUG_LEVEL <= 4
#define info(fmt, args...) __dbg(ANDROID_LOG_INFO, fmt, ##args)
#else
#define info(fmt, args...)
#endif

#if DEBUG_LEVEL <= 5
#define warn(fmt, args...) __dbg(ANDROID_LOG_WARN, fmt, ##args)
#else
#define warn(fmt, args...)
#endif

#if DEBUG_LEVEL <= 6
#define error(fmt, args...) __dbg(ANDROID_LOG_ERROR, fmt, ##args)
#else
#define error(fmt, args...)
#endif

#if DEBUG_LEVEL <= 7
#define fatal(fmt, args...) __dbg(ANDROID_LOG_FATAL, fmt, ##args)
#else
#define fatal(fmt, args...)
#endif

#endif /* JNI_LOGIC_DEBUG_HPP_ */
