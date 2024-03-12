/*
 * Upgrade.cpp
 *
 *  Created on: 8 Feb 2024
 *      Author: andy
 */

#include "Upgrade/Upgrade.h"

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "Hardware/Usb.h"
#include "Library/CRC.h"
#include "ObjectModel/Files.h"
#include "entry/EasyUIContext.h"
#include "os/UpgradeMonitor.h"
#include "storage/StoragePreferences.h"
#include "utils.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

void InitUpgradeMountListener()
{
	static UpgradeMountListener s_upgradeMountListener;
	/* Disables opening the upgrade activity if `update.img` is in root USB directory */
	// MOUNTMONITOR->removeMountListener(&UPGRADEMONITOR->getUpgradeMountListener());
	MOUNTMONITOR->addMountListener(&s_upgradeMountListener);
	system("rm /tmp/update.img");
}

bool UpgradeFromUSB(const std::string& filePath)
{
	info("Attempting upgrade from USB file %s", filePath.c_str());
	struct stat sb;
	if (stat((std::string("/mnt/usb1/") + filePath).c_str(), &sb) == -1)
	{
		error("Failed to get file stats for %s", filePath.c_str());
		return false;
	}

	system("rm /tmp/update.img"); // Remove any previous upgrade file
	int ret = system(utils::format("cd /mnt/usb1 && cp \"%s\" /tmp/update.img", filePath.c_str()).c_str());
	if (ret != 0)
	{
		error("Failed to copy file \"%s\" to /tmp, code=%d", filePath.c_str(), ret);
		return false;
	}
	verbose("File \"%s\" copied to /tmp, code=%d", filePath.c_str(), ret);
	if (!UPGRADEMONITOR->checkUpgradeFile("/tmp"))
	{
		error("No upgrade file found");
		return false;
	}

	StoragePreferences::putInt("UpgradeFileLastModified", (int)sb.st_mtim.tv_sec);
	return true;
}

void UpgradeMountListener::notify(int what, int status, const char* msg)
{
	switch (status)
	{
	case MountMonitor::E_MOUNT_STATUS_MOUNTED: {
		OM::FileSystem::RequestUsbFiles("");
		std::vector<USB::FileInfo> files = USB::ListEntriesInDirectory(msg);
		for (auto& file : files)
		{
			if (strcmp(file.d_name, "DuetScreen.img") != 0)
			{
				continue;
			}
			if ((int)file.st_mtim.tv_sec == StoragePreferences::getInt("UpgradeFileLastModified", -1))
			{
				warn("Already offered autoupdate");
				return;
			}
		}
		UpgradeFromUSB("DuetScreen.img");
		break;
	}
	case MountMonitor::E_MOUNT_STATUS_REMOVE:
		EASYUICONTEXT->closeActivity("UpgradeActivity");
		if (OM::FileSystem::IsUsbFolder())
			OM::FileSystem::RequestUsbFiles("");
		break;
	default:
		break;
	}
}
