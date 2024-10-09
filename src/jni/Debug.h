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
void dbg_inner(const char* tag, const char* fmt, ...);
void info_inner(const char* tag, const char* fmt, ...);
void warn_inner(const char* tag, const char* fmt, ...);
void error_inner(const char* tag, const char* fmt, ...);
void fatal_inner(const char* tag, const char* fmt, ...);

#ifdef DEBUG
#define verbose(fmt, args...)                                                                                          \
	do                                                                                                                 \
	{                                                                                                                  \
		verbose_inner(__FILE__, "%s(%d): " fmt, __FUNCTION__, __LINE__, ##args);                                       \
	} while (0)
#define dbg(fmt, args...)                                                                                              \
	do                                                                                                                 \
	{                                                                                                                  \
		dbg_inner(__FILE__, "%s(%d): " fmt, __FUNCTION__, __LINE__, ##args);                                           \
	} while (0)
#else
#define verbose(fmt, args...)
#define dbg(fmt, args...)
#endif

#define info(fmt, args...)                                                                                             \
	do                                                                                                                 \
	{                                                                                                                  \
		info_inner(__FILE__, "%s(%d): " fmt, __FUNCTION__, __LINE__, ##args);                                          \
	} while (0)
#define warn(fmt, args...)                                                                                             \
	do                                                                                                                 \
	{                                                                                                                  \
		warn_inner(__FILE__, "%s(%d): " fmt, __FUNCTION__, __LINE__, ##args);                                          \
	} while (0)
#define error(fmt, args...)                                                                                            \
	do                                                                                                                 \
	{                                                                                                                  \
		error_inner(__FILE__, "%s(%d): " fmt, __FUNCTION__, __LINE__, ##args);                                         \
	} while (0)
#define fatal(fmt, args...)                                                                                            \
	do                                                                                                                 \
	{                                                                                                                  \
		fatal_inner(__FILE__, "%s(%d): " fmt, __FUNCTION__, __LINE__, ##args);                                         \
	} while (0)

#endif /* JNI_DEBUG_HPP_ */
