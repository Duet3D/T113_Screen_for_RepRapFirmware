/*
 * Commands.cpp
 *
 *  Created on: 23 Feb 2024
 *      Author: andy
 */

/*
 * Commands.h
 *
 *  Created on: 23 Feb 2024
 *      Author: andy
 */

#include "Commands.h"
#include "ObjectModel/Utils.h"
#include "uart/CommDef.h"
#include <stdlib.h>

namespace Comm
{
	// The following tables will be sorted once on startup so entries can be better grouped for code maintenance
	// A '^' character indicates the position of an _ecv_array index, and a ':' character indicates the start of a
	// sub-field name
	FieldTableEntry fieldTable[] = {
		// M409 common fields
		{rcvKey, "key"},

		// M409 K"boards" response
		{rcvBoardsFirmwareName, "boards^:firmwareName"},

		// M409 K"fans" response
		{rcvFansRequestedValue, "fans^:requestedValue"},

		// M409 K"heat" response
		{rcvHeatBedHeaters, "heat:bedHeaters^"},
		{rcvHeatChamberHeaters, "heat:chamberHeaters^"},
		{rcvHeatHeatersActive, "heat:heaters^:active"},
		{rcvHeatHeatersCurrent, "heat:heaters^:current"},
		{rcvHeatHeatersStandby, "heat:heaters^:standby"},
		{rcvHeatHeatersState, "heat:heaters^:state"},

		// M409 K"job" response
		{rcvJobFileFilename, "job:file:fileName"},
		{rcvJobFileSize, "job:file:size"},
		{rcvJobFileSimulatedTime, "job:file:simulatedTime"},
		{rcvJobFilePosition, "job:filePosition"},
		{rcvJobLastFileName, "job:lastFileName"},
		{rcvJobDuration, "job:duration"},
		{rcvJobTimesLeftFilament, "job:timesLeft:filament"},
		{rcvJobTimesLeftFile, "job:timesLeft:file"},
		{rcvJobTimesLeftSlicer, "job:timesLeft:slicer"},
		{rcvJobWarmUpDuration, "job:warmUpDuration"},

		// M409 K"move" response
		{rcvMoveAxesBabystep, "move:axes^:babystep"},
		{rcvMoveAxesHomed, "move:axes^:homed"},
		{rcvMoveAxesLetter, "move:axes^:letter"},
		{rcvMoveAxesMachinePosition, "move:axes^:machinePosition"},
		{rcvMoveAxesUserPosition, "move:axes^:userPosition"},
		{rcvMoveAxesVisible, "move:axes^:visible"},
		{rcvMoveAxesWorkplaceOffsets, "move:axes^:workplaceOffsets^"},
		{rcvMoveExtrudersFactor, "move:extruders^:factor"},
		{rcvMoveKinematicsName, "move:kinematics:name"},
		{rcvMoveSpeedFactor, "move:speedFactor"},
		{rcvMoveWorkplaceNumber, "move:workplaceNumber"},

		// M409 K"network" response
		{rcvNetworkName, "network:name"},
		{rcvNetworkInterfacesActualIP, "network:interfaces^:actualIP"},

		// M409 K"sensors" response
		{rcvSensorsProbeValue, "sensors:probes^:value^"},

		// M409 K"seqs" response
		{rcvSeqsBoards, "seqs:boards"},
		{rcvSeqsDirectories, "seqs:directories"},
		{rcvSeqsFans, "seqs:fans"},
		{rcvSeqsHeat, "seqs:heat"},
		{rcvSeqsInputs, "seqs:inputs"},
		{rcvSeqsJob, "seqs:job"},
		{rcvSeqsMove, "seqs:move"},
		{rcvSeqsNetwork, "seqs:network"},
		{rcvSeqsReply, "seqs:reply"},
		{rcvSeqsScanner, "seqs:scanner"},
		{rcvSeqsSensors, "seqs:sensors"},
		{rcvSeqsSpindles, "seqs:spindles"},
		{rcvSeqsState, "seqs:state"},
		{rcvSeqsTools, "seqs:tools"},
		{rcvSeqsVolumes, "seqs:volumes"},

		// M409 K"spindles" response
		{rcvSpindlesActive, "spindles^:active"},
		{rcvSpindlesCurrent, "spindles^:current"},
		{rcvSpindlesMax, "spindles^:max"},
		{rcvSpindlesMin, "spindles^:min"},
		{rcvSpindlesState, "spindles^:state"},
		{rcvSpindlesTool, "spindles^:tool"},

		// M409 K"state" response
		{rcvStateCurrentTool, "state:currentTool"},
		{rcvStateMessageBox, "state:messageBox"},
		{rcvStateMessageBoxAxisControls, "state:messageBox:axisControls"},
		{rcvStateMessageBoxMessage, "state:messageBox:message"},
		{rcvStateMessageBoxMode, "state:messageBox:mode"},
		{rcvStateMessageBoxSeq, "state:messageBox:seq"},
		{rcvStateMessageBoxTimeout, "state:messageBox:timeout"},
		{rcvStateMessageBoxTitle, "state:messageBox:title"},
		{rcvStateStatus, "state:status"},
		{rcvStateUptime, "state:upTime"},

		// M409 K"tools" response
		{rcvToolsActive, "tools^:active^"},
		{rcvToolsExtruders, "tools^:extruders^"},
		{rcvToolsFans, "tools^:fans^"},
		{rcvToolsHeaters, "tools^:heaters^"},
		{rcvToolsNumber, "tools^:number"},
		{rcvToolsOffsets, "tools^:offsets^"},
		{rcvToolsSpindle, "tools^:spindle"},
		{rcvToolsSpindleRpm, "tools^:spindleRpm"},
		{rcvToolsStandby, "tools^:standby^"},
		{rcvToolsState, "tools^:state"},

		// M20 response
		{rcvM20Dir, "dir"},
		{rcvM20Err, "err"},
		{rcvM20Files, "files^"},

		// M36 response
		{rcvM36Filament, "filament^"},
		{rcvM36Filename, "fileName"},
		{rcvM36GeneratedBy, "generatedBy"},
		{rcvM36Height, "height"},
		{rcvM36LastModified, "lastModified"},
		{rcvM36LayerHeight, "layerHeight"},
		{rcvM36PrintTime, "printTime"},
		{rcvM36SimulatedTime, "simulatedTime"},
		{rcvM36Size, "size"},
		{rcvM36ThumbnailsFormat, "thumbnails^:format"},
		{rcvM36ThumbnailsHeight, "thumbnails^:height"},
		{rcvM36ThumbnailsOffset, "thumbnails^:offset"},
		{rcvM36ThumbnailsSize, "thumbnails^:size"},
		{rcvM36ThumbnailsWidth, "thumbnails^:width"},

		{rcvM361ThumbnailData, "thumbnail:data"},
		{rcvM361ThumbnailErr, "thumbnail:err"},
		{rcvM361ThumbnailFilename, "thumbnail:fileName"},
		{rcvM361ThumbnailNext, "thumbnail:next"},
		{rcvM361ThumbnailOffset, "thumbnail:offset"},

		// Push messages
		{rcvPushMessage, "message"},
		{rcvPushResponse, "resp"},
		{rcvPushSeq, "seq"},
		{rcvPushBeepDuration, "beep_length"},
		{rcvPushBeepFrequency, "beep_freq"},

		// Control Command message
		{rcvControlCommand, "controlCommand"},
	};

	void SortFieldTable()
	{
		// Sort the fieldTable prior searching using binary search
		qsort(fieldTable, ARRAY_SIZE(fieldTable), sizeof(FieldTableEntry), compareKey<FieldTableEntry>);
	}

	const FieldTableEntry* SearchFieldTable(const char* id)
	{
        const FieldTableEntry key = {ReceivedDataEvent::rcvUnknown, id};
		const FieldTableEntry* searchResult = (FieldTableEntry*)bsearch(
			&key, fieldTable, ARRAY_SIZE(fieldTable), sizeof(FieldTableEntry), compareKey<FieldTableEntry>);
		return searchResult;
	}
} // namespace Comm
