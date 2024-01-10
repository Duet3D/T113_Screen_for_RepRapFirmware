/*
 * ProtocolParser.cpp
 *
 *  Created on: Sep 7, 2017
 *      Author: guoxs
 */
#include <vector>
#include <string.h>
#include <system/Mutex.h>
#include "CommDef.h"
#include "uart/ProtocolParser.h"
#include "utils/Log.h"

#define DEBUG (0)
#include "Debug.h"

static Mutex sLock;
static std::vector<OnProtocolDataUpdateFun> sProtocolDataUpdateListenerList;

void registerProtocolDataUpdateListener(OnProtocolDataUpdateFun pListener) {
	Mutex::Autolock _l(sLock);
	LOGD("registerProtocolDataUpdateListener\n");
	if (pListener != NULL) {
		sProtocolDataUpdateListenerList.push_back(pListener);
	}
}

void unregisterProtocolDataUpdateListener(OnProtocolDataUpdateFun pListener) {
	Mutex::Autolock _l(sLock);
	LOGD("unregisterProtocolDataUpdateListener\n");
	if (pListener != NULL) {
		std::vector<OnProtocolDataUpdateFun>::iterator iter = sProtocolDataUpdateListenerList.begin();
		for (; iter != sProtocolDataUpdateListenerList.end(); iter++) {
			if ((*iter) == pListener) {
				sProtocolDataUpdateListenerList.erase(iter);
				return;
			}
		}
	}
}

static void notifyProtocolDataUpdate(const SProtocolData &data) {
	Mutex::Autolock _l(sLock);
	std::vector<OnProtocolDataUpdateFun>::const_iterator iter = sProtocolDataUpdateListenerList.begin();
	for (; iter != sProtocolDataUpdateListenerList.end(); iter++) {
		(*iter)(data);
	}
}

static SProtocolData sProtocolData;

SProtocolData& getProtocolData() {
	return sProtocolData;
}

/**
 * Parse each frame of data
 */
static void procParse(const unsigned char *pData, unsigned int len) {
	sProtocolData.data = pData;
	sProtocolData.len = len;

	// Notify protocol data update
	notifyProtocolDataUpdate(sProtocolData);
}

/**
 * Function: Parse protocol
 * Parameters:
 *  - pData: protocol data
 *  - len: data length received
 * Return value: the length of the actual resolution protocol
 */
int parseProtocol(const unsigned char *pData, unsigned int len) {
	dbg("uart.ProtocolParser.parseProtocol: Received %d bytes of data: %.*s", len, len, pData);
	if (pData[len-1] == '\n') {  // a serial message ends with \n
		procParse(pData, len);
		return 0;
	}
	return len;
}
