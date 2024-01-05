#include "entry/EasyUIContext.h"
#include "uart/UartContext.h"
#include "manager/ConfigManager.h"

#include "logic/Communication.hpp"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

void onEasyUIInit(EasyUIContext *pContext) {
	//设置时区为东八区
    setenv("TZ", "CST-8", 1);
	// 初始化时打开串口
	UARTCONTEXT->openUart(CONFIGMANAGER->getUartName().c_str(), CONFIGMANAGER->getUartBaudRate());
	Comm::init();
}

void onEasyUIDeinit(EasyUIContext *pContext) {
	UARTCONTEXT->closeUart();
}

const char* onStartupApp(EasyUIContext *pContext) {
	return "mainActivity";
}


#ifdef __cplusplus

}

#endif  /* __cplusplus */

