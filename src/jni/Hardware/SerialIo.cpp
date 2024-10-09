/*
 * SerialIO.cpp
 *
 * Created: 04/02/2023
 *  Author: Loïc G.
 *
 *  This file is a modified version of the SerialIO.cpp source file from PanelDue
 *  The original can be found at https://github.com/Duet3D/PanelDueFirmware/blob/master/src/Hardware/SerialIo.cpp
 *
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_INFO

#include "SerialIo.h"
#include "uart/UartContext.h"
#include <string>

#include "Debug.h"

namespace SerialIo
{
	bool Send(const char* data, size_t len)
	{
		return UARTCONTEXT->send((unsigned char*)data, len);
	}

	size_t Sendf(const char *fmt, ...) {
		va_list vargs;
		va_start(vargs, fmt);

		std::string buf;
		size_t ret = vsnprintf(0, 0, fmt, vargs);
		if (ret >= buf.capacity())
			buf.reserve(ret + sizeof(char));
		buf.resize(ret);
		vsnprintf((char*)buf.data(), buf.capacity(), fmt, vargs);
		info("Sending %s", buf.c_str());
		UARTCONTEXT->send((unsigned char*)buf.c_str(), ret);

		va_end(vargs);

		return ret;
	}
} // namespace SerialIo
