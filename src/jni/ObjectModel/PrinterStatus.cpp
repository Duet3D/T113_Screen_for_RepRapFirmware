/*
 * PrinterStatus.cpp
 *
 *  Created on: 9 Jan 2024
 *      Author: Andy Everitt
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "Hardware/Duet.h"
#include "ObjectModel/Utils.h"
#include "PrinterStatus.h"
#include "uart/CommDef.h"
#include <string>

namespace OM
{
	static std::string sPrinterName;
	static PrinterStatus sStatus = PrinterStatus::connecting;

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
		return IsPrintingStatus(sStatus);
	}

	// Return true if sending a command or file list request to the printer now is a good idea.
	// We don't want to send these when the printer is busy with a previous command, because they will block normal status requests.
	bool OkToSend()
	{
		return sStatus == OM::PrinterStatus::idle || sStatus == OM::PrinterStatus::printing ||
			   sStatus == OM::PrinterStatus::paused || sStatus == OM::PrinterStatus::off;
	}

	// Return the printer status
	PrinterStatus GetStatus()
	{
		return sStatus;
	}

	const char* GetStatusText()
	{
		if (sStatus > OM::PrinterStatus::unknown && sStatus < OM::PrinterStatus::NumTypes)
			return printerStatusMap[(int)sStatus].key;
		return "unknown";
	}

	void SetStatus(const char * status)
	{
		const PrinterStatusMapEntry key = {status, OM::PrinterStatus::unknown};
		const PrinterStatusMapEntry * statusFromMap =
				(OM::PrinterStatusMapEntry *) bsearch(
						&key,
						OM::printerStatusMap,
						ARRAY_SIZE(OM::printerStatusMap),
						sizeof(OM::PrinterStatusMapEntry),
						compareKey<OM::PrinterStatusMapEntry>);
		if (!statusFromMap) {
			error("unknown status %s", status);
			return;
		}
		SetStatus(statusFromMap->val);
	}

	// This is called when the status changes
	void SetStatus(const PrinterStatus newStatus)
	{
		if (newStatus != sStatus)
		{
			info("printer status %d -> %d\n", (int)sStatus, (int)newStatus);
			sStatus = newStatus;
			if (sStatus == OM::PrinterStatus::halted)
			{
				Comm::duet.SendGcode("M999");
			}
		}
	}

	const std::string& GetPrinterName()
	{
		return sPrinterName;
	}

	void SetPrinterName(const char* name)
	{
		sPrinterName = name;
	}
}
