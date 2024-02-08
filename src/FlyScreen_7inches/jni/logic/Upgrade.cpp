/*
 * Upgrade.cpp
 *
 *  Created on: 8 Feb 2024
 *      Author: andy
 */

#include "Upgrade.h"

#define DEBUG_LEVEL 3
#include "Debug.h"

#include "utils.h"
#include "Hardware/Usb.h"
#include "Library/CRC.h"
#include "entry/EasyUIContext.h"
#include "os/UpgradeMonitor.h"
#include <vector>
#include <sys/types.h>

void InitUpgradeMountListener()
{
	static UpgradeMountListener s_upgradeMountListener;
	MOUNTMONITOR->removeMountListener(&UPGRADEMONITOR->getUpgradeMountListener());
	MOUNTMONITOR->addMountListener(&s_upgradeMountListener);
}

void UpgradeMountListener::notify(int what, int status, const char* msg)
{
	switch (status)
	{
	case MountMonitor::E_MOUNT_STATUS_MOUNTED: {
		if (UPGRADEMONITOR->checkUpgradeFile(msg))
		{
			warn("Found upgrade file");
			std::string contents;
			USB::ReadFileContents(std::string(msg) + "/" + "update.img", contents);
			uint32_t crc = CRC::Calculate(contents.c_str(), contents.size(), CRC::CRC_32());
			warn("CRC: %d", crc);
			// EASYUICONTEXT->goBack();
//			UPGRADEMONITOR->startMonitoring();
//			UPGRADEMONITOR->startUpgrade();
//			EASYUICONTEXT->openActivity("UpgradeActivity");
		}
		break;
	}
	case MountMonitor::E_MOUNT_STATUS_REMOVE:
		break;
	default:
		break;
	}
}
