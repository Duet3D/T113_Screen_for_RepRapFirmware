/*
 * UartContext.h
 *
 *  Created on: Sep 5, 2017
 *      Author: guoxs
 */

#ifndef _UART_UARTCONTEXT_H_
#define _UART_UARTCONTEXT_H_

#include "CommDef.h"
#include "system/Thread.h"
#include <vector>

class UartContext : public Thread
{
  public:
	virtual ~UartContext();

	bool openUart(const char* pFileName, UINT baudRate);
	void closeUart();

	bool isOpen() { return m_isOpen; }

	bool send(const BYTE* pData, UINT len);

	static UartContext* getInstance();

  protected:
	virtual bool readyToRun();
	virtual bool threadLoop();

  private:
	UartContext();

  private:
	bool m_isOpen;
	int m_uartID;

	// Used for splicing cached data, there may be incomplete data parsing of the previous packet, and data remains
	BYTE* m_dataBufPtr;
	int m_dataBufLen;
};

#define UARTCONTEXT UartContext::getInstance()

#endif /* _UART_UARTCONTEXT_H_ */
