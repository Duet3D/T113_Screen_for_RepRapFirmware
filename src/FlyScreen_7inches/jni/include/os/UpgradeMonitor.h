/*
 * UpgradeMonitor.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Aug 4, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _OS_UPGRADE_MONITOR_H_
#define _OS_UPGRADE_MONITOR_H_

#include "MountMonitor.h"
#include <stdint.h>
#include <string>
#include <vector>

class UpgradeMonitor : Thread
{
  public:
	virtual ~UpgradeMonitor();

	void startMonitoring(); // Can't work out what this does
	void stopMonitoring();	// Can't work out what this does

	bool startUpgrade(); // Can't work out what this does
	void stopUpgrade();	 // Can't work out what this does

	bool isUpgrading() const;
	bool needUpgrade() const; // Not implemented?

	bool checkUpgrade();					  // Can't work out what this does
	bool checkUpgradeFile(const char* pPath); // This will open UpdateActivity if `update.img` is found

	static UpgradeMonitor* getInstance();

  public:
	typedef enum
	{
		E_UPGRADE_STATUS_START,
		E_UPGRADE_STATUS_SUCCESS,
		E_UPGRADE_STATUS_ERROR
	} EUpgradeStatus;

	typedef enum
	{
		E_UPGRADE_TYPE_UBOOT,
		E_UPGRADE_TYPE_BOOT,
		E_UPGRADE_TYPE_SYSTEM,
		E_UPGRADE_TYPE_RES,
		E_UPGRADE_TYPE_DATA,
		E_UPGRADE_TYPE_LOGO,
		E_UPGRADE_TYPE_ENV,
		E_UPGRADE_TYPE_PRIVATE,
		E_UPGRADE_TYPE_FULL
	} ESystemUpgradeType;

	/**
	 * Partition information
	 */
	typedef struct
	{
		uint8_t partn;		   // Partition number
		uint8_t reserve[3];	   // Reserved
		uint8_t offset[4];	   // Offset based on head
		uint8_t imageSize[4];  // Partition image size
		uint8_t imageHead[16]; // Image header data
	} SPartInfo;

	/**
	 * Extended information
	 */
	typedef struct
	{
		uint8_t size[4];	  // Extra information length
		uint8_t perm;		  // Permissions
		uint8_t type[4];	  // Model type
		uint8_t flag;		  // Flag
		uint8_t attr;		  // Related to encryption information
		uint8_t reserve[509]; // Append
		uint8_t crc32[4];	  // Checksum
	} SExtendInfo;

	typedef struct
	{
		ESystemUpgradeType upgradeType;
		std::string upgradeFilePath;
		std::string partName;
		SPartInfo partInfo;
		bool needUpgrade;
	} SSystemUpgradeInfo;

	/**
	 * TS upgrade file information
	 */
	typedef struct
	{
		uint8_t hsize;		// Equals tsinfo size
		uint8_t type;		// 0=Unknown model, 1=TS_GT911
		uint8_t flags;		// BIT(0)=Force upgrade bit (1 means force upgrade enabled), BIT(1)-BIT(7) reserved
		uint8_t reserve;	// Reserved
		uint8_t version[4]; // ts firmware version number
		uint8_t pix[4];		// ts resolution, H16=x L16=y, high 16 represents x, low 16 represents y
		uint8_t date[4];	// ts firmware generation date, 0x20171104 = November 4, 2017
		uint8_t dataLen[4]; // Data length
		uint8_t hmd5[16];	// MD5 check of tsdata, does not include tsinfo, #define MD5_DIGEST_LENGTH 16
		uint8_t* pData;		// Data
		bool needUpgrade;
	} STSUpgradeInfo;

	/**
	 * ts type
	 */
	enum ts_type
	{
		TS_GT911 = 1,
	};

	class IUpgradeStatusListener
	{
	  public:
		virtual ~IUpgradeStatusListener(){};
		virtual void notify(int what, int status, const char* msg) = 0;
	};

	void setUpgradeStatusListener(IUpgradeStatusListener* pListener) { mUpgradeStatusListenerPtr = pListener; }

	std::vector<SSystemUpgradeInfo*>& getUpgradeInfoList() { return mSystemUpgradeInfoList; }

	const STSUpgradeInfo& getTSUpgradeInfo() const { return mTSUpgradeInfo; }

  protected:
	virtual bool threadLoop();

  private:
	UpgradeMonitor();

	bool checkSystemUpgradeFile();
	bool checkTSUpgradeFile();
	bool checkTouchCalib(const char* pPath);
	bool checkRestart(const char* pPath);
	bool checkImgMD5(FILE* pf, const SPartInfo& partInfo);
	bool checkMtdDevMD5(const char* pDev, FILE* pf, const SPartInfo& partInfo);
	void resetSystemUpgradeInfo();
	void resetTSUpgradeInfo();

	void initMtdInfoList();
	const char* getMtdDevByName(const char* pName) const;
	bool writeMtdDev(const char* pDev, FILE* pf, const SSystemUpgradeInfo& upgradeInfo);

  private:
	class UpgradeMountListener : public MountMonitor::IMountListener
	{
	  public:
		UpgradeMountListener(UpgradeMonitor* pUM) : mUMPtr(pUM) {}

		virtual void notify(int what, int status, const char* msg)
		{
			switch (status)
			{
			case MountMonitor::E_MOUNT_STATUS_MOUNTED:
				if (mUMPtr->checkRestart(msg) || mUMPtr->checkTouchCalib(msg) || mUMPtr->checkUpgradeFile(msg))
				{
					// do nothing
				}
				break;

			case MountMonitor::E_MOUNT_STATUS_REMOVE:
				break;
			}
		}

	  private:
		UpgradeMonitor* mUMPtr;
	};

  public:
	UpgradeMountListener& getUpgradeMountListener() { return mUpgradeMountListener; }
	IUpgradeStatusListener* getUpgradeStatusListener() { return mUpgradeStatusListenerPtr; }
	const std::string& getUpgradeDir() const { return mUpgradeDir; }
	void setUpgradeDir(const std::string& dir) { mUpgradeDir = dir; }

  private:
	UpgradeMountListener mUpgradeMountListener;
	IUpgradeStatusListener* mUpgradeStatusListenerPtr;

	bool mHasStartMonitor;

	std::vector<SSystemUpgradeInfo*> mSystemUpgradeInfoList;
	std::string mUpgradeDir;

	bool mIsUpgrading;

	typedef struct
	{
		std::string dev;
		std::string name;
	} SMtdInfo;
	std::vector<SMtdInfo> mMtdInfoList;

	SExtendInfo mExtendInfo;
	STSUpgradeInfo mTSUpgradeInfo;

	const char* mErrorCode;
};

#define UPGRADEMONITOR UpgradeMonitor::getInstance()

#endif /* _OS_UPGRADE_MONITOR_H_ */