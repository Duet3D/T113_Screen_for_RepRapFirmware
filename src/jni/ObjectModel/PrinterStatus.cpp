/*
 * PrinterStatus.cpp
 *
 *  Created on: 9 Jan 2024
 *      Author: Andy Everitt
 */

#include "Debug.h"

#include "Hardware/Duet.h"
#include "ObjectModel/Utils.h"
#include "PrinterStatus.h"
#include "uart/CommDef.h"
#include <string>

namespace OM
{
	static std::string s_printerName;
	static PrinterStatus s_status = PrinterStatus::connecting;

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
		return IsPrintingStatus(s_status);
	}

	// Return true if sending a command or file list request to the printer now is a good idea.
	// We don't want to send these when the printer is busy with a previous command, because they will block normal status requests.
	bool OkToSend()
	{
		return s_status == OM::PrinterStatus::idle || s_status == OM::PrinterStatus::printing ||
			   s_status == OM::PrinterStatus::paused || s_status == OM::PrinterStatus::off;
	}

	// Return the printer status
	PrinterStatus GetStatus()
	{
		return s_status;
	}

	const char* GetStatusText()
	{
		if (s_status > OM::PrinterStatus::unknown && s_status < OM::PrinterStatus::NumTypes)
			return printerStatusMap[(int)s_status].key;
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
		if (newStatus != s_status)
		{
			info("printer status %d -> %d\n", (int)s_status, (int)newStatus);
			s_status = newStatus;
			if (s_status == OM::PrinterStatus::halted)
			{
				Comm::DUET.SendGcode("M999");
			}
		}
	}

	const std::string& GetPrinterName()
	{
		return s_printerName;
	}

	void SetPrinterName(const char* name)
	{
		s_printerName = name;
	}
}
