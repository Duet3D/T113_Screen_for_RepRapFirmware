/*
 * UartContext.h
 *
 *  Created on: Sep 5, 2017
 *      Author: guoxs
 */

#ifndef _UART_UARTCONTEXT_H_
#define _UART_UARTCONTEXT_H_

#include <vector>
#include "system/Thread.h"
#include "CommDef.h"

class UartContext : public Thread {
public:
	virtual ~UartContext();

	bool openUart(const char *pFileName, UINT baudRate);
	void closeUart();

	bool isOpen() { return mIsOpen; }

	bool send(const BYTE *pData, UINT len);

	static UartContext* getInstance();

protected:
	virtual bool readyToRun();
	virtual bool threadLoop();

private:
	UartContext();

private:
	bool mIsOpen;
	int mUartID;

	// Used for splicing cached data, there may be incomplete data parsing of the previous packet, and data remains
	BYTE *mDataBufPtr;
	int mDataBufLen;
};

#define UARTCONTEXT		UartContext::getInstance()

#endif /* _UART_UARTCONTEXT_H_ */
