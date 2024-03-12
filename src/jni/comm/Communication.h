/*
 * Communication.h
 *
 *  Created on: Jan 30, 2023
 *      Author: Loïc G.
 */

#ifndef JNI_COMM_COMMUNICATION_HPP_
#define JNI_COMM_COMMUNICATION_HPP_

#include "Library/Thumbnail.h"

#include "Comm/Commands.h"
#include "Duet3D/General/String.h"
#include "Duet3D/General/StringRef.h"
#include "UI/UserInterfaceConstants.h"
#include <stdint.h>
#include <string>

namespace Comm
{
	enum ThumbnailState
	{
		Init = 0,
		Header,
		DataRequest,
		DataWait,
		Data
	};

	struct ThumbnailContext
	{
		String<MaxFileNameLength> filename;
		enum ThumbnailState state;
		int16_t parseErr;
		int32_t err;
		uint32_t size;
		uint32_t offset;
		uint32_t next;

		void Init()
		{
			filename.Clear();
			state = ThumbnailState::Init;
			parseErr = 0;
			err = 0;
			size = 0;
			offset = 0;
			next = 0;
		};
	};

	extern ThumbnailData thumbnailData;
	extern Thumbnail thumbnail;
	extern ThumbnailContext thumbnailContext;
	extern bool stopThumbnailRequest;
	extern bool thumbnailRequestInProgress;

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

	extern Seq* currentReqSeq;
	extern Seq* currentRespSeq;

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
	void RequestNextThumbnailChunk();
	void CancelThumbnailRequest();
	void sendNext();
	void init();
} // namespace Comm

#endif /* JNI_COMM_COMMUNICATION_HPP_ */
