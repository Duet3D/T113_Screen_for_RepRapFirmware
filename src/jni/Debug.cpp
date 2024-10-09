/*
 * Debug.cpp
 *
 *  Created on: 10 May 2024
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "Storage.h"
#include <storage/StoragePreferences.h>

static DebugLevel s_debugLevel = DebugLevel::Info;

void SetDebugLevel(DebugLevel level)
{
	StoragePreferences::putInt(ID_DEBUG_LEVEL, (int)level);
	s_debugLevel = level;
}

const DebugLevel& GetDebugLevel()
{
	return s_debugLevel;
}

void verbose_inner(const char* tag, const char* fmt, ...)
{
	if (GetDebugLevel() > DebugLevel::Verbose)
		return;
	va_list args;
	va_start(args, fmt);
	__dbg(ANDROID_LOG_VERBOSE, tag, fmt, args);
	va_end(args);
}

void dbg_inner(const char* tag, const char* fmt, ...)
{
	if (GetDebugLevel() > DebugLevel::Debug)
		return;
	va_list args;
	va_start(args, fmt);
	__dbg(ANDROID_LOG_DEBUG, tag, fmt, args);
	va_end(args);
}

void info_inner(const char* tag, const char* fmt, ...)
{
	if (GetDebugLevel() > DebugLevel::Info)
		return;
	va_list args;
	va_start(args, fmt);
	__dbg(ANDROID_LOG_INFO, tag, fmt, args);
	va_end(args);
}

void warn_inner(const char* tag, const char* fmt, ...)
{
	if (GetDebugLevel() > DebugLevel::Warn)
		return;
	va_list args;
	va_start(args, fmt);
	__dbg(ANDROID_LOG_WARN, tag, fmt, args);
	va_end(args);
}

void error_inner(const char* tag, const char* fmt, ...)
{
	if (GetDebugLevel() > DebugLevel::Error)
		return;
	va_list args;
	va_start(args, fmt);
	__dbg(ANDROID_LOG_ERROR, tag, fmt, args);
	va_end(args);
}

void fatal_inner(const char* tag, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	__dbg(ANDROID_LOG_FATAL, tag, fmt, args);
	va_end(args);
}