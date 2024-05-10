/*
 * Settings.h
 *
 *  Created on: 24 April 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_UI_LOGIC_SETTINGS_H_
#define JNI_UI_LOGIC_SETTINGS_H_

#include "UI/UserInterface.h"
#include <string>

namespace UI::Settings
{
	void Init();

	size_t GetWindowSelectCount();
	void SetWindowSelectListItem(ZKListView::ZKListItem* pListItem, const int index);
	void WindowSelectListItemCallback(const int index);

	/* Duet Settings */
	size_t GetBaudRateCount();
	void SetBaudRateListItem(ZKListView::ZKListItem* pListItem, const int index);
	void BaudRateListItemCallback(const int index);

	size_t GetDuetCommMethodCount();
	void SetDuetCommMethodListItem(ZKListView::ZKListItem* pListItem, const int index);
	void DuetCommMethodListItemCallback(const int index);

	void DuetPollIntervalInputCallback(const std::string& text);
	void DuetHostnameInputCallback(const std::string& text);
	void DuetPasswordInputCallback(const std::string& text);

	/* Popup Settings */
	void InfoTimeoutInputCallback(const std::string& text);

	/* Guides */
	void SetGuideListItem(ZKListView::ZKListItem* pListItem, const int index);
	void GuideListItemCallback(const int index);
	void SetShowGuideOnStartup(const bool enabled);

	/* Screensaver */
	void SetScreensaverEnabled(const bool enabled);
	void SetScreensaverTimeout(const std::string& text);

	/* Theme */
	void SetThemeListItem(ZKListView::ZKListItem* pListItem, const int index);
	void ThemeListItemCallback(const int index);

	/* Buzzer */
	void SetBuzzerEnabled(const bool enabled);

	/* Webcam */
	void SetWebcamUrlListItem(ZKListView::ZKListItem* pListItem, const int index);
	void WebcamUrlListItemCallback(const int index, const int id);

	/* Developer */
	void SetUsbHostMode(const bool isHost);
	void SetConsoleSystemMode(const bool isSystem);
	size_t GetDebugLevelCount();
	void SetDebugLevelListItem(ZKListView::ZKListItem* pListItem, const int index);
	void DebugLevelListItemCallback(const int index);
} // namespace UI::Settings

#endif /* JNI_UI_LOGIC_SETTINGS_H_ */