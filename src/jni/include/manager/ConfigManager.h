/*
 * ConfigManager.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Jun 17, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _MANAGER_CONFIG_MANAGER_H_
#define _MANAGER_CONFIG_MANAGER_H_

#include <string>

class ConfigManager {
public:
	static ConfigManager* getInstance();

	const char* getVersion() const;

	std::string getResFilePath(const std::string &resFileName) const;

	int getLongClickTimeOut() const;
	int getMinBrightness() const;
	int getMaxBrightness() const;
	int getDefBrightness() const;

	const std::string& getStartupLibPath() const;

	int getScreensaverTimeOut() const;

	const std::string& getUartName() const;
	unsigned int getUartBaudRate() const;

	const std::string& getDictPinyinPath() const;

	const std::string& getLanguagePath() const;
	const std::string& getLanguageInternalPath() const;
	const std::string& getLanguageCode() const;

	const std::string& getTouchDev() const;

	int getScreenRotate() const;

	bool isDebugVersion() const;

	bool isNeedToStartupTouchCalibration() const;

	bool isBeepEnable() const;
	void setBeepEnable(bool isEnable);

private:
	ConfigManager();
	void initEasyUICfg();
};

#define CONFIGMANAGER			ConfigManager::getInstance()

#endif /* _MANAGER_CONFIG_MANAGER_H_ */
