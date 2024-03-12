/*
 * Commands.h
 *
 *  Created on: 23 Feb 2024
 *      Author: andy
 */

#ifndef JNI_COMM_COMMANDS_H_
#define JNI_COMM_COMMANDS_H_

namespace Comm
{
	enum ReceivedDataEvent
	{
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

	struct FieldTableEntry
	{
		const ReceivedDataEvent val;
		const char* key;
	};

	// The following tables will be sorted once on startup so entries can be better grouped for code maintenance
	// A '^' character indicates the position of an _ecv_array index, and a ':' character indicates the start of a
	// sub-field name
	// Needs to be sorted alphabetically by key for binary search to work
	extern FieldTableEntry fieldTable[];

	void SortFieldTable();
	const FieldTableEntry* SearchFieldTable(const char* id);
} // namespace Comm

#endif /* JNI_COMM_COMMANDS_H_ */
