#ifndef _NET_NET_MANAGER_H_
#define _NET_NET_MANAGER_H_

#include <vector>

#include "EthernetManager.h"
#include "WifiManager.h"
#include "LTE4GManager.h"
#include "SoftApManager.h"

enum ENetChannel {
	E_NET_CHANNEL_NONE = 0,
	E_NET_CHANNEL_WIFI,
	E_NET_CHANNEL_ETHERNET,
	E_NET_CHANNEL_SOFTAP,
};

class NetContextPrivate;

class NetManager {
	friend class NetContextPrivate;
public:
	NetManager();

	static NetManager* getInstance();

	void start();

	WifiManager* getWifiManager() const;
	EthernetManager* getEthernetManager() const;
	LTE4GManager* getLTE4GManager() const;
	SoftApManager* getSoftApManager() const;

	ENetChannel getConnChannel() const;

	class INetChannelChangedListener {
	public:
		virtual ~INetChannelChangedListener() { }
		virtual void onNetChannelChanged(ENetChannel channel) = 0;
	};

	void addNetChannelChangedListener(INetChannelChangedListener *pListener);
	void removeNetChannelChangedListener(INetChannelChangedListener *pListener);

private:
	~NetManager();

private:
	NetContextPrivate *mContextPtr;
};

#define NETMANAGER			NetManager::getInstance()

#endif	// _NET_NET_MANAGER_H_
