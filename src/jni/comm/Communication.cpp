/*
 * Communication.cpp
 *
 * Created: 30/01/2023
 *  Author: Loïc G.
 *
 *  This file is a based on PanelDue project
 *  The original can be found at https://github.com/Duet3D/PanelDueFirmware/blob/master/src/PanelDue.cpp
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "UI/UserInterface.h"

#include "Comm/Communication.h"
#include <stdarg.h>

#include "Comm/JsonDecoder.h"
#include "Hardware/Duet.h"
#include "Hardware/Reset.h"
#include "Hardware/SerialIo.h"

#include "Comm/ControlCommands.h"
#include "ObjectModel/Alert.h"
#include "ObjectModel/Axis.h"
#include "ObjectModel/BedOrChamber.h"
#include "ObjectModel/PrinterStatus.h"
#include "ObjectModel/Spindle.h"
#include "ObjectModel/Tool.h"
#include "ObjectModel/Utils.h"
#include "UI/OmObserver.h"
#include "uart/CommDef.h"
#include "utils/TimeHelper.h"

// These defines control which detailed M409 requests will be sent
// If one of the fields in the disabled ones need to be fetched the
// corresponding define has to be set to (1)
#define FETCH_BOARDS (1)
#define FETCH_DIRECTORIES (0)
#define FETCH_FANS (1)
#define FETCH_HEAT (1)
#define FETCH_INPUTS (0)
#define FETCH_JOB (1)
#define FETCH_MOVE (1)
#define FETCH_NETWORK (1)
#define FETCH_SCANNER (0)
#define FETCH_SENSORS (1)
#define FETCH_SPINDLES (1)
#define FETCH_STATE (1)
#define FETCH_TOOLS (1)
#define FETCH_VOLUMES (1)

namespace Comm
{

	static long long lastResponseTime = 0;

	static bool initialized = false;

	Seq seqs[] = {
#if FETCH_NETWORK
		{.event = rcvOMKeyNetwork,
		 .seqid = rcvSeqsNetwork,
		 .lastSeq = 0,
		 .state = SeqStateInit,
		 .key = "network",
		 .flags = "v"},
#endif
#if FETCH_BOARDS
		{.event = rcvOMKeyBoards,
		 .seqid = rcvSeqsBoards,
		 .lastSeq = 0,
		 .state = SeqStateInit,
		 .key = "boards",
		 .flags = "v"},
#endif
#if FETCH_MOVE
		{.event = rcvOMKeyMove, .seqid = rcvSeqsMove, .lastSeq = 0, .state = SeqStateInit, .key = "move", .flags = "v"},
#endif
#if FETCH_HEAT
		{.event = rcvOMKeyHeat, .seqid = rcvSeqsHeat, .lastSeq = 0, .state = SeqStateInit, .key = "heat", .flags = "v"},
#endif
#if FETCH_TOOLS
		{.event = rcvOMKeyTools,
		 .seqid = rcvSeqsTools,
		 .lastSeq = 0,
		 .state = SeqStateInit,
		 .key = "tools",
		 .flags = "v"},
#endif
#if FETCH_SPINDLES
		{.event = rcvOMKeySpindles,
		 .seqid = rcvSeqsSpindles,
		 .lastSeq = 0,
		 .state = SeqStateInit,
		 .key = "spindles",
		 .flags = "v"},
#endif
#if FETCH_DIRECTORIES
		{.event = rcvOMKeyDirectories,
		 .seqid = rcvSeqsDirectories,
		 .lastSeq = 0,
		 .state = SeqStateInit,
		 .key = "directories",
		 .flags = "v"},
#endif
#if FETCH_FANS
		{.event = rcvOMKeyFans, .seqid = rcvSeqsFans, .lastSeq = 0, .state = SeqStateInit, .key = "fans", .flags = "v"},
#endif
#if FETCH_INPUTS
		{.event = rcvOMKeyInputs,
		 .seqid = rcvSeqsInputs,
		 .lastSeq = 0,
		 .state = SeqStateInit,
		 .key = "inputs",
		 .flags = "v"},
#endif
#if FETCH_JOB
		{.event = rcvOMKeyJob, .seqid = rcvSeqsJob, .lastSeq = 0, .state = SeqStateInit, .key = "job", .flags = "v"},
#endif
#if FETCH_SCANNER
		{.event = rcvOMKeyScanner,
		 .seqid = rcvSeqsScanner,
		 .lastSeq = 0,
		 .state = SeqStateInit,
		 .key = "scanner",
		 .flags = "v"},
#endif
#if FETCH_SENSORS
		{.event = rcvOMKeySensors,
		 .seqid = rcvSeqsSensors,
		 .lastSeq = 0,
		 .state = SeqStateInit,
		 .key = "sensors",
		 .flags = "v"},
#endif
#if FETCH_STATE
		{.event = rcvOMKeyState,
		 .seqid = rcvSeqsState,
		 .lastSeq = 0,
		 .state = SeqStateInit,
		 .key = "state",
		 .flags = "vn"},
#endif
#if FETCH_VOLUMES
		{.event = rcvOMKeyVolumes,
		 .seqid = rcvSeqsVolumes,
		 .lastSeq = 0,
		 .state = SeqStateInit,
		 .key = "volumes",
		 .flags = "v"},
#endif
	};

	Seq* currentReqSeq = nullptr;
	Seq* currentRespSeq = nullptr;

	struct Seq* GetNextSeq(struct Seq* current)
	{
		if (current == nullptr)
		{
			current = seqs;
		}

		for (size_t i = current - seqs; i < ARRAY_SIZE(seqs); ++i)
		{
			current = &seqs[i];
			if (current->state == SeqStateError)
			{
				warn("seq %s had an error\n", current->key);
				// skip and re-init if last request had an error
				current->state = SeqStateInit;
				continue;
			}
			if (current->state == SeqStateInit || current->state == SeqStateUpdate)
			{
				dbg("seq %s\n", current->key);
				return current;
			}
		}
		return nullptr;
	}

	Seq* FindSeqByKey(const char* key)
	{
		verbose("key %s\n", key);

		for (size_t i = 0; i < ARRAY_SIZE(seqs); ++i)
		{
			if (strcasecmp(seqs[i].key, key) == 0)
			{
				return &seqs[i];
			}
		}

		return nullptr;
	}

	void UpdateSeq(const ReceivedDataEvent seqid, int32_t val)
	{
		for (size_t i = 0; i < ARRAY_SIZE(seqs); ++i)
		{
			if (seqs[i].seqid == seqid)
			{
				if (seqs[i].lastSeq != val)
				{
					dbg("%s %d -> %d\n", seqs[i].key, seqs[i].lastSeq, val);
					seqs[i].lastSeq = val;
					seqs[i].state = SeqStateUpdate;
				}
			}
		}
	}

	void ResetSeqs()
	{
		for (size_t i = 0; i < ARRAY_SIZE(seqs); ++i)
		{
			seqs[i].lastSeq = 0;
			seqs[i].state = SeqStateInit;
		}
	}

	// Try to get an integer value from a string. If it is actually a floating point value, round it.
	// NB: We are using the old fashioned way (using strtol/strtof) instead of the safe functions provided
	// by the RRFLibraries because of the uClibc++ library not providing the things we need for that.
	bool GetInteger(const char s[], int32_t& rslt)
	{
		if (s[0] == 0)
			return false; // empty string

		char* endptr;
		rslt = (int)strtol(s, &endptr, 10);
		if (*endptr == 0)
			return true; // we parsed an integer

		if (strlen(s) > 10)
			return false; // avoid strtod buggy behaviour on long input strings

		float d = strtof(s, &endptr); // try parsing a floating point number
		if (*endptr == 0)
		{
			rslt = (int)((d < 0.0) ? d - 0.5 : d + 0.5);
			return true;
		}
		return false;
	}

	// Try to get an unsigned integer value from a string
	// NB: We are using the old fashioned way (using strtol/strtof) instead of the safe functions provided
	// by the RRFLibraries because of the uClibc++ library not providing the things we need for that.
	bool GetUnsignedInteger(const char s[], unsigned int& rslt)
	{
		if (s[0] == 0)
			return false; // empty string
		char* endptr;
		rslt = (int)strtoul(s, &endptr, 10);
		return *endptr == 0;
	}

	// Try to get a floating point value from a string. if it is actually a floating point value, round it.
	// NB: We are using the old fashioned way (using strtol/strtof) instead of the safe functions provided
	// by the RRFLibraries because of the uClibc++ library not providing the things we need for that.
	bool GetFloat(const char s[], float& rslt)
	{
		if (s[0] == 0)
			return false; // empty string

		// GNU strtod is buggy, it's very slow for some long inputs, and some versions have a buffer overflow bug.
		// We presume strtof may be buggy too. Tame it by rejecting any strings that much longer than we expect to
		// receive.
		if (strlen(s) > 10)
			return false;

		char* endptr;
		rslt = strtof(s, &endptr);
		return *endptr == 0; // we parsed a float
	}

	// Try to get a bool value from a string.
	bool GetBool(const char s[], bool& rslt)
	{
		if (s[0] == 0)
			return false; // empty string

		rslt = (strcasecmp(s, "true") == 0);
		return true;
	}

	void Reconnect()
	{
		warn("Reconnecting");
		KickWatchdog();
		//		lastOutOfBufferResponse = 0;
		OM::SetStatus(OM::PrinterStatus::connecting);
		// duet.SendGcode("M29");
		duet.Reconnect();
		ResetSeqs();
	}

	void KickWatchdog()
	{
		lastResponseTime = TimeHelper::getCurrentTime();
	}

	//------------------------------------------------------------------------------------------------------------------

	static std::vector<std::string> thumbnailQueue;

	void QueueThumbnailRequest(std::string filename)
	{
		thumbnailQueue.push_back(filename);
		info("Queued thumbnail request for %s", filename.c_str());
	}

	void RequestNextThumbnailChunk()
	{
		FILEINFO_CACHE->Spin();
	}

	void CancelThumbnailRequest()
	{
		warn("Requesting thumbnail cancel");
		FILEINFO_CACHE->StopThumbnailRequest();
	}

	void sendNext()
	{
		long long now = TimeHelper::getCurrentTime();
		if (now > (lastResponseTime + duet.GetPollInterval() + printerResponseTimeout))
		{
			Reconnect();
		}

		currentReqSeq = GetNextSeq(currentReqSeq);
		if (currentReqSeq != nullptr)
		{
			Comm::duet.RequestModel("state", "f"); // Check if state is halted, if so we need to send M999
			info("requesting %s\n", currentReqSeq->key);
			Comm::duet.RequestModel(currentReqSeq->key, currentReqSeq->flags);
		}
		else
		{
			// Once we get here the first time we will have work all seqs once
			if (!initialized)
			{
				info("seqs init DONE\n");
				initialized = true;
			}

			Comm::duet.RequestModel("d99f");
		}
	}

	void init()
	{
		// Sort the fieldTable prior searching using binary search
		system("mkdir /tmp/thumbnails");
		SortFieldTable();
	}
} // namespace Comm
