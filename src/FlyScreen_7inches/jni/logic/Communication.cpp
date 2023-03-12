/*
 * Communication.cpp
 *
 * Created: 30/01/2023
 *  Author: Loïc G.
 *
 *  This file is a based on PanelDue project
 *  The original can be found at https://github.com/Duet3D/PanelDueFirmware/blob/master/src/PanelDue.cpp
 */

#include "Communication.hpp"

#include <stdarg.h>

#include "Hardware/SerialIo.hpp"
#include "Hardware/Reset.hpp"

#include "uart/CommDef.h"
#include "utils/Log.h"
#include "Configuration.hpp"
#include "ControlCommands.hpp"
#include "Library/Thumbnail.hpp"

// These defines control which detailed M409 requests will be sent
// If one of the fields in the disabled ones need to be fetched the
// corresponding define has to be set to (1)
#define FETCH_BOARDS		(1)
#define FETCH_DIRECTORIES	(0)
#define FETCH_FANS			(0)
#define FETCH_HEAT			(1)
#define FETCH_INPUTS		(0)
#define FETCH_JOB			(1)
#define FETCH_MOVE			(1)
#define FETCH_NETWORK		(1)
#define FETCH_SCANNER		(0)
#define FETCH_SENSORS		(0)
#define FETCH_SPINDLES		(1)
#define FETCH_STATE			(1)
#define FETCH_TOOLS			(1)
#define FETCH_VOLUMES		(1)

namespace Comm {

	const int parserMinErrors = 2;

	static uint32_t lastPollTime = 0;
	static uint32_t lastResponseTime = 0;
	static uint32_t lastOutOfBufferResponse = 0;

	static uint8_t oobCounter = 0;
	static bool outOfBuffers = false;

	static size_t numAxes = 2;
	static int32_t beepFrequency = 0;
	static int32_t beepLength = 0;

	static uint32_t messageSeq = 0;
	static uint32_t newMessageSeq = 0;

	static uint32_t fileSize = 0;
	static uint8_t visibleAxesCounted = 0;

	static int8_t lastBed = -1;
	static int8_t lastChamber = -1;
	static int8_t lastSpindle = -1;
	static int8_t lastTool = -1;
	static bool initialized = false;

	static struct ThumbnailData thumbnailData;
	static struct Thumbnail thumbnail;

	enum ThumbnailState {
		Init = 0,
		Header,
		DataRequest,
		DataWait,
		Data
	};

	static struct ThumbnailContext {
		String<MaxFilenameLength> filename;
		enum ThumbnailState state;
		int16_t parseErr;
		int32_t err;
		uint32_t size;
		uint32_t offset;
		uint32_t next;

		void Init() {
			filename.Clear();
			state = ThumbnailState::Init;
			parseErr = 0;
			err = 0;
			size = 0;
			offset = 0;
			next = 0;
		};
	} thumbnailContext;

	enum ReceivedDataEvent {
		rcvUnknown = 0,

		// Keys for control command messages
		rcvControlCommand,

		// Keys for push messages
		rcvPushMessage,
		rcvPushResponse,
		rcvPushSeq,
		rcvPushBeepDuration,
		rcvPushBeepFrequency,

		// Keys for M20 response
		rcvM20Dir,
		rcvM20Err,
		rcvM20Files,

		// Keys for M36 response
		rcvM36Filament,
		rcvM36Filename,
		rcvM36GeneratedBy,
		rcvM36Height,
		rcvM36LastModified,
		rcvM36LayerHeight,
		rcvM36PrintTime,
		rcvM36SimulatedTime,
		rcvM36Size,
		rcvM36ThumbnailsFormat,
		rcvM36ThumbnailsHeight,
		rcvM36ThumbnailsOffset,
		rcvM36ThumbnailsSize,
		rcvM36ThumbnailsWidth,

		rcvM361ThumbnailData,
		rcvM361ThumbnailErr,
		rcvM361ThumbnailFilename,
		rcvM361ThumbnailNext,
		rcvM361ThumbnailOffset,

		// Keys for M409 response
		rcvKey,
		rcvFlags,
		rcvResult,

		// Available keys
		rcvOMKeyBoards,
		rcvOMKeyDirectories,
		rcvOMKeyFans,
		rcvOMKeyHeat,
		rcvOMKeyInputs,
		rcvOMKeyJob,
		rcvOMKeyLimits,
		rcvOMKeyMove,
		rcvOMKeyNetwork,
		rcvOMKeyReply,
		rcvOMKeyScanner,
		rcvOMKeySensors,
		rcvOMKeySeqs,
		rcvOMKeySpindles,
		rcvOMKeyState,
		rcvOMKeyTools,
		rcvOMKeyVolumes,

		// Keys for boards response
		rcvBoardsFirmwareName,

		// Keys for fans response
		rcvFansRequestedValue,

		// Keys for heat response
		rcvHeatBedHeaters,
		rcvHeatChamberHeaters,
		rcvHeatHeatersActive,
		rcvHeatHeatersCurrent,
		rcvHeatHeatersStandby,
		rcvHeatHeatersState,

		// Keys for job response
		rcvJobDuration,
		rcvJobFileFilename,
		rcvJobFileSize,
		rcvJobFilePosition,
		rcvJobFileSimulatedTime,
		rcvJobLastFileName,
		rcvJobLastFileSimulated,
		rcvJobTimesLeftFilament,
		rcvJobTimesLeftFile,
		rcvJobTimesLeftSlicer,
		rcvJobWarmUpDuration,

		// Keys for move response
		rcvMoveAxesBabystep,
		rcvMoveAxesHomed,
		rcvMoveAxesLetter,
		rcvMoveAxesMachinePosition,
		rcvMoveAxesUserPosition,
		rcvMoveAxesVisible,
		rcvMoveAxesWorkplaceOffsets,
		rcvMoveExtrudersFactor,
		rcvMoveKinematicsName,
		rcvMoveSpeedFactor,
		rcvMoveWorkplaceNumber,

		// Keys for network response
		rcvNetworkName,
		rcvNetworkInterfacesActualIP,

		// Keys for sensors response
		rcvSensorsProbeValue,

		// Keys for seqs response
		rcvSeqsBoards,
		rcvSeqsDirectories,
		rcvSeqsFans,
		rcvSeqsHeat,
		rcvSeqsInputs,
		rcvSeqsJob,
		rcvSeqsMove,
		rcvSeqsNetwork,
		rcvSeqsReply,
		rcvSeqsScanner,
		rcvSeqsSensors,
		rcvSeqsSpindles,
		rcvSeqsState,
		rcvSeqsTools,
		rcvSeqsVolumes,

		// Keys for spindles response
		rcvSpindlesActive,
		rcvSpindlesCurrent,
		rcvSpindlesMax,
		rcvSpindlesMin,
		rcvSpindlesState,
		rcvSpindlesTool,

		// Keys from state response
		rcvStateCurrentTool,
		rcvStateMessageBox,
		rcvStateMessageBoxAxisControls,
		rcvStateMessageBoxMessage,
		rcvStateMessageBoxMode,
		rcvStateMessageBoxSeq,
		rcvStateMessageBoxTimeout,
		rcvStateMessageBoxTitle,
		rcvStateStatus,
		rcvStateUptime,

		// Keys from tools response
		rcvToolsActive,
		rcvToolsExtruders,
		rcvToolsFans,
		rcvToolsHeaters,
		rcvToolsOffsets,
		rcvToolsNumber,
		rcvToolsSpindle,
		rcvToolsSpindleRpm,
		rcvToolsStandby,
		rcvToolsState,
	};

	struct FieldTableEntry {
		const ReceivedDataEvent val;
		const char* key;
	};

	// The following tables will be sorted once on startup so entries can be better grouped for code maintenance
	// A '^' character indicates the position of an _ecv_array index, and a ':' character indicates the start of a sub-field name
	static FieldTableEntry fieldTable[] = {
		// M409 common fields
		{ rcvKey, 							"key" },

		// M409 K"boards" response
		{ rcvBoardsFirmwareName, 			"boards^:firmwareName" },

		// M409 K"fans" response
		{ rcvFansRequestedValue,			"fans^:requestedValue" },

		// M409 K"heat" response
		{ rcvHeatBedHeaters,				"heat:bedHeaters^" },
		{ rcvHeatChamberHeaters,			"heat:chamberHeaters^" },
		{ rcvHeatHeatersActive,				"heat:heaters^:active" },
		{ rcvHeatHeatersCurrent,			"heat:heaters^:current" },
		{ rcvHeatHeatersStandby,			"heat:heaters^:standby" },
		{ rcvHeatHeatersState,				"heat:heaters^:state" },

		// M409 K"job" response
		{ rcvJobFileFilename, 				"job:file:fileName" },
		{ rcvJobFileSize, 					"job:file:size" },
		{ rcvJobFileSimulatedTime, 			"job:file:simulatedTime" },
		{ rcvJobFilePosition,				"job:filePosition" },
		{ rcvJobLastFileName,				"job:lastFileName" },
		{ rcvJobDuration,					"job:duration" },
		{ rcvJobTimesLeftFilament,			"job:timesLeft:filament" },
		{ rcvJobTimesLeftFile,				"job:timesLeft:file" },
		{ rcvJobTimesLeftSlicer,			"job:timesLeft:slicer" },
		{ rcvJobWarmUpDuration,				"job:warmUpDuration" },

		// M409 K"move" response
		{ rcvMoveAxesBabystep, 				"move:axes^:babystep" },
		{ rcvMoveAxesHomed,					"move:axes^:homed" },
		{ rcvMoveAxesLetter,	 			"move:axes^:letter" },
		{ rcvMoveAxesMachinePosition,		"move:axes^:machinePosition" },
		{ rcvMoveAxesUserPosition,			"move:axes^:userPosition" },
		{ rcvMoveAxesVisible, 				"move:axes^:visible" },
		{ rcvMoveAxesWorkplaceOffsets, 		"move:axes^:workplaceOffsets^" },
		{ rcvMoveExtrudersFactor, 			"move:extruders^:factor" },
		{ rcvMoveKinematicsName, 			"move:kinematics:name" },
		{ rcvMoveSpeedFactor, 				"move:speedFactor" },
		{ rcvMoveWorkplaceNumber, 			"move:workplaceNumber" },

		// M409 K"network" response
		{ rcvNetworkName, 					"network:name" },
		{ rcvNetworkInterfacesActualIP,		"network:interfaces^:actualIP" },

		// M409 K"sensors" response
		{ rcvSensorsProbeValue,				"sensors:probes^:value^" },

		// M409 K"seqs" response
		{ rcvSeqsBoards,					"seqs:boards" },
		{ rcvSeqsDirectories,				"seqs:directories" },
		{ rcvSeqsFans,						"seqs:fans" },
		{ rcvSeqsHeat,						"seqs:heat" },
		{ rcvSeqsInputs,					"seqs:inputs" },
		{ rcvSeqsJob,						"seqs:job" },
		{ rcvSeqsMove,						"seqs:move" },
		{ rcvSeqsNetwork,					"seqs:network" },
		{ rcvSeqsReply,						"seqs:reply" },
		{ rcvSeqsScanner,					"seqs:scanner" },
		{ rcvSeqsSensors,					"seqs:sensors" },
		{ rcvSeqsSpindles,					"seqs:spindles" },
		{ rcvSeqsState,						"seqs:state" },
		{ rcvSeqsTools,						"seqs:tools" },
		{ rcvSeqsVolumes,					"seqs:volumes" },

		// M409 K"spindles" response
		{ rcvSpindlesActive, 				"spindles^:active" },
		{ rcvSpindlesCurrent,				"spindles^:current" },
		{ rcvSpindlesMax, 					"spindles^:max" },
		{ rcvSpindlesMin, 					"spindles^:min" },
		{ rcvSpindlesState, 				"spindles^:state" },
		{ rcvSpindlesTool,	 				"spindles^:tool" },

		// M409 K"state" response
		{ rcvStateCurrentTool,				"state:currentTool" },
		{ rcvStateMessageBox,				"state:messageBox" },
		{ rcvStateMessageBoxAxisControls,	"state:messageBox:axisControls" },
		{ rcvStateMessageBoxMessage,		"state:messageBox:message" },
		{ rcvStateMessageBoxMode,			"state:messageBox:mode" },
		{ rcvStateMessageBoxSeq,			"state:messageBox:seq" },
		{ rcvStateMessageBoxTimeout,		"state:messageBox:timeout" },
		{ rcvStateMessageBoxTitle,			"state:messageBox:title" },
		{ rcvStateStatus,					"state:status" },
		{ rcvStateUptime,					"state:upTime" },

		// M409 K"tools" response
		{ rcvToolsActive, 					"tools^:active^" },
		{ rcvToolsExtruders,				"tools^:extruders^" },
		{ rcvToolsFans,						"tools^:fans^" },
		{ rcvToolsHeaters,					"tools^:heaters^" },
		{ rcvToolsNumber, 					"tools^:number" },
		{ rcvToolsOffsets, 					"tools^:offsets^" },
		{ rcvToolsSpindle, 					"tools^:spindle" },
		{ rcvToolsSpindleRpm,				"tools^:spindleRpm" },
		{ rcvToolsStandby, 					"tools^:standby^" },
		{ rcvToolsState, 					"tools^:state" },

		// M20 response
		{ rcvM20Dir,						"dir" },
		{ rcvM20Err,						"err" },
		{ rcvM20Files,						"files^" },

		// M36 response
		{ rcvM36Filament,					"filament^" },
		{ rcvM36Filename,					"fileName" },
		{ rcvM36GeneratedBy,				"generatedBy" },
		{ rcvM36Height,						"height" },
		{ rcvM36LastModified,				"lastModified" },
		{ rcvM36LayerHeight,				"layerHeight" },
		{ rcvM36PrintTime,					"printTime" },
		{ rcvM36SimulatedTime,				"simulatedTime" },
		{ rcvM36Size,						"size" },
		{ rcvM36ThumbnailsFormat,			"thumbnails^:format" },
		{ rcvM36ThumbnailsHeight,			"thumbnails^:height" },
		{ rcvM36ThumbnailsOffset,			"thumbnails^:offset" },
		{ rcvM36ThumbnailsSize,				"thumbnails^:size" },
		{ rcvM36ThumbnailsWidth,			"thumbnails^:width" },

		{ rcvM361ThumbnailData,				"thumbnail:data" },
		{ rcvM361ThumbnailErr,				"thumbnail:err" },
		{ rcvM361ThumbnailFilename,			"thumbnail:fileName" },
		{ rcvM361ThumbnailNext,				"thumbnail:next" },
		{ rcvM361ThumbnailOffset,			"thumbnail:offset" },

		// Push messages
		{ rcvPushMessage,					"message" },
		{ rcvPushResponse,					"resp" },
		{ rcvPushSeq,						"seq" },
		{ rcvPushBeepDuration,				"beep_length" },
		{ rcvPushBeepFrequency,				"beep_freq" },

		// Control Command message
		{ rcvControlCommand,				"controlCommand" },
	};

	enum SeqState {
		SeqStateInit,
		SeqStateOk,
		SeqStateUpdate,
		SeqStateError,
		SeqStateDisabled
	};

	static struct Seq {
		const ReceivedDataEvent event;
		const ReceivedDataEvent seqid;

		uint16_t lastSeq;
		enum SeqState state;

		const char * const key;
		const char * const flags;
	}

	seqs[] = {
	#if FETCH_NETWORK
		{ .event = rcvOMKeyNetwork, .seqid = rcvSeqsNetwork, .lastSeq = 0, .state = SeqStateInit, .key = "network", .flags = "v" },
	#endif
	#if FETCH_BOARDS
		{ .event = rcvOMKeyBoards, .seqid = rcvSeqsBoards, .lastSeq = 0, .state = SeqStateInit, .key = "boards", .flags = "v" },
	#endif
	#if FETCH_MOVE
		{ .event = rcvOMKeyMove, .seqid = rcvSeqsMove, .lastSeq = 0, .state = SeqStateInit, .key = "move", .flags = "v" },
	#endif
	#if FETCH_HEAT
		{ .event = rcvOMKeyHeat, .seqid = rcvSeqsHeat, .lastSeq = 0, .state = SeqStateInit, .key = "heat", .flags = "v" },
	#endif
	#if FETCH_TOOLS
		{ .event = rcvOMKeyTools, .seqid = rcvSeqsTools, .lastSeq = 0, .state = SeqStateInit, .key = "tools", .flags = "v" },
	#endif
	#if FETCH_SPINDLES
		{ .event = rcvOMKeySpindles, .seqid = rcvSeqsSpindles, .lastSeq = 0, .state = SeqStateInit, .key = "spindles", .flags = "v" },
	#endif
	#if FETCH_DIRECTORIES
		{ .event = rcvOMKeyDirectories, .seqid = rcvSeqsDirectories, .lastSeq = 0, .state = SeqStateInit, .key = "directories", .flags = "v" },
	#endif
	#if FETCH_FANS
		{ .event = rcvOMKeyFans, .seqid = rcvSeqsFans, .lastSeq = 0, .state = SeqStateInit, .key = "fans", .flags = "v" },
	#endif
	#if FETCH_INPUTS
		{ .event = rcvOMKeyInputs, .seqid = rcvSeqsInputs, .lastSeq = 0, .state = SeqStateInit, .key = "inputs", .flags = "v" },
	#endif
	#if FETCH_JOB
		{ .event = rcvOMKeyJob, .seqid = rcvSeqsJob, .lastSeq = 0, .state = SeqStateInit, .key = "job", .flags = "v" },
	#endif
	#if FETCH_SCANNER
		{ .event = rcvOMKeyScanner, .seqid = rcvSeqsScanner, .lastSeq = 0, .state = SeqStateInit, .key = "scanner", .flags = "v" },
	#endif
	#if FETCH_SENSORS
		{ .event = rcvOMKeySensors, .seqid = rcvSeqsSensors, .lastSeq = 0, .state = SeqStateInit, .key = "sensors", .flags = "v" },
	#endif
	#if FETCH_STATE
		{ .event = rcvOMKeyState, .seqid = rcvSeqsState, .lastSeq = 0, .state = SeqStateInit, .key = "state", .flags = "vn" },
	#endif
	#if FETCH_VOLUMES
		{ .event = rcvOMKeyVolumes, .seqid = rcvSeqsVolumes, .lastSeq = 0, .state = SeqStateInit, .key = "volumes", .flags = "v" },
	#endif
	};

	Seq *currentReqSeq = nullptr;
	Seq *currentRespSeq = nullptr;

	struct Seq* GetNextSeq(struct Seq *current) {
		if (current == nullptr) {
			LOGD("current seq is null");
			current = seqs;
		}

		for (size_t i = current - seqs; i < ARRAY_SIZE(seqs); ++i) {
			current = &seqs[i];
			if (current->state == SeqStateError) {
				// skip and re-init if last request had an error
				current->state = SeqStateInit;
				continue;
			}
			if (current->state == SeqStateInit || current->state == SeqStateUpdate) {
				return current;
			}
		}
		return nullptr;
	}

	static struct Seq *FindSeqByKey(const char *key) {
		LOGD("key %s\n", key);

		for (size_t i = 0; i < ARRAY_SIZE(seqs); ++i) {
			if (strcasecmp(seqs[i].key, key) == 0) {
				return &seqs[i];
			}

		}

		return nullptr;
	}

	static void UpdateSeq(const ReceivedDataEvent seqid, int32_t val) {
		for (size_t i = 0; i < ARRAY_SIZE(seqs); ++i) {
			if (seqs[i].seqid == seqid) {
				if (seqs[i].lastSeq != val) {
					LOGD("%s %d -> %d\n", seqs[i].key, seqs[i].lastSeq, val);
					seqs[i].lastSeq = val;
					seqs[i].state = SeqStateUpdate;
				}
			}
		}
	}

	static void ResetSeqs() {
		for (size_t i = 0; i < ARRAY_SIZE(seqs); ++i) {
			seqs[i].lastSeq = 0;
			seqs[i].state = SeqStateInit;
		}
	}

	template<typename T>
	int compare(const void* lp, const void* rp){
		return strcasecmp(((T*) lp)->key, ((T*) rp)->key);
	}

	// Try to get an integer value from a string. If it is actually a floating point value, round it.
	// NB: We are using the old fashioned way (using strtol/strtof) instead of the safe functions provided
	// by the RRFLibraries because of the uClibc++ library not providing the things we need for that.
	static bool GetInteger(const char s[], int32_t &rslt) {
		if (s[0] == 0) return false;			// empty string

		char* endptr;
		rslt = (int) strtol(s, &endptr, 10);
		if (*endptr == 0) return true;			// we parsed an integer

		if (strlen(s) > 10) return false;		// avoid strtod buggy behaviour on long input strings

		float d = strtof(s, &endptr);			// try parsing a floating point number
		if (*endptr == 0) {
			rslt = (int)((d < 0.0) ? d - 0.5 : d + 0.5);
			return true;
		}
		return false;
	}

	// Try to get an unsigned integer value from a string
	// NB: We are using the old fashioned way (using strtol/strtof) instead of the safe functions provided
	// by the RRFLibraries because of the uClibc++ library not providing the things we need for that.
	bool GetUnsignedInteger(const char s[], unsigned int &rslt) {
		if (s[0] == 0) return false;			// empty string
		char* endptr;
		rslt = (int) strtoul(s, &endptr, 10);
		return *endptr == 0;
	}

	// Try to get a floating point value from a string. if it is actually a floating point value, round it.
	// NB: We are using the old fashioned way (using strtol/strtof) instead of the safe functions provided
		// by the RRFLibraries because of the uClibc++ library not providing the things we need for that.
	bool GetFloat(const char s[], float &rslt) {
		if (s[0] == 0) return false;			// empty string

		// GNU strtod is buggy, it's very slow for some long inputs, and some versions have a buffer overflow bug.
		// We presume strtof may be buggy too. Tame it by rejecting any strings that much longer than we expect to receive.
		if (strlen(s) > 10) return false;

		char* endptr;
		rslt = strtof(s, &endptr);
		return *endptr == 0;					// we parsed a float
	}

	// Try to get a bool value from a string.
	static bool GetBool(const char s[], bool &rslt) {
		if (s[0] == 0) return false;			// empty string

		rslt = (strcasecmp(s, "true") == 0);
		return true;
	}

	static void StartReceivedMessage();
	static void EndReceivedMessage();
	static void ProcessReceivedValue(StringRef id, const char data[], const size_t indices[]);
	static void ProcessArrayEnd(const char id[], const size_t indices[]);
	static void ParserErrorEncountered(int currentState, const char*, int errors);

	static struct SerialIo::SerialIoCbs serial_cbs = {
		.StartReceivedMessage = StartReceivedMessage,
		.EndReceivedMessage = EndReceivedMessage,
		.ProcessReceivedValue = ProcessReceivedValue,
		.ProcessArrayEnd = ProcessArrayEnd,
		.ParserErrorEncountered = ParserErrorEncountered
	};

	static void StartReceivedMessage() {
		newMessageSeq = messageSeq;/*
		MessageLog::BeginNewMessage();
		FileManager::BeginNewMessage();
		currentAlert.flags.Clear();*/

		if (thumbnailContext.state == ThumbnailState::Init) {
			thumbnailContext.Init();
			ThumbnailInit(thumbnail);
			memset(&thumbnailData, 0, sizeof(thumbnailData));
		}
	}

	static void EndReceivedMessage() {

		//lastResponseTime = SystemTick::GetTickCount();

		if (currentRespSeq != nullptr) {
			currentRespSeq->state = outOfBuffers ? SeqStateError : SeqStateOk;
			LOGD("seq %s %d DONE\n", currentRespSeq->key, currentRespSeq->state);
			currentRespSeq = nullptr;
		}
		outOfBuffers = false;	// Reset the out-of-buffers flag

		if (newMessageSeq != messageSeq) {
			messageSeq = newMessageSeq;
			//MessageLog::DisplayNewMessage();
		}
		//FileManager::EndReceivedMessage();

		if (thumbnailContext.parseErr != 0 || thumbnailContext.err != 0) {
			LOGD("thumbnail parseErr %d err %d.\n", thumbnailContext.parseErr, thumbnailContext.err);
			thumbnailContext.state = ThumbnailState::Init;
		}
	#if 0 // && DEBUG
		if (thumbnail.imageFormat != Thumbnail::ImageFormat::Invalid) {
			dbg("filename %s offset %d size %d format %d width %d height %d\n",
				thumbnailContext.filename.c_str(),
				thumbnailContext.offset, thumbnailContext.size,
				thumbnail.imageFormat,
				thumbnail.width, thumbnail.height);
		}
	#endif
		int ret;

		switch (thumbnailContext.state) {
		case ThumbnailState::Init:
		case ThumbnailState::DataRequest:
		case ThumbnailState::DataWait:
			break;
		case ThumbnailState::Header:
			if (!ThumbnailIsValid(thumbnail)) {
				LOGD("thumbnail meta invalid.\n");
				break;
			}
			thumbnailContext.state = ThumbnailState::DataRequest;
			break;
		case ThumbnailState::Data:
			if (!ThumbnailDataIsValid(thumbnailData)) {
				LOGD("thumbnail meta or data invalid.\n");
				thumbnailContext.state = ThumbnailState::Init;
				break;
			}/*
			if ((ret = ThumbnailDecodeChunk(thumbnail, thumbnailData, UI::UpdateFileThumbnailChunk)) < 0) {
				LOGD("failed to decode thumbnail chunk %d.\n", ret);
				thumbnailContext.state = ThumbnailState::Init;
				break;
			}*/
			if (thumbnailContext.next == 0) {
				thumbnailContext.state = ThumbnailState::Init;
			} else {
				thumbnailContext.state = ThumbnailState::DataRequest;
			}
			break;
		}
	}
/*
	void HandleOutOfBufferResponse() {
		const uint32_t now = SystemTick::GetTickCount();

		// We received the previous out-of-buffer within 10s
		if (now - lastOutOfBufferResponse <= 10000) {
			++oobCounter;
		} else {
			// Reset it if it was more than 10s ago - glitches happen
			oobCounter = 0;
		}

		// Slow down the poll interval by 10% if we see too many out-of-buffer in short time
		if (oobCounter >= 3) {
			pollIntervalMultiplier += 0.1;
			UpdatePollRate(screensaverActive);
			oobCounter = 0;
			MessageLog::AppendMessage("Info: slowing down poll rate");
		}
		lastOutOfBufferResponse = now;
		outOfBuffers = true;
	}
*/
	// Public functions called by the SerialIo module
	static void ProcessReceivedValue(StringRef id, const char data[], const size_t indices[]) {
		ReceivedDataEvent currentResponseType = ReceivedDataEvent::rcvUnknown;

		if (StringStartsWith(id.c_str(), "result")) {
			// We might either get something like:
			// * "result[optional modified]:[key]:[field]" for a live response or
			// * "result[optional modified]:[field]" for a detailed response
			// If live response remove "result:"
			// else replace "result" by "key" (do NOT replace anything beyond "result" as there might be an _ecv_array modifier)

			id.Erase(0, 6);
			if (currentRespSeq != nullptr) {
				currentResponseType = currentRespSeq->seqid;
				id.Prepend(currentRespSeq->key);
			} else {
				// if empty key also erase the colon
				id.Erase(0);
			}
		}

		const FieldTableEntry key = { ReceivedDataEvent::rcvUnknown, id.c_str()};
		const FieldTableEntry* searchResult = (FieldTableEntry*) bsearch(
				&key,
				fieldTable,
				ARRAY_SIZE(fieldTable),
				sizeof(FieldTableEntry),
				compare<FieldTableEntry>);

		// no matching key found
		if (!searchResult) {
			LOGD("no matching key found for %s\n", id.c_str());
			return;
		}
		const ReceivedDataEvent rde = searchResult->val;
		LOGD("event: %s(%d) rtype %d data '%s'\n", searchResult->key, searchResult->val, currentResponseType, data);
		switch (rde) {
		// M409 section
		// TODO: Uncomment stuff below related to UI/OM
		case rcvKey:
			{
				// try a quick check otherwise search for key
				if (currentReqSeq && (strcasecmp(data, currentReqSeq->key) == 0)) {
					currentRespSeq = currentReqSeq;
				} else {
					currentRespSeq = FindSeqByKey(data);
				}

				if (currentRespSeq == nullptr) {
					break;
				}

				// reset processing variables
				switch (currentRespSeq->event) {
				case rcvOMKeyHeat:
					lastBed = -1;
					lastChamber = -1;
					break;
				case rcvOMKeyMove:
					visibleAxesCounted = 0;
					break;
				case rcvOMKeySpindles:
					lastSpindle = -1;
					break;
				case rcvOMKeyTools:
					lastTool = -1;
					break;
				default:
					break;
				}
			}
			break;

		// Boards section
		case rcvBoardsFirmwareName:/*
			if (indices[0] == 0) {			// currently we only handle the first board
				for (size_t i = 0; i < ARRAY_SIZE(firmwareTypes); ++i) {
					if (StringStartsWith(data, firmwareTypes[i].name)) {
						const FirmwareFeatureMap newFeatures = firmwareTypes[i].features;
						if (newFeatures != firmwareFeatures) {
							firmwareFeatures = newFeatures;
							FileManager::FirmwareFeaturesChanged();
						}
						break;
					}
				}
			}*/
			break;

		// Fans section
		case rcvFansRequestedValue:
			{/*
				float f;
				bool b = GetFloat(data, f);
				if (b && f >= 0.0 && f <= 1.0) {
					UI::UpdateFanPercent(indices[0], (int)((f * 100.0f) + 0.5f));
				}*/
			}
			break;

		// Heat section
		case rcvHeatBedHeaters:
			{/*
				int32_t heaterNumber;
				if (GetInteger(data, heaterNumber) && heaterNumber > -1) {
					UI::SetBedOrChamberHeater(indices[0], heaterNumber);
					for (size_t i = lastBed + 1; i < indices[0]; ++i) {
						OM::RemoveBed(i, false);
					}
					lastBed = indices[0];
				}*/
			}
			break;

		case rcvHeatChamberHeaters:
			{/*
				int32_t heaterNumber;
				if (GetInteger(data, heaterNumber) && heaterNumber > -1) {
					UI::SetBedOrChamberHeater(indices[0], heaterNumber, false);
					for (size_t i = lastChamber + 1; i < indices[0]; ++i) {
						OM::RemoveChamber(i, false);
					}
					lastChamber = indices[0];
				}*/
			}
			break;

		case rcvHeatHeatersActive:
			{/*
				int32_t ival;
				if (GetInteger(data, ival)) {
					UI::UpdateActiveTemperature(indices[0], ival);
				}*/
			}
			break;

		case rcvHeatHeatersCurrent:
			{/*
				float fval;
				if (GetFloat(data, fval)) {
					UI::UpdateCurrentTemperature(indices[0], fval);
				}*/
			}
			break;

		case rcvHeatHeatersStandby:
			{/*
				int32_t ival;
				if (GetInteger(data, ival)) {
					UI::UpdateStandbyTemperature(indices[0], ival);
				}*/
			}
			break;

		case rcvHeatHeatersState:
			{/*
				const OM::HeaterStatusMapEntry key = (OM::HeaterStatusMapEntry) {data, OM::HeaterStatus::off};
				const OM::HeaterStatusMapEntry * statusFromMap =
						(OM::HeaterStatusMapEntry *) bsearch(
								&key,
								OM::heaterStatusMap,
								ARRAY_SIZE(OM::heaterStatusMap),
								sizeof(OM::HeaterStatusMapEntry),
								compare<OM::HeaterStatusMapEntry>);
				const OM::HeaterStatus status = (statusFromMap != nullptr) ? statusFromMap->val : OM::HeaterStatus::off;
				UI::UpdateHeaterStatus(indices[0], status);*/
			}
			break;

		// Job section
		case rcvJobDuration:
			{/*
				uint32_t duration;
				if (GetUnsignedInteger(data, duration)) {
					UI::UpdateDuration(duration);
				} else {
					UI::UpdateDuration(0);
				}*/
			}
			break;

		case rcvJobFileFilename:
			//UI::PrintingFilenameChanged(data);
			break;

		case rcvJobFileSize:
			{
				uint32_t ival;
				if (GetUnsignedInteger(data, ival)) {
					fileSize = ival;
				} else {
					fileSize = 0;
				}
			}
			break;

		case rcvJobFileSimulatedTime:
			{/*
				uint32_t simulatedTime;
				if (GetUnsignedInteger(data, simulatedTime)) {
					UI::SetSimulatedTime(simulatedTime);
				} else  {
					UI::SetSimulatedTime(0);
				}*/
			}
			break;

		case rcvJobFilePosition:
			{/*
				if (PrintInProgress() && fileSize > 0) {
					uint32_t ival;
					if (GetUnsignedInteger(data, ival)) {
						UI::SetPrintProgressPercent((unsigned int)(((ival*100.0f)/fileSize) + 0.5));
					}

				}*/
			}
			break;

		case rcvJobLastFileName:
			//UI::LastJobFileNameAvailable(true);	// If we get here there is a filename
			break;

		case rcvJobLastFileSimulated:
			{/*
				bool lastFileSimulated;
				if (GetBool(data, lastFileSimulated)) {
					UI::SetLastFileSimulated(lastFileSimulated);
				}*/
			}
			break;

		case rcvJobTimesLeftFile:
		case rcvJobTimesLeftFilament:
		case rcvJobTimesLeftSlicer:
			{/*
				int32_t timeLeft;
				bool b = GetInteger(data, timeLeft);
				if (b && timeLeft >= 0 && timeLeft < 10 * 24 * 60 * 60 && PrintInProgress()) {
					UI::UpdateTimesLeft((rde == rcvJobTimesLeftFilament) ? 1 : (rde == rcvJobTimesLeftSlicer) ? 2 : 0, timeLeft);
				}*/
			}
			break;

		case rcvJobWarmUpDuration:
			{/*
				uint32_t warmUpDuration;
				if (GetUnsignedInteger(data, warmUpDuration)) {
					UI::UpdateWarmupDuration(warmUpDuration);
				} else {
					UI::UpdateWarmupDuration(0);
				}*/
			}
			break;

		// Move section
		case rcvMoveAxesBabystep:
			{/*
				float f;
				if (GetFloat(data, f)) {
					UI::SetBabystepOffset(indices[0], f);
				}*/
			}
			break;

		case rcvMoveAxesHomed:
			{/*
				bool isHomed;
				if (indices[0] < MaxTotalAxes && GetBool(data, isHomed)) {
					UI::UpdateHomedStatus(indices[0], isHomed);
				}*/
			}
			break;

		case rcvMoveAxesLetter:
			{
				//UI::SetAxisLetter(indices[0], data[0]);
			}
			break;

		case rcvMoveAxesUserPosition:
			{/*
				float fval;
				if (GetFloat(data, fval)) {
					UI::UpdateAxisPosition(indices[0], fval);
				}*/
			}
			break;

		case rcvMoveAxesVisible:
			{/*
				bool visible;
				if (GetBool(data, visible)) {
					UI::SetAxisVisible(indices[0], visible);
					if (visible) {
						++visibleAxesCounted;
					}
				}*/
			}
			break;

		case rcvMoveAxesWorkplaceOffsets:
			{/*
				float offset;
				if (GetFloat(data, offset)) {
					UI::SetAxisWorkplaceOffset(indices[0], indices[1], offset);
				}*/
			}
			break;

		case rcvMoveExtrudersFactor:
			{/*
				float fval;
				if (GetFloat(data, fval)) {
					UI::UpdateExtrusionFactor(indices[0], (int)((fval * 100.0f) + 0.5));
				}*/
			}
			break;

		case rcvMoveKinematicsName:/*
			if (status != OM::PrinterStatus::configuring && status != OM::PrinterStatus::connecting) {
				isDelta = (strcasecmp(data, "delta") == 0);
				UI::UpdateGeometry(numAxes, isDelta);
			}*/
			break;

		case rcvMoveSpeedFactor:
			{/*
				float fval;
				if (GetFloat(data, fval)) {
					UI::UpdateSpeedPercent((int) ((fval * 100.0f) + 0.5f));
				}*/
			}
			break;

		case rcvMoveWorkplaceNumber:
			{/*
				uint32_t workplaceNumber;
				if (GetUnsignedInteger(data, workplaceNumber)) {
					UI::SetCurrentWorkplaceNumber(workplaceNumber);
				}*/
			}
			break;

		// Network section
		case rcvNetworkName:/*
			if (status != OM::PrinterStatus::configuring && status != OM::PrinterStatus::connecting) {
				UI::UpdateMachineName(data);
			}*/
			break;

		case rcvNetworkInterfacesActualIP:
			{/*
				// Only look at the first valid IP
				if (indices[0] > 0) {
					return;
				}
				UI::UpdateIP(data);*/
			}
			break;

		// Seqs section
		case rcvSeqsBoards:
		case rcvSeqsDirectories:
		case rcvSeqsFans:
		case rcvSeqsHeat:
		case rcvSeqsInputs:
		case rcvSeqsJob:
		case rcvSeqsMove:
		case rcvSeqsNetwork:
		case rcvSeqsReply:
		case rcvSeqsScanner:
		case rcvSeqsSensors:
		case rcvSeqsSpindles:
		case rcvSeqsState:
		case rcvSeqsTools:
		case rcvSeqsVolumes:
			{
				int32_t ival;

				if (GetInteger(data, ival)) {
					UpdateSeq(rde, ival);
				}
			}
			break;

		// Sensors section
		case rcvSensorsProbeValue:
			{/*
				if (indices[0] == 0 && indices[1] == 0)	{		// currently we only handle one probe with one value
					UI::UpdateZProbe(data);
				}*/
			}
			break;

		// Spindles section
		case rcvSpindlesActive:
			{/*
				int32_t active;
				if (GetInteger(data, active)) {
					if (active < 0) {
						firmwareFeatures.SetBit(m568TempAndRPM);
					}
					UI::SetSpindleActive(indices[0], active);
				}

				for (size_t i = lastSpindle + 1; i < indices[0]; ++i) {
					OM::RemoveSpindle(i, false);
				}
				lastSpindle = indices[0];*/
			}
			break;

		case rcvSpindlesCurrent:
			{/*
				int32_t current;
				if (GetInteger(data, current)) {
					if (current < 0) {
						firmwareFeatures.SetBit(m568TempAndRPM);
					}
					UI::SetSpindleCurrent(indices[0], current);
				}*/
			}
			break;

		case rcvSpindlesMax:
		case rcvSpindlesMin:
			// fans also has a field "result^:max"
			if (currentResponseType != rcvOMKeySpindles) {
				break;
			}
			{/*
				uint32_t speedLimit;
				if (GetUnsignedInteger(data, speedLimit)) {
					UI::SetSpindleLimit(indices[0], speedLimit, rde == rcvSpindlesMax);
				}*/
			}
			break;

		case rcvSpindlesState:
			{/*
				const OM::SpindleStateMapEntry key = (OM::SpindleStateMapEntry) {data, OM::SpindleState::stopped};
				const OM::SpindleStateMapEntry * statusFromMap =
						(OM::SpindleStateMapEntry *) bsearch(
								&key,
								OM::spindleStateMap,
								ARRAY_SIZE(OM::spindleStateMap),
								sizeof(OM::SpindleStateMapEntry),
								compare<OM::SpindleStateMapEntry>);
				const OM::SpindleState state = (statusFromMap != nullptr) ? statusFromMap->val : OM::SpindleState::stopped;
				UI::SetSpindleState(indices[0], state);*/
			}
			break;

		case rcvSpindlesTool:
			{/*
				int32_t toolNumber;
				if (GetInteger(data, toolNumber)) {
					firmwareFeatures.ClearBit(m568TempAndRPM);
					UI::SetSpindleTool(indices[0], toolNumber);
				}*/
			}
			break;

		// State section
		case rcvStateCurrentTool:/*
			if (status == OM::PrinterStatus::connecting)
			{
				break;
			}
			{
				int32_t tool;
				if (GetInteger(data, tool)) {
					UI::SetCurrentTool(tool);
				}
			}*/
			break;

		case rcvStateMessageBox:/*
			if (data[0] == 0) {
				UI::ClearAlert();
			}*/
			break;

		case rcvStateMessageBoxAxisControls:/*
			if (GetUnsignedInteger(data, currentAlert.controls)) {
				currentAlert.flags.SetBit(Alert::GotControls);
			}*/
			break;

		case rcvStateMessageBoxMessage:/*
			currentAlert.text.copy(data);
			currentAlert.flags.SetBit(Alert::GotText);*/
			break;

		case rcvStateMessageBoxMode:/*
			if (GetInteger(data, currentAlert.mode)) {
				currentAlert.flags.SetBit(Alert::GotMode);
			} else {
				currentAlert.mode = 0;
			}*/
			break;

		case rcvStateMessageBoxSeq:/*
			if (GetUnsignedInteger(data, currentAlert.seq)) {
				currentAlert.flags.SetBit(Alert::GotSeq);
			}*/
			break;

		case rcvStateMessageBoxTimeout:/*
			if (GetFloat(data, currentAlert.timeout)) {
				currentAlert.flags.SetBit(Alert::GotTimeout);
			}*/
			break;

		case rcvStateMessageBoxTitle:/*
			currentAlert.title.copy(data);
			currentAlert.flags.SetBit(Alert::GotTitle);*/
			break;

		case rcvStateStatus:/*
			{
				const OM::PrinterStatusMapEntry key = (OM::PrinterStatusMapEntry) { .key = data, .val = OM::PrinterStatus::connecting};
				const OM::PrinterStatusMapEntry * statusFromMap =
						(OM::PrinterStatusMapEntry *) bsearch(
								&key,
								OM::printerStatusMap,
								ARRAY_SIZE(OM::printerStatusMap),
								sizeof(OM::PrinterStatusMapEntry),
								compare<OM::PrinterStatusMapEntry>);
				if (!statusFromMap) {
					dbg("unknown status %s", data);
					break;
				}
				SetStatus(statusFromMap->val);
			}*/
			break;

		case rcvStateUptime:/*
			{
				uint32_t uival;
				if (GetUnsignedInteger(data, uival)) {
					// Controller was restarted
					if (uival < remoteUpTime) {
						Reconnect();
					}
					remoteUpTime = uival;
				}
			}*/
			break;

		// Tools section
		case rcvToolsActive:
		case rcvToolsStandby:
			{/*
				if (indices[1] >= MaxHeatersPerTool) {
					break;
				}
				int32_t temp;
				if (GetInteger(data, temp)) {
					UI::UpdateToolTemp(indices[0], indices[1], temp, rde == rcvToolsActive);
				}*/
			}
			break;

		case rcvToolsExtruders:
			{/*
				uint32_t extruder;
				if (GetUnsignedInteger(data, extruder)) {
					UI::SetToolExtruder(indices[0], extruder);
				}*/
			}
			break;

		case rcvToolsFans:
			{/*
				uint32_t fan;
				if (GetUnsignedInteger(data, fan)) {
					UI::SetToolFan(indices[0], fan);
				}*/
			}
			break;

		case rcvToolsHeaters:
			{/*
				if (indices[1] >= MaxHeatersPerTool) {
					break;
				}
				uint32_t heaterIndex;
				if (GetUnsignedInteger(data, heaterIndex)) {
					UI::SetToolHeater(indices[0], indices[1], heaterIndex);
				}*/
			}
			break;

		case rcvToolsNumber:
			{/*
				for (size_t i = lastTool + 1; i < indices[0]; ++i) {
					OM::RemoveTool(i, false);
				}
				lastTool = indices[0];*/
			}
			break;

		case rcvToolsOffsets:
			{/*
				float offset;
				if (GetFloat(data, offset)) {
					UI::SetToolOffset(indices[0], indices[1], offset);
				}*/
			}
			break;

		case rcvToolsSpindle:
			{/*
				int32_t spindleNumber;
				if (GetInteger(data, spindleNumber)) {
					firmwareFeatures.SetBit(m568TempAndRPM);
					UI::SetToolSpindle(indices[0], spindleNumber);
				}*/
			}
			break;

		case rcvToolsState:
			{/*
				const OM::ToolStatusMapEntry key = (OM::ToolStatusMapEntry) {data, OM::ToolStatus::off};
				const OM::ToolStatusMapEntry * statusFromMap =
						(OM::ToolStatusMapEntry *) bsearch(
								&key,
								OM::toolStatusMap,
								ARRAY_SIZE(OM::toolStatusMap),
								sizeof(OM::ToolStatusMapEntry),
								compare<OM::ToolStatusMapEntry>);
				const OM::ToolStatus status = (statusFromMap != nullptr) ? statusFromMap->val : OM::ToolStatus::off;
				UI::UpdateToolStatus(indices[0], status);*/
			}
			break;

		// Push messages
		case rcvPushResponse:
			//MessageLog::SaveMessage(data);
			break;

		case rcvPushMessage:/*
			if (data[0] == 0) {
				UI::ClearAlert();
			} else {
				UI::ProcessSimpleAlert(data);
			}*/
			break;

		case rcvPushSeq:
			GetUnsignedInteger(data, newMessageSeq);
			break;

		case rcvPushBeepDuration:
			GetInteger(data, beepLength);
			break;

		case rcvPushBeepFrequency:
			GetInteger(data, beepFrequency);
			break;

		// M20 section
		case rcvM20Dir:
			//FileManager::ReceiveDirectoryName(data);
			break;

		case rcvM20Err:
			{/*
				int32_t i;
				if (GetInteger(data, i))
				{
					if (i >= 0) {
						FileManager::ReceiveErrorCode(i);
					} else if (i == -1) {
						// This is not actually part of M20 but RRF ran out of buffers
						HandleOutOfBufferResponse();
					}
				}*/
			}
			break;

		case rcvM20Files:/*
			if (indices[0] == 0) {
				FileManager::BeginReceivingFiles();
			}
			FileManager::ReceiveFile(data);*/
			break;

		// M36 section
		case rcvM36Filament:/*
			{
				static float totalFilament = 0.0;
				if (indices[0] == 0) {
					totalFilament = 0.0;
				}
				float f;
				if (GetFloat(data, f)) {
					totalFilament += f;
					UI::UpdateFileFilament((int)totalFilament);
				}
			}*/
			break;
		case rcvM36Filename:
			thumbnailContext.filename.copy(data);
			break;

		case rcvM36GeneratedBy:
			//UI::UpdateFileGeneratedByText(data);
			break;

		case rcvM36Height:/*
			{
				float f;
				if (GetFloat(data, f)) {
					UI::UpdateFileObjectHeight(f);
				}
			}*/
			break;

		case rcvM36LastModified:
			//UI::UpdateFileLastModifiedText(data);
			break;

		case rcvM36LayerHeight:/*
			{
				float f;
				if (GetFloat(data, f)) {
					UI::UpdateFileLayerHeight(f);
				}
			}*/
			break;

		case rcvM36PrintTime:
		case rcvM36SimulatedTime:
			{/*
				int32_t sz;
				if (GetInteger(data, sz) && sz > 0) {
					UI::UpdatePrintTimeText((uint32_t)sz, rde == rcvM36SimulatedTime);
				}*/
			}
			break;

		case rcvM36Size:
			{/*
				int32_t sz;
				if (GetInteger(data, sz)) {
					UI::UpdateFileSize(sz);
				}*/
			}
			break;

		case rcvM36ThumbnailsFormat:
			thumbnail.imageFormat = Thumbnail::ImageFormat::Invalid;
			if (strcmp(data, "qoi") == 0) {
				thumbnail.imageFormat = Thumbnail::ImageFormat::Qoi;

				thumbnailContext.state = ThumbnailState::Header;
			}
			break;
		case rcvM36ThumbnailsHeight:
			uint32_t height;
			if (GetUnsignedInteger(data, height))
			{
				thumbnail.height = height;
			}
			break;
		case rcvM36ThumbnailsOffset:
			uint32_t offset;
			if (GetUnsignedInteger(data, offset)) {
				thumbnailContext.next = offset;
				LOGD("receive initial offset %d.\n", offset);
			}
			break;
		case rcvM36ThumbnailsSize:
			uint32_t size;
			if (GetUnsignedInteger(data, size)) {
				thumbnailContext.size = size;
			}
			break;
		case rcvM36ThumbnailsWidth:
			uint32_t width;
			if (GetUnsignedInteger(data, width)) {
				thumbnail.width = width;
			}
			break;

		case rcvM361ThumbnailData:
			thumbnailData.size = std::min(strlen(data), sizeof(thumbnailData.buffer));
			memcpy(thumbnailData.buffer, data, thumbnailData.size);
			thumbnailContext.state = ThumbnailState::Data;
			break;
		case rcvM361ThumbnailErr:
			if (!GetInteger(data, thumbnailContext.err)) {
				thumbnailContext.parseErr = -1;
			}
			break;
		case rcvM361ThumbnailFilename:
			if (!thumbnailContext.filename.Equals(data)) {
				thumbnailContext.parseErr = -2;
			}
			break;
		case rcvM361ThumbnailNext:
			if (!GetUnsignedInteger(data, thumbnailContext.next)) {
				thumbnailContext.parseErr = -3;
				break;
			}
			LOGD("receive next offset %d.\n", thumbnailContext.next);
			break;
		case rcvM361ThumbnailOffset:
			if (!GetUnsignedInteger(data, thumbnailContext.offset)) {
				thumbnailContext.parseErr = -4;
				break;
			}
			LOGD("receive current offset %d.\n", thumbnailContext.offset);
			break;

		case rcvControlCommand:
			{
				const ControlCommandMapEntry key = (ControlCommandMapEntry) {data, ControlCommand::invalid};
				const ControlCommandMapEntry * controlCommandFromMap =
						(ControlCommandMapEntry *) bsearch(
								&key,
								controlCommandMap,
								ARRAY_SIZE(controlCommandMap),
								sizeof(ControlCommandMapEntry),
								compare<ControlCommandMapEntry>);
				const ControlCommand controlCommand = (controlCommandFromMap != nullptr) ? controlCommandFromMap->val : ControlCommand::invalid;
				switch (controlCommand) {
				case ControlCommand::eraseAndReset:
					EraseAndReset();					// Does not return
					break;
				case ControlCommand::reset:
					Reset();							// Does not return
					break;
				default:
					// Invalid command. Just ignore.
					break;
				}
			}
			break;

		default:
			break;
		}
	}

	// Public function called when the serial I/O module finishes receiving an array of values
	static void ProcessArrayEnd(const char id[], const size_t indices[]) {
		ReceivedDataEvent currentResponseType = currentRespSeq != nullptr ? currentRespSeq->event : ReceivedDataEvent::rcvUnknown;
		// TODO: uncomment stuff below related to UI/OM
		if (indices[0] == 0 && strcmp(id, "files^") == 0) {
	//		FileManager::BeginReceivingFiles();				// received an empty file list - need to tell the file manager about it
		} else if (currentResponseType == rcvOMKeyHeat) {
			if (strcasecmp(id, "heat:bedHeaters^") == 0) {
	//			OM::RemoveBed(lastBed + 1, true);
				if (initialized) {
	//				UI::AllToolsSeen();
				}
			} else if (strcasecmp(id, "heat:chamberHeaters^") == 0) {
	//			OM::RemoveChamber(lastChamber + 1, true);
				if (initialized) {
	//				UI::AllToolsSeen();
				}
			}
		} else if (currentResponseType == rcvOMKeyMove && strcasecmp(id, "move:axes^") == 0) {
	//		OM::RemoveAxis(indices[0], true);
	//		numAxes = constrain<unsigned int>(visibleAxesCounted, MIN_AXES, MaxDisplayableAxes);
	//		UI::UpdateGeometry(numAxes, isDelta);
		} else if (currentResponseType == rcvOMKeySpindles) {
			if (strcasecmp(id, "spindles^") == 0) {
	//			OM::RemoveSpindle(lastSpindle + 1, true);
				if (initialized) {
	//				UI::AllToolsSeen();
				}
			}
		} else if (currentResponseType == rcvOMKeyTools) {
			if (strcasecmp(id, "tools^") == 0) {
	//			OM::RemoveTool(lastTool + 1, true);
				if (initialized) {
	//				UI::AllToolsSeen();
				}
			} else if (strcasecmp(id, "tools^:extruders^") == 0 && indices[1] == 0) {
	//			UI::SetToolExtruder(indices[0], -1);			// No extruder defined for this tool
			} else if (strcasecmp(id, "tools^:heaters^") == 0) {
				// Remove all heaters no longer defined
	//			if (UI::RemoveToolHeaters(indices[0], indices[1]) && initialized) {
	//				UI::AllToolsSeen();
	//			}
			}
		} else if (currentResponseType == rcvOMKeyVolumes && strcasecmp(id, "volumes^") == 0) {
	//		FileManager::SetNumVolumes(indices[0]);
		}
	}

	static void ParserErrorEncountered(int currentState, const char*, int errors) {
		(void)currentState;

		if (errors > parserMinErrors) {
			//MessageLog::AppendMessageF("Warning: received %d malformed responses.", errors);
			LOGD("Warning: received %d malformed responses.", errors);
		}
		if (currentRespSeq == nullptr) {
			return;
		}

		currentRespSeq->state = SeqStateError;
	}


//------------------------------------------------------------------------------------------------------------------

	void sendNext() {
		currentReqSeq = GetNextSeq(currentReqSeq);
		if (currentReqSeq != nullptr) {
			LOGD("requesting %s\n", currentReqSeq->key);
			SerialIo::Sendf("M409 K\"%s\" F\"%s\"\n", currentReqSeq->key, currentReqSeq->flags);
		} else {
			// Once we get here the first time we will have work all seqs once
			if (!initialized) {
				LOGD("seqs init DONE\n");
				//UI::AllToolsSeen();
				initialized = true;
			}

			// check if specific info is needed
			bool sent = false;
			// TODO: uncomment below
			/*
			if (OkToSend()) {
				sent = FileManager::ProcessTimers();
			}
			*/

			// if nothing was fetched do a status update
			if (!sent) {
				SerialIo::Sendf("M409 F\"d99f\"\n");
			}
		}
	}

	void init() {
		SerialIo::Init(NULL, &serial_cbs);

		// Sort the fieldTable prior searching using binary search
		qsort(
				fieldTable,
				ARRAY_SIZE(fieldTable),
				sizeof(FieldTableEntry),
				compare<FieldTableEntry>);
	}

}
