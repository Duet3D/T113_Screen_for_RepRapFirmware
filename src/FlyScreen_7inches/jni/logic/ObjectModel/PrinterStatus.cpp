/*
 * PrinterStatus.cpp
 *
 *  Created on: 9 Jan 2024
 *      Author: Andy Everitt
 */

#define DEBUG (1)
#include "Debug.h"

#include "PrinterStatus.h"
#include "uart/CommDef.h"
#include "ObjectModel/Utils.h"


namespace OM
{
	static PrinterStatus status = PrinterStatus::connecting;

	bool IsPrintingStatus(OM::PrinterStatus status)
	{
		return status == OM::PrinterStatus::printing
				|| status == OM::PrinterStatus::paused
				|| status == OM::PrinterStatus::pausing
				|| status == OM::PrinterStatus::resuming
				|| status == OM::PrinterStatus::simulating;
	}

	bool PrintInProgress()
	{
		return IsPrintingStatus(status);
	}

	// Return true if sending a command or file list request to the printer now is a good idea.
	// We don't want to send these when the printer is busy with a previous command, because they will block normal status requests.
	bool OkToSend()
	{
		return status == OM::PrinterStatus::idle
				|| status == OM::PrinterStatus::printing
				|| status == OM::PrinterStatus::paused
				|| status == OM::PrinterStatus::off;
	}

	// Return the printer status
	PrinterStatus GetStatus()
	{
		return status;
	}

	void SetStatus(const char * status)
	{
		const PrinterStatusMapEntry key = {status, OM::PrinterStatus::connecting};
		const PrinterStatusMapEntry * statusFromMap =
				(OM::PrinterStatusMapEntry *) bsearch(
						&key,
						OM::printerStatusMap,
						ARRAY_SIZE(OM::printerStatusMap),
						sizeof(OM::PrinterStatusMapEntry),
						compareKey<OM::PrinterStatusMapEntry>);
		if (!statusFromMap) {
			dbg("unknown status %s", status);
			return;
		}
		SetStatus(statusFromMap->val);
	}

	// This is called when the status changes
	void SetStatus(const PrinterStatus newStatus)
	{
		if (newStatus != status)
		{
			dbg("printer status %d -> %d\n", status, newStatus);
			status = newStatus;
		}
	}
}
