/*
 * Upgrade.h
 *
 *  Created on: 8 Feb 2024
 *      Author: andy
 */

#ifndef JNI_LOGIC_UPGRADE_H_
#define JNI_LOGIC_UPGRADE_H_

#include "os/MountMonitor.h"
#include "os/UpgradeMonitor.h"
#include <string>

void InitUpgradeMountListener();

bool UpgradeFromUSB(const std::string& filePath);

class UpgradeMountListener : public MountMonitor::IMountListener
{
  public:
	void notify(int what, int status, const char* msg);
};

class UpgradeStatusListener : public UpgradeMonitor::IUpgradeStatusListener
{
  public:
	void notify(int what, int status, const char* msg);
};

#endif /* JNI_LOGIC_UPGRADE_H_ */
