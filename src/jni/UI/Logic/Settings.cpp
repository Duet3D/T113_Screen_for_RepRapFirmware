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
#include "Hardware/Reset.h"
#include "Settings.h"
#include "Storage.h"
#include "UI/GuidedSetup.h"
#include "UI/Logic/Heightmap.h"
#include "UI/Logic/Webcam.h"
#include "UI/Themes.h"
#include "comm/Communication.h"
#include "timer.h"
#include "upgrade/Upgrade.h"
#include <manager/ConfigManager.h>
#include <manager/LanguageManager.h>
#include <storage/StoragePreferences.h>
#include <utils/BrightnessHelper.h>

namespace UI::Settings
{
	/* Modify this array to set what settings windows are visible  */
	static WindowSelectItem s_windows[] = {
		{"language", []() { EASYUICONTEXT->openActivity("LanguageSettingActivity"); }},
		{"duet",
		 []() {
			 UI::GetUIControl(ID_MAIN_DuetUartCommSettingWindow)
				 ->setVisible(Comm::DUET.GetCommunicationType() == Comm::Duet::CommunicationType::uart);
			 UI::GetUIControl(ID_MAIN_DuetNetworkCommSettingWindow)
				 ->setVisible(Comm::DUET.GetCommunicationType() == Comm::Duet::CommunicationType::network);
			 UI::WINDOW.OpenOverlay(ID_MAIN_DuetCommSettingWindow);
		 }},
		{"update",
		 []() {
			 UI::POPUP_WINDOW.Open([]() {
				 if (!UpgradeFromDuet())
				 {
					 registerDelayedCallback("upgrade_failed", 100, []() {
						 UI::POPUP_WINDOW.Open();
						 UI::POPUP_WINDOW.SetTitle(LANGUAGEMANAGER->getValue("upgrade_failed"));
						 return false;
					 });
				 }
			 });
			 UI::POPUP_WINDOW.SetTitle(LANGUAGEMANAGER->getValue("upgrade_firmware").c_str());
			 UI::POPUP_WINDOW.CancelTimeout();
		 }},
		{"restart",
		 []() {
			 // Synchronise data and save cached data to prevent data loss
			 Reset();
		 }},
		{"dev", []() { EASYUICONTEXT->openActivity("DeveloperSettingActivity"); }},
		{"power_off", []() { EASYUICONTEXT->openActivity("PowerOffActivity"); }},
		{"zk_setting", []() { EASYUICONTEXT->openActivity("ZKSettingActivity"); }},
		{"touch_calibration", []() { EASYUICONTEXT->openActivity("TouchCalibrationActivity"); }},
		{"guides", []() { UI::WINDOW.OpenOverlay(ID_MAIN_GuideSelectionWindow); }},
		{"brightness",
		 []() {
			 //! TODO: There is a bug in the flythings brightness api. Sometimes when calling it, the screen will go
			 //! completely white and need a power cycle.
			 //? This might be a bug with the screen hardware itself. TODO: test with next rev hardware
			 UI::SLIDER_WINDOW.Open(LANGUAGEMANAGER->getValue("set_brightness").c_str(),
									"",
									"",
									"%",
									0,
									100,
									100 - BRIGHTNESSHELPER->getBrightness(),
									[](int percent) {
										BRIGHTNESSHELPER->setBrightness(100 -
																		percent); // Flythings brightness is inverted
									});
		 }},
		{"theme", []() { UI::WINDOW.OpenOverlay(ID_MAIN_ThemeSelectionWindow); }},
		{"screensaver", []() { UI::WINDOW.OpenOverlay(ID_MAIN_ScreensaverSettingWindow); }},
		{"buzzer", []() { UI::WINDOW.OpenOverlay(ID_MAIN_BuzzerSettingWindow); }},
		{"webcam", []() { UI::WINDOW.OpenOverlay(ID_MAIN_WebcamSettingWindow); }},
	};

	void Init()
	{
		dbg("Initialising Settings UI");

		// Screensaver
		bool screensaverEnable = StoragePreferences::getBool(ID_SCREENSAVER_ENABLE, true);
		UI::GetUIControl<ZKCheckBox>(ID_MAIN_ScreensaverEnable)->setSelected(screensaverEnable);
		EASYUICONTEXT->setScreensaverEnable(screensaverEnable);
		UI::GetUIControl<ZKEditText>(ID_MAIN_ScreensaverTimeoutInput)
			->setText(StoragePreferences::getInt(ID_SCREENSAVER_TIMEOUT, 120));

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

	size_t GetWindowSelectCount()
	{
		return ARRAY_SIZE(s_windows);
	}

	void SetWindowSelectListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		if (index < 0 || index >= (int)ARRAY_SIZE(s_windows))
		{
			warn("Invalid window index %d", index);
			return;
		}

		pListItem->setTextTr(s_windows[index].id);

		const Theme::Theme* theme = Theme::GetCurrentTheme();
		if (theme == nullptr)
		{
			warn("Failed to get current theme, not applying icon");
			return;
		}

		auto iconIt = theme->colors->settingsScreenWindowSelect.find(s_windows[index].id);
		pListItem->setButtonStatusPic(
			ZK_CONTROL_STATUS_NORMAL,
			iconIt == theme->colors->settingsScreenWindowSelect.end() ? nullptr : iconIt->second.normal);
		pListItem->setButtonStatusPic(
			ZK_CONTROL_STATUS_PRESSED,
			iconIt == theme->colors->settingsScreenWindowSelect.end() ? nullptr : iconIt->second.pressed);
		pListItem->setButtonStatusPic(
			ZK_CONTROL_STATUS_SELECTED,
			iconIt == theme->colors->settingsScreenWindowSelect.end() ? nullptr : iconIt->second.selected);
		pListItem->setButtonStatusPic(
			ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED,
			iconIt == theme->colors->settingsScreenWindowSelect.end() ? nullptr : iconIt->second.pressedAndSelected);
		pListItem->setButtonStatusPic(
			ZK_CONTROL_STATUS_INVALID,
			iconIt == theme->colors->settingsScreenWindowSelect.end() ? nullptr : iconIt->second.invalid);
	}

	void WindowSelectListItemCallback(const int index)
	{
		dbg("index %d", index);
		if (index < 0 || index >= (int)ARRAY_SIZE(s_windows))
		{
			warn("Invalid window index %d", index);
			return;
		}

		s_windows[index].callback();
	}

	/* Duet Settings */

	size_t GetBaudRateCount()
	{
		return ARRAY_SIZE(Comm::baudRates);
	}

	void SetBaudRateListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		pListItem->setSelected(Comm::baudRates[index].rate == Comm::DUET.GetBaudRate().rate);
		pListItem->setText((int)Comm::baudRates[index].rate);
	}

	void BaudRateListItemCallback(const int index)
	{
		Comm::DUET.SetBaudRate(Comm::baudRates[index]);
	}

	size_t GetDuetCommMethodCount()
	{
		return (size_t)Comm::Duet::CommunicationType::COUNT;
	}

	void SetDuetCommMethodListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		pListItem->setText(Comm::duetCommunicationTypeNames[index]);
		pListItem->setSelected(index == (int)Comm::DUET.GetCommunicationType());
	}

	void DuetCommMethodListItemCallback(const int index)
	{
		Comm::DUET.SetCommunicationType((Comm::Duet::CommunicationType)index);
		UI::GetUIControl(ID_MAIN_DuetUartCommSettingWindow)
			->setVisible(Comm::DUET.GetCommunicationType() == Comm::Duet::CommunicationType::uart);
		UI::GetUIControl(ID_MAIN_DuetNetworkCommSettingWindow)
			->setVisible(Comm::DUET.GetCommunicationType() == Comm::Duet::CommunicationType::network);
		UI::GetUIControl<ZKTextView>(ID_MAIN_CommunicationType)->setText(Comm::duetCommunicationTypeNames[index]);
	}

	void DuetPollIntervalInputCallback(const std::string& text)
	{
		if (text.empty() || atoi(text.c_str()) < (int)MIN_PRINTER_POLL_INTERVAL)
		{
			UI::GetUIControl<ZKEditText>(ID_MAIN_PollIntervalInput)->setText((int)MIN_PRINTER_POLL_INTERVAL);
			return;
		}
		Comm::DUET.SetPollInterval(atoi(text.c_str()));
	}

	void DuetHostnameInputCallback(const std::string& text)
	{
		Comm::DUET.SetHostname(text);
	}

	void DuetPasswordInputCallback(const std::string& text)
	{
		Comm::DUET.SetPassword(text);
	}

	/* Popup Settings */

	void InfoTimeoutInputCallback(const std::string& text)
	{
		int32_t timeout = -1;
		if (text.empty() || !Comm::GetInteger(text.c_str(), timeout) || timeout < 0)
		{
			UI::GetUIControl<ZKEditText>(ID_MAIN_InfoTimeoutInput)->setText((int)UI::POPUP_WINDOW.GetTimeout());
			return;
		}
		UI::POPUP_WINDOW.SetTimeout((uint32_t)timeout);
	}

	/* Guides */

	void SetGuideListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		UI::GuidedSetup::Guide* guide = UI::GuidedSetup::GetGuideByIndex(index);
		if (guide == nullptr)
		{
			pListItem->setText("");
			return;
		}
		pListItem->setTextTr(guide->GetId());
	}

	void GuideListItemCallback(const int index)
	{
		UI::GuidedSetup::Guide* guide = UI::GuidedSetup::GetGuideByIndex(index);
		if (guide == nullptr)
		{
			return;
		}
		UI::GuidedSetup::Show(guide->GetId());
	}

	void SetShowGuideOnStartup(const bool enabled)
	{
		dbg("enabled = %d", enabled);
		StoragePreferences::putBool(ID_SHOW_SETUP_ON_STARTUP, enabled);
	}

	/* Screensaver */

	void SetScreensaverEnabled(const bool enabled)
	{
		info("Screensaver %s", enabled ? "enabled" : "disabled");
		StoragePreferences::putBool(ID_SCREENSAVER_ENABLE, enabled);
		EASYUICONTEXT->setScreensaverEnable(enabled);
	}

	void SetScreensaverTimeout(const std::string& text)
	{
		int timeout = -1;
		if (!Comm::GetInteger(text.c_str(), timeout))
		{
			// Invalid input
			return;
		}
		if (timeout < 10)
		{
			UI::GetUIControl<ZKEditText>(ID_MAIN_ScreensaverTimeoutInput)->setText(10);
			return;
		}
		StoragePreferences::putInt(ID_SCREENSAVER_TIMEOUT, timeout);
		EASYUICONTEXT->setScreensaverTimeOut(timeout);
	}

	/* Theme */

	void SetThemeListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		UI::Theme::Theme* theme = UI::Theme::GetThemeByIndex(index);
		if (theme == nullptr)
		{
			pListItem->setText("");
			return;
		}
		pListItem->setTextTr(theme->id.c_str());
		pListItem->setSelected(theme->id == UI::Theme::GetCurrentTheme()->id);
	}

	void ThemeListItemCallback(const int index)
	{
		UI::Theme::Theme* theme = UI::Theme::GetThemeByIndex(index);
		if (theme == nullptr)
		{
			return;
		}
		UI::Theme::SetTheme(theme);
	}

	/* Buzzer */

	void SetBuzzerEnabled(const bool enabled)
	{
		CONFIGMANAGER->setBeepEnable(enabled);
		StoragePreferences::putBool(ID_BUZZER_ENABLED, enabled);
	}

	/* Webcam */

	void SetWebcamUrlListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		pListItem->setTextf("[%d] %s", index, UI::Webcam::GetWebcamUrl(index).c_str());
	}

	void WebcamUrlListItemCallback(const int index, const int id)
	{
		switch (id)
		{
		case ID_MAIN_DeleteWebcamSubItem:
			UI::Webcam::DeleteWebcam(index);
			break;
		default:
			UI::Webcam::OpenWebcamUrlInput(index);
			break;
		}
	}

} // namespace UI::Settings
