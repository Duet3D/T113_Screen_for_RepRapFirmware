/*
 * Webcam.h
 *
 *  Created on: 23 Apr 2024
 *      Author: andy
 */

#ifndef JNI_UI_LOGIC_WEBCAM_H_
#define JNI_UI_LOGIC_WEBCAM_H_

#include "UI/UserInterface.h"

namespace UI::Webcam
{
	size_t GetWebcamCount();
	void SetWebcamListItem(ZKListView::ZKListItem* pListItem, const int index);
	bool SetActiveWebcamIndex(size_t index);

	size_t GetActiveWebcamIndex();

	void RestoreWebcamSettings();
	void AddNewWebcam();
	bool DeleteWebcam(const size_t index);
	void OpenWebcamUrlInput(const size_t index);
	void SetWebcamUrl(const size_t index, const char* url);
	const std::string& GetWebcamUrl(const size_t index);

	bool GetActiveWebcamFrame();
	void UpdateWebcamFrame();

	int GetWebcamUpdateInterval();
	void SetWebcamUpdateInterval(int interval);

	void RegisterUpdateLoop();
} // namespace UI::Webcam

#endif /* JNI_UI_LOGIC_WEBCAM_H_ */
