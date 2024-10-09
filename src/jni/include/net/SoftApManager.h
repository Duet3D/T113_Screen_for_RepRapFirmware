#ifndef _NET_SOFT_AP_MANAGER_H_
#define _NET_SOFT_AP_MANAGER_H_

#include <vector>

#include "system/Thread.h"
#include "utils/MessageQueue.h"

typedef enum {
	E_SOFTAP_DISABLED,
	E_SOFTAP_ENABLING,
	E_SOFTAP_ENABLED,
	E_SOFTAP_DISABLING,
	E_SOFTAP_ENABLE_ERROR
} ESoftApState;

/**
 * @brief Hotspot Manager
 */
class SoftApManager {
public:
	SoftApManager();
	~SoftApManager();

	bool init();

	/**
	 * @brief Enable or disable the hotspot
	 * @param isEnable true to enable the hotspot, false to disable the hotspot
	 */
	void setEnable(bool isEnable);

	/**
	 * @brief Check if the hotspot is enabled
	 */
	bool isEnable() const;

	ESoftApState getSoftApState() const;

	/**
	 * @brief Get the IP address
	 */
	const char* getIp() const;

	/**
	 * @brief Set the hotspot name and password
	 * @param ssid Hotspot name
	 * @param pwd Hotspot password
	 */
	void setSsidAndPwd(const char *ssid, const char *pwd);

	/**
	 * @brief Get the hotspot name
	 */
	const char* getSsid() const;

	/**
	 * @brief Get the hotspot password
	 */
	const char* getPwd() const;

	/**
	 * @brief Hotspot state listener interface
	 */
	class ISoftApStateListener {
	public:
		virtual ~ISoftApStateListener() { }
		virtual void handleSoftApState(ESoftApState state) = 0;
	};

	/**
	 * @brief Add a hotspot state listener interface
	 */
	void addSoftApStateListener(ISoftApStateListener *pListener);

	/**
	 * @brief Remove a hotspot state listener interface
	 */
	void removeSoftApStateListener(ISoftApStateListener *pListener);

private:
	void updateSoftApState(ESoftApState state);

	typedef enum {
		E_MSGTYPE_ENABLE = 1,
		E_MSGTYPE_DISABLE,
		E_MSGTYPE_SET_SSID_PWD,
		E_MSGTYPE_QUIT_THREAD
	} EMessageType;

	class SoftApThread : public Thread {
	public:
		SoftApThread(SoftApManager &sam) : mSAM(sam) { }

		bool start();
		void stop();

	protected:
		virtual bool readyToRun();
		virtual bool threadLoop();

	private:
		SoftApManager &mSAM;
	};

private:
	MessageQueue mMsgQueue;

	ESoftApState mState;

	mutable Mutex mLock;

	std::vector<ISoftApStateListener *> mSoftApStateListenerList;
	mutable Mutex mListenerLock;

	SoftApThread mSoftApThread;
};

#endif	// _NET_SOFT_AP_MANAGER_H_
