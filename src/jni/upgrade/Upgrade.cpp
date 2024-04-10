/*
 * Upgrade.cpp
 *
 *  Created on: 8 Feb 2024
 *      Author: andy
 */

#include "Upgrade.h"

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "ObjectModel/Files.h"

#include "Hardware/Duet.h"
#include "Hardware/Usb.h"
#include "entry/EasyUIContext.h"
#include "os/UpgradeMonitor.h"
#include "storage/StoragePreferences.h"
#include "utils/utils.h"
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

bool UpgradeFromDuet()
{
	std::string filePath = utils::format("/firmware/%s", UPGRADE_FILE_NAME);

	info("Attempting upgrade from Duet file %s", filePath.c_str());
	system("rm /tmp/update.img"); // Remove any previous upgrade file

	std::string contents;
	if (!Comm::duet.DownloadFile(filePath.c_str(), contents))
	{
		error("Failed to download file \"%s\" from Duet", filePath.c_str());
		return false;
	}

	std::ofstream file("/tmp/update.img", std::ios::binary);
	if (!file.is_open())
	{
		error("Failed to create file \"/tmp/update.img\"");
		return false;
	}
	file.write(contents.c_str(), contents.size());
	file.close();

	if (!UPGRADEMONITOR->checkUpgradeFile("/tmp"))
	{
		error("No upgrade file found");
		return false;
	}

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
			if (strcmp(file.d_name, UPGRADE_FILE_NAME) != 0)
			{
				continue;
			}
			if ((int)file.st_mtim.tv_sec == StoragePreferences::getInt("UpgradeFileLastModified", -1))
			{
				warn("Already offered autoupdate");
				return;
			}
		}
		UpgradeFromUSB(UPGRADE_FILE_NAME);
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
