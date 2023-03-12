/*
 * Communication.h
 *
 *  Created on: Jan 30, 2023
 *      Author: Loïc G.
 */

#ifndef JNI_LOGIC_COMMUNICATION_HPP_
#define JNI_LOGIC_COMMUNICATION_HPP_

#include <string>

namespace Comm
{
	struct Seq;

	extern Seq *currentReqSeq;
	extern Seq *currentRespSeq;

	extern struct Seq* GetNextSeq(struct Seq *current);
	extern void sendNext();
	void init();

}

#endif /* JNI_LOGIC_COMMUNICATION_HPP_ */
