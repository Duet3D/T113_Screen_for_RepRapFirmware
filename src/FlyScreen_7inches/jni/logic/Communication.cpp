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

#include "Communication.h"
#include <stdarg.h>

#include "Hardware/Duet.h"
#include "Hardware/Reset.h"
#include "Hardware/SerialIo.h"

#include "Configuration.h"
#include "ControlCommands.h"
#include "Library/Thumbnail.h"
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

#include "utils/Log.h"

// These defines control which detailed M409 requests will be sent
// If one of the fields in the disabled ones need to be fetched the
// corresponding define has to be set to (1)
#define FETCH_BOARDS		(1)
#define FETCH_DIRECTORIES	(0)
#define FETCH_FANS			(1)
#define FETCH_HEAT			(1)
#define FETCH_INPUTS		(0)
#define FETCH_JOB			(1)
#define FETCH_MOVE			(1)
#define FETCH_NETWORK		(1)
#define FETCH_SCANNER		(0)
#define FETCH_SENSORS		(1)
#define FETCH_SPINDLES		(1)
#define FETCH_STATE			(1)
#define FETCH_TOOLS			(1)
#define FETCH_VOLUMES		(1)

namespace Comm {

	const int parserMinErrors = 2;

	static long long lastResponseTime = 0;

	static bool outOfBuffers = false;

	static uint32_t messageSeq = 0;
	static uint32_t newMessageSeq = 0;

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
	FieldTableEntry fieldTable[] = {
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
		dbg("key %s\n", key);

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
					dbg("%s %d -> %d\n", seqs[i].key, seqs[i].lastSeq, val);
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
	bool GetInteger(const char s[], int32_t &rslt) {
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
	bool GetBool(const char s[], bool &rslt) {
		if (s[0] == 0) return false;			// empty string

		rslt = (strcasecmp(s, "true") == 0);
		return true;
	}

	void Reconnect()
	{
		warn("Reconnecting");
		lastResponseTime = TimeHelper::getCurrentTime();
		//		lastOutOfBufferResponse = 0;
		OM::SetStatus(OM::PrinterStatus::connecting);
		duet.SendGcode("M29");
		ResetSeqs();
	}

	static void StartReceivedMessage();
	static void EndReceivedMessage();
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

		lastResponseTime = TimeHelper::getCurrentTime();

		if (currentRespSeq != nullptr) {
			currentRespSeq->state = outOfBuffers ? SeqStateError : SeqStateOk;
			dbg("seq %s %d DONE\n", currentRespSeq->key, currentRespSeq->state);
			currentRespSeq = nullptr;
		}
		outOfBuffers = false;	// Reset the out-of-buffers flag

		if (newMessageSeq != messageSeq) {
			messageSeq = newMessageSeq;
			//MessageLog::DisplayNewMessage();
		}
		//FileManager::EndReceivedMessage();

		// Open M291 message box if required
		if (OM::currentAlert.mode != OM::Alert::Mode::None && OM::currentAlert.seq != OM::lastAlertSeq)
		{
			UI::POPUP_WINDOW->Open();
			OM::lastAlertSeq = OM::currentAlert.seq;
		}

		if (thumbnailContext.parseErr != 0 || thumbnailContext.err != 0) {
			error("thumbnail parseErr %d err %d.\n", thumbnailContext.parseErr, thumbnailContext.err);
			thumbnailContext.state = ThumbnailState::Init;
		}
#if 1 // && DEBUG
		if (thumbnail.imageFormat != Thumbnail::ImageFormat::Invalid) {
			dbg("filename %s offset %d size %d format %d width %d height %d\n",
				thumbnailContext.filename.c_str(),
				thumbnailContext.offset,
				thumbnailContext.size,
				thumbnail.imageFormat,
				thumbnail.width,
				thumbnail.height);
		}
#endif
		int ret;

		dbg("thumbnailContext state %d", thumbnailContext.state);
		switch (thumbnailContext.state) {
		case ThumbnailState::Init:
		case ThumbnailState::DataRequest:
		case ThumbnailState::DataWait:
			break;
		case ThumbnailState::Header:
			if (!ThumbnailIsValid(thumbnail)) {
				error("thumbnail meta invalid.\n");
				break;
			}
			thumbnailContext.state = ThumbnailState::DataRequest;
			break;
		case ThumbnailState::Data:
			if (!ThumbnailDataIsValid(thumbnailData)) {
				error("thumbnail meta or data invalid.\n");
				thumbnailContext.state = ThumbnailState::Init;
				break;
			}
			if ((ret = ThumbnailDecodeChunk(thumbnail, thumbnailData, UI::UpdateFileThumbnailChunk)) < 0)
			{
				error("failed to decode thumbnail chunk %d.\n", ret);
				thumbnailContext.state = ThumbnailState::Init;
				break;
			}
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
	void ProcessReceivedValue(StringRef id, const char data[], const size_t indices[])
	{
		verbose(
			"id %s, data %s, indices [%d|%d|%d|%d]", id.c_str(), data, indices[0], indices[1], indices[2], indices[3]);
		if (StringStartsWith(id.c_str(), "result")) {
			// We might either get something like:
			// * "result[optional modified]:[key]:[field]" for a live response or
			// * "result[optional modified]:[field]" for a detailed response
			// If live response remove "result:"
			// else replace "result" by "key" (do NOT replace anything beyond "result" as there might be an _ecv_array modifier)

			id.Erase(0, 6);
			if (currentRespSeq != nullptr)
			{
				id.Prepend(currentRespSeq->key);
			}
			else
			{
				// if empty key also erase the colon
				id.Erase(0);
			}
		}

		// search for key in observerMap
		auto observers = UI::observerMap.GetObservers(id.c_str());
		if (observers.size() != 0)
		{
			for (auto &observer : observers)
			{
				observer.Update(data, indices);
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
			verbose("no matching key found for %s\n", id.c_str());
			return;
		}
		const ReceivedDataEvent rde = searchResult->val;
//		dbg("event: %s(%d) rtype %d data '%s'\n", searchResult->key, searchResult->val, currentResponseType, data);
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

		case rcvM36Filename:
			thumbnailContext.filename.copy(data);
			break;

		case rcvM36ThumbnailsFormat:
			info("thumbnail format %s", data);
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
				info("thumbnail height %d", height);
				thumbnail.height = height;
			}
			break;
		case rcvM36ThumbnailsOffset:
			uint32_t offset;
			if (GetUnsignedInteger(data, offset)) {
				info("thumbnail offset %d", offset);
				thumbnailContext.next = offset;
				dbg("receive initial offset %d.\n", offset);
			}
			break;
		case rcvM36ThumbnailsSize:
			uint32_t size;
			if (GetUnsignedInteger(data, size)) {
				info("thumbnail size %d", size);
				thumbnailContext.size = size;
			}
			break;
		case rcvM36ThumbnailsWidth:
			uint32_t width;
			if (GetUnsignedInteger(data, width)) {
				info("thumbnail width %d", width);
				thumbnail.width = width;
			}
			break;

		case rcvM361ThumbnailData:
			info("thumbnail data %d", strlen(data));
			thumbnailData.size = std::min(strlen(data), sizeof(thumbnailData.buffer));
			memcpy(thumbnailData.buffer, data, thumbnailData.size);
			thumbnailContext.state = ThumbnailState::Data;
			break;
		case rcvM361ThumbnailErr:
			if (!GetInteger(data, thumbnailContext.err)) {
				warn("thumbnail err %s", data);
				thumbnailContext.parseErr = -1;
			}
			break;
		case rcvM361ThumbnailFilename:
			if (!thumbnailContext.filename.Equals(data)) {
				warn("thumbnail filename error \"%s\"", data);
				thumbnailContext.parseErr = -2;
			}
			break;
		case rcvM361ThumbnailNext:
			if (!GetUnsignedInteger(data, thumbnailContext.next)) {
				warn("thumbnail next error \"%s\"", data);
				thumbnailContext.parseErr = -3;
				break;
			}
			dbg("thumbnail receive next offset %d.", thumbnailContext.next);
			break;
		case rcvM361ThumbnailOffset:
			if (!GetUnsignedInteger(data, thumbnailContext.offset)) {
				warn("thumbnail offset error \"%s\"", data);
				thumbnailContext.parseErr = -4;
				break;
			}
			dbg("thumbnail receive current offset %d.", thumbnailContext.offset);
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
		// search for key in observerMap
		auto observers = UI::observerMapArrayEnd.GetObservers(id);
		if (observers.size() != 0)
		{
			for (auto &observer : observers)
			{
				observer.Update(indices);
			}
		}
	}

	static void ParserErrorEncountered(int currentState, const char* id, int errors)
	{
		(void)currentState;

		if (errors > parserMinErrors) {
			UI::CONSOLE->AddResponse(utils::format("Warning: received %d malformed responses.", errors).c_str());
			error("Warning: received %d malformed responses for id \"%s\"", errors, id);
		}
		if (currentRespSeq == nullptr) {
			return;
		}

		currentRespSeq->state = SeqStateError;
	}

//------------------------------------------------------------------------------------------------------------------

	void sendNext() {
		long long now = TimeHelper::getCurrentTime();
		if (now > (lastResponseTime + duet.GetPollInterval() + printerResponseTimeout))
		{
			Reconnect();
		}

		if (thumbnailContext.state == ThumbnailState::DataRequest)
		{
			// Request thumbnail data
			info("Requesting thumbnail data for %s at offset %d\n",
				 thumbnailContext.filename.c_str(),
				 thumbnailContext.next);
			duet.SendGcodef("M36.1 P\"%s\" S%d", thumbnailContext.filename.c_str(), thumbnailContext.next);
			thumbnailContext.state = ThumbnailState::DataWait;
			return;
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
