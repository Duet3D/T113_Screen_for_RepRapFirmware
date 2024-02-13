#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "activity/mainActivity.h"
#include "entry/EasyUIContext.h"
#include "manager/ConfigManager.h"
#include "uart/UartContext.h"

#include "logic/Communication.h"
#include "logic/Hardware/Duet.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

void onEasyUIInit(EasyUIContext *pContext) {
	//设置时区为东八区
	info("");
	setenv("TZ", "CST-8", 1);

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

