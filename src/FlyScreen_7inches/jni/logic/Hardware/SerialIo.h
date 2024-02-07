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
#include "../ecv.h"
#undef array
#undef result
#undef value
#undef in

namespace SerialIo {
	struct SerialIoCbs {
		void (*StartReceivedMessage)(void);
		void (*EndReceivedMessage)(void);
		void (*ProcessReceivedValue)(StringRef id, const char val[], const size_t indices[]);
		void (*ProcessArrayEnd)(const char id[], const size_t indices[]);
		void (*ParserErrorEncountered)(int currentState, const char* id, int errors);
	};

	void Init(uint32_t baudRate, struct SerialIoCbs *callbacks);
	//void SetBaudRate(uint32_t baudRate);
	//void SendChar(char c);
	//void SetCRC16(bool enable);
	bool Send(const char* data, size_t len);
	size_t Sendf(const char *fmt, ...) __attribute__((format (printf, 1, 0)));
	//size_t Dbg(const char *fmt, ...) __attribute__((format (printf, 1, 0)));
	//void SendFilename(const char * _ecv_array dir, const char * _ecv_array name);
	void CheckInput(const unsigned char* rxBuffer, unsigned int len, bool log = false);
}

#endif /* JNI_LOGIC_SERIALIO_HPP_ */
