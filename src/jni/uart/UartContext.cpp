/*
 * UartContext.cpp
 *
 *  Created on: Sep 5, 2017
 *      Author: guoxs
 */
#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
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

UartContext::UartContext() :
	mIsOpen(false),
	mUartID(0),
	mDataBufPtr(NULL),
	mDataBufLen(0) {

}

UartContext::~UartContext() {
	delete[] mDataBufPtr;
	closeUart();
}

bool UartContext::openUart(const char *pFileName, UINT baudRate) {
	info("openUart pFileName = %s, baudRate = %s\n", pFileName, getBaudRate(baudRate));
	mUartID = open(pFileName, O_RDWR|O_NOCTTY);

	if (mUartID <= 0) {
		mIsOpen = false;
	} else {
		struct termios oldtio = { 0 };
		struct termios newtio = { 0 };
		tcgetattr(mUartID, &oldtio);

		newtio.c_cflag = baudRate|CS8|CLOCAL|CREAD;
		newtio.c_iflag = 0;	// IGNPAR | ICRNL
		newtio.c_oflag = 0;
		newtio.c_lflag = 0;	// ICANON
		newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
		newtio.c_cc[VMIN] = 1; /* blocking read until 1 character arrives */
		tcflush(mUartID, TCIOFLUSH);
		tcsetattr(mUartID, TCSANOW, &newtio);

		// Set to non-blocking
		fcntl(mUartID, F_SETFL, O_NONBLOCK);

		mIsOpen = run("uart");
		if (!mIsOpen) {
			error("Failed to open UART\n");
			close(mUartID);
			mUartID = 0;
		}

		info("openUart mIsOpen = %d\n", mIsOpen);
	}

	return mIsOpen;
}

void UartContext::closeUart() {
	dbg("closeUart mIsOpen: %d...\n", mIsOpen);
	if (mIsOpen) {
		info("Closing UART");
		requestExit();

		close(mUartID);
		Thread::sleep(100);
		mUartID = 0;
		mIsOpen = false;
	}
	mDataBufLen = 0;
}

bool UartContext::send(const BYTE *pData, UINT len) {
	if (!mIsOpen) {
		return false;
	}
	int delay = (int)ceilf((float)(1e4 * len) / Comm::duet.GetBaudRate().rate);

	int ret = write(mUartID, pData, len);
    Thread::sleep(delay);   // Ensure we don't accidentally send data while previous data is still being sent
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
	if (mDataBufPtr == NULL) {
		mDataBufPtr = new BYTE[UART_DATA_BUF_LEN];
	}

	if (mDataBufPtr == NULL) {
		closeUart();
	}

	return (mDataBufPtr != NULL);
}

bool UartContext::threadLoop() {
	if (mIsOpen) {
		// There may be residual data after the last analysis, which needs to be spliced
		int readNum = read(mUartID, mDataBufPtr + mDataBufLen, UART_DATA_BUF_LEN - mDataBufLen);

		if (readNum > 0) {
			mDataBufLen += readNum;

			// Parse protocol
			int len = parseProtocol(mDataBufPtr, mDataBufLen);
			if (len == 0) { mDataBufLen = 0; }
			if (len >= UART_DATA_BUF_LEN)
			{
				error("UART buffer overflow");
				warn("Buffer: %s", mDataBufPtr);
				UI::POPUP_WINDOW->Open();
				UI::POPUP_WINDOW->SetText(LANGUAGEMANAGER->getValue("uart_buffer_overflow").c_str());
				mDataBufLen = 0;
				Comm::Reconnect();
			}
		} else {
			Thread::sleep(50);
		}

		return true;
	}

	return false;
}
