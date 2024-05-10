/*
 * Webcam.cpp
 *
 *  Created on: 23 Apr 2024
 *      Author: andy
 */

#include "Debug.h"

#include "Configuration.h"
#include "Storage.h"
#include "Webcam.h"
#include "comm/Network.h"
#include "timer.h"
#include <entry/EasyUIContext.h>
#include <storage/StoragePreferences.h>
#include <string>
#include <vector>

namespace UI::Webcam
{
	static std::vector<std::string> s_webcamUrls;
	static size_t s_activeWebcamIndex = 0;
	static int s_webcamUpdateInterval = DEFAULT_WEBCAM_UPDATE_INTERVAL;
	static const char* s_webcamFile = "/tmp/webcam.png";
	static std::string s_nullString = "";

	class IMETextUpdateListener : public IMEContext::IIMETextUpdateListener
	{
	  public:
		virtual void onIMETextUpdate(const std::string& text)
		{
			SetWebcamUrl(m_index, text.c_str());
			UI::GetUIControl<ZKListView>(ID_MAIN_WebcamUrlList)->refreshListView();
		}

		void SetIndex(size_t i) { m_index = i; }

	  private:
		size_t m_index;
	};

	size_t GetWebcamCount()
	{
		UI::GetUIControl<ZKButton>(ID_MAIN_AddWebcamBtn)->setInvalid(s_webcamUrls.size() >= MAX_WEBCAMS);
		return s_webcamUrls.size();
	}

	void SetWebcamListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		verbose("%d", index);
		if (index >= (int)GetWebcamCount())
		{
			warn("Invalid webcam index %d", index);
			return;
		}
		pListItem->setText(index);
		pListItem->setSelected(index == (int)UI::Webcam::GetActiveWebcamIndex());
	}

	size_t GetActiveWebcamIndex()
	{
		return s_activeWebcamIndex;
	}

	bool SetActiveWebcamIndex(size_t index)
	{
		if (index >= GetWebcamCount())
		{
			warn("Invalid webcam index %d", index);
			return false;
		}
		s_activeWebcamIndex = index;
		StoragePreferences::putInt(ID_ACTIVE_WEBCAM_INDEX, s_activeWebcamIndex);

		system(utils::format("rm -f %s", s_webcamFile).c_str());
		UpdateWebcamFrame(); // Clears the existing frame
		return true;
	}

	void RestoreWebcamSettings()
	{
		std::string url;
		for (size_t i = 0; i < MAX_WEBCAMS; i++)
		{
			url = StoragePreferences::getString(utils::format(ID_WEBCAM_URL, i), "");
			if (url.empty())
			{
				break;
			}
			s_webcamUrls.push_back(url);
		}
		SetActiveWebcamIndex(StoragePreferences::getInt(ID_ACTIVE_WEBCAM_INDEX, 0));
		UI::GetUIControl<ZKEditText>(ID_MAIN_WebcamUpdateIntervalInput)
			->setText(StoragePreferences::getInt(ID_WEBCAM_UPADTE_ITERVAL, DEFAULT_WEBCAM_UPDATE_INTERVAL));
	}

	void AddNewWebcam()
	{
		if (GetWebcamCount() >= MAX_WEBCAMS)
		{
			warn("Maximum number of webcams reached");
			return;
		}
		s_webcamUrls.push_back("");
		UI::GetUIControl<ZKListView>(ID_MAIN_WebcamUrlList)->refreshListView();
	}

	bool DeleteWebcam(const size_t index)
	{
		if (index >= GetWebcamCount())
		{
			warn("Invalid webcam index %d", index);
			return false;
		}
		s_webcamUrls.erase(s_webcamUrls.begin() + index);

		for (size_t i = 0; i < GetWebcamCount(); i++)
		{
			StoragePreferences::putString(utils::format(ID_WEBCAM_URL, i), s_webcamUrls[i]);
		}
		StoragePreferences::remove(utils::format(ID_WEBCAM_URL, GetWebcamCount()));
		if (s_activeWebcamIndex >= GetWebcamCount())
		{
			s_activeWebcamIndex = 0;
		}
		UI::GetUIControl<ZKListView>(ID_MAIN_WebcamUrlList)->refreshListView();
		return true;
	}

	void OpenWebcamUrlInput(const size_t index)
	{
		static IMETextUpdateListener listener;
		static IMEContext::SIMETextInfo info;

		if (index >= GetWebcamCount())
		{
			warn("Invalid webcam index %d", index);
			return;
		}
		listener.SetIndex(index);

		info.imeTextType = IMEContext::E_IME_TEXT_TYPE_ALL;
		info.isPassword = false;
		info.text = s_webcamUrls[index];

		EASYUICONTEXT->showIME(&info, &listener);
	}

	void SetWebcamUrl(const size_t index, const char* url)
	{
		if (index >= GetWebcamCount())
		{
			warn("Invalid webcam index %d", index);
			return;
		}
		s_webcamUrls[index] = url;
		StoragePreferences::putString(utils::format(ID_WEBCAM_URL, index), url);
	}

	const std::string& GetWebcamUrl(const size_t index)
	{
		if (index >= GetWebcamCount())
		{
			warn("Invalid webcam index %d", index);
			return s_nullString;
		}
		return s_webcamUrls[index];
	}

	bool GetActiveWebcamFrame()
	{
		RestClient::Response r;
		Comm::QueryParameters_t queryParameters;

		if (GetWebcamCount() == 0)
		{
			return false;
		}

		size_t currentIndex = s_activeWebcamIndex;
		return Comm::AsyncGet(s_webcamUrls[currentIndex], "", queryParameters, [currentIndex](RestClient::Response& r) {
			if (r.code != 200)
			{
				warn("Failed to get webcam frame: [%d] %s", r.code, r.body.c_str());
				return false;
			}

			// Save the frame to a file
			FILE* f = fopen(s_webcamFile, "wb");
			if (f == nullptr)
			{
				warn("Failed to open file");
				return false;
			}
			fwrite(r.body.c_str(), 1, r.body.size(), f);
			fclose(f);

			if (currentIndex != s_activeWebcamIndex)
			{
				return false;
			}

			UpdateWebcamFrame();
			return true;
		});
	}

	void UpdateWebcamFrame()
	{
		static ZKTextView* feed = UI::GetUIControl<ZKTextView>(ID_MAIN_WebcamFeed);

		if (feed == nullptr)
		{
			warn("Failed to get webcam feed");
			return;
		}

		feed->setBackgroundPic(s_webcamFile);
	}

	int GetWebcamUpdateInterval()
	{
		return s_webcamUpdateInterval;
	}

	void SetWebcamUpdateInterval(int interval)
	{
		if (interval < MINIMUM_WEBCAM_UPDATE_INTERVAL)
		{
			interval = MINIMUM_WEBCAM_UPDATE_INTERVAL;
			UI::GetUIControl<ZKEditText>(ID_MAIN_WebcamUpdateIntervalInput)
				->setText(interval); // this will recall this function
			return;
		}
		s_webcamUpdateInterval = interval;
		StoragePreferences::putInt(ID_WEBCAM_UPADTE_ITERVAL, s_webcamUpdateInterval);
	}

	void RegisterUpdateLoop()
	{
		GetActiveWebcamFrame(); // So there is no delay in getting the first frame
		registerDelayedCallback("WebcamUpdate", s_webcamUpdateInterval, []() {
			if (!UI::GetUIControl<ZKWindow>(ID_MAIN_WebcamWindow)->isVisible())
			{
				// unregister callback if window is not visible
				return false;
			}
			GetActiveWebcamFrame();
			return true;
		});
	}
} // namespace UI::Webcam
