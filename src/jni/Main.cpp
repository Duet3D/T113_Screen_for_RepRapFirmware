#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "Storage.h"
#include "activity/mainActivity.h"
#include "entry/EasyUIContext.h"
#include "manager/ConfigManager.h"
#include "manager/LanguageManager.h"
#include "restclient-cpp/restclient.h"
#include "storage/StoragePreferences.h"
#include "uart/UartContext.h"
#include "utils/BrightnessHelper.h"

#include "Comm/Communication.h"
#include "Configuration.h"
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
	RestClient::init();
	if (StoragePreferences::getString(ID_SYS_LANG_CODE_KEY, "") == "")
	{
		StoragePreferences::putString(ID_SYS_LANG_CODE_KEY, DEFAULT_LANGUAGE_CODE);
		LANGUAGEMANAGER->setCurrentCode(DEFAULT_LANGUAGE_CODE);
	}
	if (StoragePreferences::getInt(ID_SYS_BRIGHTNESS_KEY, -1) == -1)
	{
		StoragePreferences::putInt(ID_SYS_BRIGHTNESS_KEY, 0);
		BRIGHTNESSHELPER->setBrightness(0);
	}
	return "mainActivity";
}

#ifdef __cplusplus

}

#endif  /* __cplusplus */

