#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "activity/mainActivity.h"
#include "entry/EasyUIContext.h"
#include "manager/ConfigManager.h"
#include "manager/LanguageManager.h"
#include "storage/StoragePreferences.h"
#include "uart/UartContext.h"
#include "utils/BrightnessHelper.h"

#include "Comm/Communication.h"
#include "Hardware/Duet.h"

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

const char* onStartupApp(EasyUIContext* pContext)
{
	if (StoragePreferences::getString("sys_lang_code_key", "") == "")
	{
		StoragePreferences::putString("sys_lang_code_key", "en_US");
		LANGUAGEMANAGER->setCurrentCode("en_US");
	}
	if (StoragePreferences::getInt("sys_brightness_key", -1) == -1)
	{
		StoragePreferences::putInt("sys_brightness_key", 0);
		BRIGHTNESSHELPER->setBrightness(0);
	}
	return "mainActivity";
}

#ifdef __cplusplus

}

#endif  /* __cplusplus */

