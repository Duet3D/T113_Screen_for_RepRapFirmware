/*
 * Debug.h
 *
 *  Created on: Nov 19, 2023
 *      Author: Loïc
 */

#ifndef JNI_LOGIC_DEBUG_HPP_
#define JNI_LOGIC_DEBUG_HPP_

#include "DebugLevels.h"

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_LEVEL_WARN
#endif

#include "utils/Log.h"
#define __dbg(level, fmt, args...)                                                                                     \
	do                                                                                                                 \
	{                                                                                                                  \
		__android_log_print(level, __FILE__, "%s(%d): " fmt, __FUNCTION__, __LINE__, ##args);                          \
	} while (0)

#if DEBUG_LEVEL <= DEBUG_LEVEL_VERBOSE
#define verbose(fmt, args...) __dbg(ANDROID_LOG_VERBOSE, fmt, ##args)
#else
#define verbose(fmt, args...)
#endif

#if DEBUG_LEVEL <= DEBUG_LEVEL_DBG
#define dbg(fmt, args...) __dbg(ANDROID_LOG_DEBUG, fmt, ##args)
#else
#define dbg(fmt, args...)
#endif

#if DEBUG_LEVEL <= DEBUG_LEVEL_INFO
#define info(fmt, args...) __dbg(ANDROID_LOG_INFO, fmt, ##args)
#else
#define info(fmt, args...)
#endif

#if DEBUG_LEVEL <= DEBUG_LEVEL_WARN
#define warn(fmt, args...) __dbg(ANDROID_LOG_WARN, fmt, ##args)
#else
#define warn(fmt, args...)
#endif

#if DEBUG_LEVEL <= DEBUG_LEVEL_ERROR
#define error(fmt, args...) __dbg(ANDROID_LOG_ERROR, fmt, ##args)
#else
#define error(fmt, args...)
#endif

#if DEBUG_LEVEL <= DEBUG_LEVEL_FATAL
#define fatal(fmt, args...) __dbg(ANDROID_LOG_FATAL, fmt, ##args)
#else
#define fatal(fmt, args...)
#endif

#endif /* JNI_LOGIC_DEBUG_HPP_ */
