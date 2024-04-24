/*
 * Settings.cpp
 *
 *  Created on: 24 April 2024
 *      Author: Andy Everitt
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "UI/UserInterface.h"

#include "Hardware/Duet.h"
#include "Settings.h"
#include "Storage.h"
#include "UI/Heightmap.h"
#include "UI/Themes.h"
#include "UI/Webcam.h"
#include <storage/StoragePreferences.h>

namespace UI::Settings
{
	void Init()
	{
		dbg("Init Settings UI");

		// Screensaver
		bool screensaverEnable = StoragePreferences::getBool(ID_SCREENSAVER_ENABLE, true);
		UI::GetUIControl<ZKCheckBox>(ID_MAIN_ScreensaverEnable)->setSelected(screensaverEnable);
		EASYUICONTEXT->setScreensaverEnable(screensaverEnable);
		UI::GetUIControl<ZKEditText>(ID_MAIN_ScreensaverTimeoutInput)
			->setText(StoragePreferences::getInt(ID_SCREENSAVER_TIMEOUT, 120));

		// Heightmap
		UI::SetHeightmapRenderMode(UI::HeightmapRenderMode(StoragePreferences::getInt(ID_HEIGHTMAP_RENDER_MODE, 0)));

		// Webcams
		UI::Webcam::RestoreWebcamSettings();

		// Guided setup
		UI::GetUIControl<ZKCheckBox>(ID_MAIN_ShowSetupOnStartup)
			->setSelected(StoragePreferences::getBool(ID_SHOW_SETUP_ON_STARTUP, DEFAULT_SHOW_SETUP_ON_STARTUP));

		// Buzzer
		UI::GetUIControl<ZKCheckBox>(ID_MAIN_BuzzerEnabled)
			->setChecked(StoragePreferences::getBool(ID_BUZZER_ENABLED, true));

		// Theme
		UI::Theme::SetTheme(StoragePreferences::getString(ID_THEME, "dark"));

		// Duet communication settings
		UI::GetUIControl<ZKTextView>(ID_MAIN_CommunicationType)
			->setText(Comm::duetCommunicationTypeNames[(int)Comm::DUET.GetCommunicationType()]);
		UI::GetUIControl<ZKEditText>(ID_MAIN_HostnameInput)->setText(Comm::DUET.GetHostname());
		UI::GetUIControl<ZKEditText>(ID_MAIN_PasswordInput)->setText(Comm::DUET.GetPassword());
		UI::GetUIControl<ZKEditText>(ID_MAIN_PollIntervalInput)->setText((int)Comm::DUET.GetPollInterval());
		UI::GetUIControl<ZKEditText>(ID_MAIN_InfoTimeoutInput)->setText((int)UI::POPUP_WINDOW.GetTimeout());
	}
} // namespace UI::Settings
