/*
 * ProtocolParser.cpp
 *
 *  Created on: Sep 7, 2017
 *      Author: guoxs
 */
#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_WARN

#include "uart/ProtocolParser.h"
#include "CommDef.h"
#include "Hardware/Duet.h"
#include "utils/Log.h"
#include <string.h>
#include <system/Mutex.h>
#include <vector>

#include "Debug.h"

static Mutex s_lock;
static std::vector<OnProtocolDataUpdateFun> s_protocolDataUpdateListenerList;

void registerProtocolDataUpdateListener(OnProtocolDataUpdateFun pListener) {
	Mutex::Autolock _l(s_lock);
	dbg();
	if (pListener != NULL) {
		s_protocolDataUpdateListenerList.push_back(pListener);
	}
}

void unregisterProtocolDataUpdateListener(OnProtocolDataUpdateFun pListener) {
	Mutex::Autolock _l(s_lock);
	dbg();
	if (pListener != NULL) {
		std::vector<OnProtocolDataUpdateFun>::iterator iter = s_protocolDataUpdateListenerList.begin();
		for (; iter != s_protocolDataUpdateListenerList.end(); iter++)
		{
			if ((*iter) == pListener) {
				s_protocolDataUpdateListenerList.erase(iter);
				return;
			}
		}
	}
}

static void notifyProtocolDataUpdate(const SProtocolData &data) {
	Mutex::Autolock _l(s_lock);
	std::vector<OnProtocolDataUpdateFun>::const_iterator iter = s_protocolDataUpdateListenerList.begin();
	for (; iter != s_protocolDataUpdateListenerList.end(); iter++)
	{
		(*iter)(data);
	}
}

static SProtocolData s_protocolData;

SProtocolData& getProtocolData() {
	return s_protocolData;
}

/**
 * Parse each frame of data
 */
static void procParse(const unsigned char *pData, unsigned int len) {
	s_protocolData.data = pData;
	s_protocolData.len = len;

	// Notify protocol data update
	notifyProtocolDataUpdate(s_protocolData);
}

/**
 * Function: Parse protocol
 * Parameters:
 *  - pData: protocol data
 *  - len: data length received
 * Return value: the length of the actual resolution protocol
 */
int parseProtocol(const unsigned char *pData, unsigned int len) {
	if (Comm::DUET.GetCommunicationType() != Comm::Duet::CommunicationType::uart)
	{
		return 0;
	}
	dbg("uart.ProtocolParser.parseProtocol: Received %d bytes of data: %.*s", len, len, pData);
	if (pData[len-1] == '\n') {  // a serial message ends with \n
		procParse(pData, len);
		return 0;
	}
	return len;
}
