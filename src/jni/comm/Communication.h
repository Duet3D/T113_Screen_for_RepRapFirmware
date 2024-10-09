/*
 * Communication.h
 *
 *  Created on: Jan 30, 2023
 *      Author: Loïc G.
 */

#ifndef JNI_COMM_COMMUNICATION_HPP_
#define JNI_COMM_COMMUNICATION_HPP_

#include "Comm/Commands.h"
#include <stdint.h>
#include "FileInfo.h"

namespace Comm
{

	enum SeqState
	{
		SeqStateInit,
		SeqStateOk,
		SeqStateUpdate,
		SeqStateError,
		SeqStateDisabled
	};

	struct Seq
	{
		const ReceivedDataEvent event;
		const ReceivedDataEvent seqid;

		uint16_t lastSeq;
		enum SeqState state;

		const char* const key;
		const char* const flags;
	};

	extern Seq* g_currentReqSeq;
	extern Seq* g_currentRespSeq;

	bool GetInteger(const char s[], int32_t& rslt);
	bool GetUnsignedInteger(const char s[], unsigned int& rslt);
	bool GetFloat(const char s[], float& rslt);
	bool GetBool(const char s[], bool& rslt);
	void Reconnect();

	Seq* FindSeqByKey(const char* key);
	void UpdateSeq(const ReceivedDataEvent seqid, int32_t val);
	void ResetSeqs();

	void KickWatchdog();

	Seq* GetNextSeq(struct Seq* current);
	void sendNext();
	void init();
} // namespace Comm

#endif /* JNI_COMM_COMMUNICATION_HPP_ */
