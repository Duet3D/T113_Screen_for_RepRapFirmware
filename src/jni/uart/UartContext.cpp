/*
 * UartContext.cpp
 *
 *  Created on: Sep 5, 2017
 *      Author: guoxs
 */
#include "Debug.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>
#include <termio.h>
#include <sys/ioctl.h>
#include <math.h>

#include "UI/UserInterface.h"
#include "uart/UartContext.h"

#include "Comm/Communication.h"
#include "Hardware/Duet.h"
#include "manager/LanguageManager.h"
#include "utils/Log.h"

#define UART_DATA_BUF_LEN 32768 // 32KB

extern int parseProtocol(const BYTE *pData, UINT len);

static const char* getBaudRate(UINT baudRate) {
	struct {
		UINT baud;
		const char *pBaudStr;
	} baudInfoTab[] = {
		{ B1200, "B1200" },
		{ B2400, "B2400" },
		{ B4800, "B4800" },
		{ B9600, "B9600" },
		{ B19200, "B19200" },
		{ B38400, "B38400" },
		{ B57600, "B57600" },
		{ B115200, "B115200" },
		{ B230400, "B230400" },
		{ B460800, "B460800" },
		{ B921600, "B921600" }
	};

	int len = sizeof(baudInfoTab) / sizeof(baudInfoTab[0]);
	for (int i = 0; i < len; ++i) {
		if (baudInfoTab[i].baud == baudRate) {
			return baudInfoTab[i].pBaudStr;
		}
	}

	return NULL;
}

UartContext::UartContext() : m_isOpen(false), m_uartID(0), m_dataBufPtr(NULL), m_dataBufLen(0) {}

UartContext::~UartContext() {
	delete[] m_dataBufPtr;
	closeUart();
}

bool UartContext::openUart(const char *pFileName, UINT baudRate) {
	info("openUart pFileName = %s, baudRate = %s\n", pFileName, getBaudRate(baudRate));
	m_uartID = open(pFileName, O_RDWR | O_NOCTTY);

	if (m_uartID <= 0)
	{
		m_isOpen = false;
	}
	else
	{
		struct termios oldtio = { 0 };
		struct termios newtio = { 0 };
		tcgetattr(m_uartID, &oldtio);

		newtio.c_cflag = baudRate|CS8|CLOCAL|CREAD;
		newtio.c_iflag = 0;	// IGNPAR | ICRNL
		newtio.c_oflag = 0;
		newtio.c_lflag = 0;	// ICANON
		newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
		newtio.c_cc[VMIN] = 1; /* blocking read until 1 character arrives */
		tcflush(m_uartID, TCIOFLUSH);
		tcsetattr(m_uartID, TCSANOW, &newtio);

		// Set to non-blocking
		fcntl(m_uartID, F_SETFL, O_NONBLOCK);

		m_isOpen = run("uart");
		if (!m_isOpen)
		{
			error("Failed to open UART\n");
			close(m_uartID);
			m_uartID = 0;
		}

		info("openUart m_isOpen = %d\n", m_isOpen);
	}

	return m_isOpen;
}

void UartContext::closeUart() {
	dbg("closeUart m_isOpen: %d...\n", m_isOpen);
	if (m_isOpen)
	{
		info("Closing UART");
		requestExit();

		close(m_uartID);
		Thread::sleep(100);
		m_uartID = 0;
		m_isOpen = false;
	}
	m_dataBufLen = 0;
}

bool UartContext::send(const BYTE *pData, UINT len) {
	if (!m_isOpen)
	{
		return false;
	}
	int delay = (int)ceilf((float)(1e4 * len) / Comm::DUET.GetBaudRate().rate);

	int ret = write(m_uartID, pData, len);
	Thread::sleep(delay); // Ensure we don't accidentally send data while previous data is still being sent
	verbose("delay %d", delay);
	if (ret != (int) len) {	// fail
		error("send Fail\n");
		return false;
	}
	return true;
}

UartContext* UartContext::getInstance() {
	static UartContext sUC;
	return &sUC;
}

bool UartContext::readyToRun() {
	if (m_dataBufPtr == NULL)
	{
		m_dataBufPtr = new BYTE[UART_DATA_BUF_LEN];
	}

	if (m_dataBufPtr == NULL)
	{
		closeUart();
	}

	return (m_dataBufPtr != NULL);
}

bool UartContext::threadLoop() {
	if (m_isOpen)
	{
		// There may be residual data after the last analysis, which needs to be spliced
		int readNum = read(m_uartID, m_dataBufPtr + m_dataBufLen, UART_DATA_BUF_LEN - m_dataBufLen);

		if (readNum > 0) {
			m_dataBufLen += readNum;

			// Parse protocol
			int len = parseProtocol(m_dataBufPtr, m_dataBufLen);
			if (len == 0)
			{
				m_dataBufLen = 0;
			}
			if (len >= UART_DATA_BUF_LEN)
			{
				error("UART buffer overflow");
				warn("Buffer: %s", m_dataBufPtr);
				UI::POPUP_WINDOW.Open();
				UI::POPUP_WINDOW.SetText(LANGUAGEMANAGER->getValue("uart_buffer_overflow").c_str());
				m_dataBufLen = 0;
				Comm::Reconnect();
			}
		} else {
			Thread::sleep(50);
		}

		return true;
	}

	return false;
}
