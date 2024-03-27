#ifndef _NET_LTE4G_MANAGER_H_
#define _NET_LTE4G_MANAGER_H_

#include <vector>
#include "system/Thread.h"

typedef enum {
	E_LTE4G_POWER_ON,
	E_LTE4G_POWER_ONING,
	E_LTE4G_POWER_OFF,
	E_LTE4G_POWER_OFFING,
	E_LTE4G_UNKNOWN
} ELTE4GPowerState;

class LTE4GInfo;

/**
 * @brief 4G网络管理器
 */
class LTE4GManager {
public:
	LTE4GManager();
	~LTE4GManager();

	bool init();
	bool isSupported() const;

	/**
	 * @brief 获取4G电源状态
	 */
	ELTE4GPowerState getPowerState() const;

	/**
	 * @brief 开关4G电源
	 */
	void setPower(bool isOn);

	const char* getIp() const;
	const char* getMacAddr() const;

	const char* getIMEI() const;
	const char* getICCID() const;
	const char* getIMSI() const;
	const char* getSN() const;
	const char* getManufacturer() const;
	const char* getVersion() const;

	/**
	 * @brief 查询信号质量
	 */
	int queryRSSI();

	/**
	 * @brief 4G开关状态监听接口
	 */
	class ILTE4GPowerStateListener {
	public:
		virtual ~ILTE4GPowerStateListener() { }
		virtual void handleLTE4GPowerState(ELTE4GPowerState state) = 0;
	};

	void addLTE4GPowerStateListener(ILTE4GPowerStateListener *pListener);
	void removeLTE4GPowerStateListener(ILTE4GPowerStateListener *pListener);

private:
	void notifyPowerState(ELTE4GPowerState state);
	bool writePowerDev(const char *pCmd);

	class PowerThread : public Thread {
	public:
		PowerThread(LTE4GManager &lte) :
			m4G(lte),
			mReqPower(false),
			mIsFirst(true) {

		}

		void setPower(bool isOn);

	protected:
		virtual bool readyToRun();
		virtual bool threadLoop();

	private:
		LTE4GManager &m4G;
		bool mReqPower;
		bool mIsFirst;
	};

private:
	PowerThread mPowerThread;
	mutable Mutex mLock;

	std::vector<ILTE4GPowerStateListener *> mListenerList;
	mutable Mutex mListenerLock;

	LTE4GInfo *mLTE4GInfoPtr;
};

#endif	// _NET_LTE4G_MANAGER_H_
