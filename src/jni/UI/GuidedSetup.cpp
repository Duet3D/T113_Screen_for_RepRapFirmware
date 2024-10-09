/*
 * GuidedSetup.cpp
 *
 *  Created on: 28 Feb 2024
 *      Author: andy
 */

#include "Debug.h"

#include "GuidedSetup.h"
#include "Storage.h"
#include "UI/UserInterface.h"
#include "activity/mainActivity.h"
#include "control/ZKButton.h"
#include "control/ZKTextView.h"
#include "storage/StoragePreferences.h"

namespace UI::GuidedSetup
{
	static ZKWindow* s_window = nullptr;
	static ZKTextView* s_pageNum = nullptr;
	static ZKButton* s_nextBtn = nullptr;
	static ZKButton* s_previousBtn = nullptr;
	static ZKButton* s_closeBtn = nullptr;
	static Guide* s_currentGuide = nullptr;
	static std::map<const char*, Guide*> s_guides;

	Page::Page(const char* guideId,
			   const char* imagePath,
			   function<void(void)> nextCb,
			   function<void(void)> previousCb,
			   int windowId)
		: imagePath(imagePath), nextCb(nextCb), previousCb(previousCb), windowId(windowId)
	{
		if (s_guides.find(guideId) == s_guides.end())
		{
			// TODO we could create the guide here, need to decide if we want to do that
			error("Guide with id %s does not exist", guideId);
			return;
		}
		info("Adding page to guide %s", guideId);
		s_guides[guideId]->AddPage(*this);
	}

	Guide::Guide(const char* id, bool closable, int windowId)
		: m_id(id), m_index(0), m_closable(closable), m_currentPage(nullptr), m_windowId(windowId)
	{
		if (s_guides.find(id) != s_guides.end())
		{
			error("Guide with id %s already exists", id);
			return;
		}
		info("Creating guide %s", id);
		s_guides[id] = this;
	}

	Guide::~Guide()
	{
		Close();
	}

	bool Guide::Show(size_t index)
	{

		if (index >= GetPageCount())
		{
			error("invalid page index %d, only %d pages available", index, GetPageCount());
			return false;
		}

		if (s_window == nullptr)
		{
			error("Guided setup window not initialised");
			return false;
		}
		s_window->showWnd();

		SetPageWindowVisible(false);

		m_index = index;
		m_currentPage = &m_pages.at(m_index);
		s_pageNum->setTextTrf("page_num", m_index + 1, GetPageCount());
		s_previousBtn->setInvalid(m_index <= 0);
		s_nextBtn->setTextTr(m_index + 1 >= GetPageCount() ? "finish" : "next");
		s_closeBtn->setVisible(m_closable);

		SetBackground();
		SetGuideWindowVisible(true);
		SetPageWindowVisible(true);
		return true;
	}

	void Guide::Close()
	{
		s_window->hideWnd();
		SetPageWindowVisible(false);
		SetGuideWindowVisible(false);
		m_currentPage = nullptr;
	}

	void Guide::NextPage()
	{
		RunNextCallback();
		if (m_index + 1 >= GetPageCount())
		{
			Close();
			return;
		}
		Show(m_index + 1);
	}

	void Guide::PreviousPage()
	{
		if (m_index <= 0)
		{
			return;
		}
		RunPreviousCallback();
		Show(m_index - 1);
	}

	bool Guide::SetGuideWindowVisible(bool show)
	{
		if (m_windowId == NULL)
		{
			return false;
		}

		ZKWindow* window = (ZKWindow*)UI::GetUIControl(m_windowId);
		if (window == nullptr)
		{
			error("Invalid window id %d for guide %s", m_windowId, m_id);
			return false;
		}
		dbg("Setting guide window %s", show ? "visible" : "invisible");
		window->setVisible(show);
		return true;
	}

	bool Guide::SetPageWindowVisible(bool show)
	{
		if (m_currentPage == nullptr || m_currentPage->windowId == NULL)
		{
			return false;
		}

		ZKWindow* window = (ZKWindow*)UI::GetUIControl(m_currentPage->windowId);
		if (window == nullptr)
		{
			error("Invalid window id %d for guide %s", m_windowId, m_id);
			return false;
		}
		dbg("Setting page window %s", show ? "visible" : "invisible");
		window->setVisible(show);
		return true;
	}

	bool Guide::SetBackground()
	{
		if (m_currentPage != nullptr && m_currentPage->imagePath != nullptr)
		{
			info("Setting background pic \"%s\"", m_currentPage->imagePath);
			s_window->setBackgroundPic(m_currentPage->imagePath);
			dbg("Set background pic");
			return true;
		}
		return false;
	}

	bool Guide::RunNextCallback()
	{
		if (m_currentPage != nullptr)
		{
			m_currentPage->nextCb();
			return true;
		}
		return false;
	}

	bool Guide::RunPreviousCallback()
	{
		if (m_currentPage != nullptr)
		{
			m_currentPage->previousCb();
			return true;
		}
		return false;
	}

	void Init(ZKWindow* window)
	{
		s_window = window;
		s_pageNum = (ZKTextView*)s_window->findControlByID(ID_MAIN_GuidePageNum);
		s_nextBtn = (ZKButton*)s_window->findControlByID(ID_MAIN_NextPageBtn);
		s_previousBtn = (ZKButton*)s_window->findControlByID(ID_MAIN_PreviousPageBtn);
		s_closeBtn = (ZKButton*)s_window->findControlByID(ID_MAIN_CloseGuideBtn);

		if (StoragePreferences::getBool(ID_SHOW_SETUP_ON_STARTUP, DEFAULT_SHOW_SETUP_ON_STARTUP))
		{
			Show("setup");
		}
	}

	void AddPage(const char* guideId, Page& page)
	{
		if (s_guides.find(guideId) == s_guides.end())
		{
			error("Guide with id %s does not exist", guideId);
			return;
		}
		s_guides[guideId]->AddPage(page);
	}

	Guide* GetGuide(const char* id)
	{
		if (s_guides.find(id) == s_guides.end())
		{
			error("Guide with id %s does not exist", id);
			return nullptr;
		}
		dbg("Got guide %s", id);
		return s_guides[id];
	}

	Guide* GetCurrentGuide()
	{
		return s_currentGuide;
	}

	size_t GetGuideCount()
	{
		return s_guides.size();
	}

	Guide* GetGuideByIndex(size_t index)
	{
		if (index >= GetGuideCount())
		{
			error("invalid guide index %d, only %d guides available", index, GetGuideCount());
			return nullptr;
		}
		auto it = s_guides.begin();
		std::advance(it, index);
		return it->second;
	}

	void Show(const char* guideId, size_t index)
	{
		dbg("Show guide %s", guideId);
		Guide* guide = GetGuide(guideId);
		Show(guide, index);
	}

	void Show(Guide* guide, size_t index)
	{
		if (guide == nullptr)
		{
			warn("Guide is null");
			return;
		}

		Close();
		guide->Show(index);
		s_currentGuide = guide;
	}

	void Close()
	{
		if (s_currentGuide == nullptr)
		{
			return;
		}
		s_currentGuide->Close();
	}

} // namespace UI::GuidedSetup
