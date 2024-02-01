#include "entry/EasyUIContext.h"
#include "uart/UartContext.h"
#include "manager/ConfigManager.h"

#include "logic/Communication.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

void onEasyUIInit(EasyUIContext *pContext) {
	//设置时区为东八区
    setenv("TZ", "CST-8", 1);

	Comm::duet.Init();
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

