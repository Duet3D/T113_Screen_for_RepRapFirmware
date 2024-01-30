/*
 * Communication.h
 *
 *  Created on: Jan 30, 2023
 *      Author: Loïc G.
 */

#ifndef JNI_LOGIC_COMMUNICATION_HPP_
#define JNI_LOGIC_COMMUNICATION_HPP_

#include<stdint.h>
#include <string>
#include "Duet3D/General/StringRef.h"

namespace Comm
{
	struct Seq;

	extern Seq *currentReqSeq;
	extern Seq *currentRespSeq;

	bool GetInteger(const char s[], int32_t &rslt);
	bool GetUnsignedInteger(const char s[], unsigned int &rslt);
	bool GetFloat(const char s[], float &rslt);
	bool GetBool(const char s[], bool &rslt);

	void ProcessReceivedValue(StringRef id, const char data[], const size_t indices[]);

	const uint32_t GetPollInterval();

	extern struct Seq* GetNextSeq(struct Seq *current);
	extern void sendNext();
	void init();

}

#endif /* JNI_LOGIC_COMMUNICATION_HPP_ */
