/*
 * Debug.h
 *
 *  Created on: Nov 19, 2023
 *      Author: Loïc
 */

#ifndef JNI_DEBUG_HPP_
#define JNI_DEBUG_HPP_

#include "DebugLevels.h"

#include "utils/Log.h"
#include <cstdarg>

#define __dbg(level, tag, fmt, args...)                                                                                \
	do                                                                                                                 \
	{                                                                                                                  \
		__android_log_vprint(level, tag, fmt, args);                                                                   \
	} while (0)

constexpr const char* DebugLevelStrings[] = {
	"dbg_verbose",
	"dbg_debug",
	"dbg_info",
	"dbg_warn",
	"dbg_error",
	"dbg_fatal",
};

enum class DebugLevel
{
	Verbose = 0,
	Debug,
	Info,
	Warn,
	Error,
	Fatal,
	COUNT
};

void SetDebugLevel(DebugLevel level);
const DebugLevel& GetDebugLevel();

void verbose_inner(const char* tag, const char* fmt, ...);
#define verbose(fmt, args...) verbose_inner(__FILE__, "%s(%d): " fmt, __FUNCTION__, __LINE__, ##args)

void dbg_inner(const char* tag, const char* fmt, ...);
#define dbg(fmt, args...) dbg_inner(__FILE__, "%s(%d): " fmt, __FUNCTION__, __LINE__, ##args)

void info_inner(const char* tag, const char* fmt, ...);
#define info(fmt, args...) info_inner(__FILE__, "%s(%d): " fmt, __FUNCTION__, __LINE__, ##args)

void warn_inner(const char* tag, const char* fmt, ...);
#define warn(fmt, args...) warn_inner(__FILE__, "%s(%d): " fmt, __FUNCTION__, __LINE__, ##args)

void error_inner(const char* tag, const char* fmt, ...);
#define error(fmt, args...) error_inner(__FILE__, "%s(%d): " fmt, __FUNCTION__, __LINE__, ##args)

void fatal_inner(const char* tag, const char* fmt, ...);
#define fatal(fmt, args...) fatal_inner(__FILE__, "%s(%d): " fmt, __FUNCTION__, __LINE__, ##args)

#endif /* JNI_DEBUG_HPP_ */
