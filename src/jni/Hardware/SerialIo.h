/*
 * SerialIO.h
 *
 * Created: 04/02/2023
 *  Author: Loïc G.
 *
 *  This file is a modified version of the SerialIO.h source file from PanelDue
 *  The original can be found at https://github.com/Duet3D/PanelDueFirmware/blob/master/src/Hardware/SerialIo.h
 */

#ifndef JNI_LOGIC_SERIALIO_HPP_
#define JNI_LOGIC_SERIALIO_HPP_

#include <cstddef>
#include <sys/types.h>
#include <Duet3D/General/String.h>
#include "ecv.h"
#undef array
#undef result
#undef value
#undef in

namespace SerialIo
{
	bool Send(const char* data, size_t len);
	size_t Sendf(const char* fmt, ...) __attribute__((format(printf, 1, 0)));
} // namespace SerialIo

#endif /* JNI_LOGIC_SERIALIO_HPP_ */
