/*
 * GuidedSetup.cpp
 *
 *  Created on: 28 Feb 2024
 *      Author: andy
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#include "Debug.h"

#include "GuidedSetup.h"

namespace UI::GuidedSetup
{
	static ZKWindow* s_window = nullptr;
	static Guide* s_currentGuide = nullptr;

	bool Guide::Show(int index)
	{
		if (index >= GetPageCount())
		{
			error("invalid page index %d, only %d pages available", index, GetPageCount());
			return false;
		}

		SetWindowVisible(false);

		m_index = index;
		m_currentPage = &m_pages[m_index];

		SetBackground();
		SetWindowVisible(true);
		return true;
	}

	void Guide::Close()
	{
		SetWindowVisible(false);
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

	bool Guide::SetWindowVisible(bool show)
	{
		if (m_currentPage != nullptr && m_currentPage->window != nullptr)
		{
			m_currentPage->window->setVisible(show);
			return true;
		}
		return false;
	}

	bool Guide::SetBackground()
	{
		if (m_currentPage != nullptr && m_currentPage->imagePath != nullptr)
		{
			info("Setting guided setup background pic");
			s_window->setBackgroundPic(m_currentPage->imagePath);
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

		int myInt[] = {1,2,3};
		std::vector<int> vec(std::begin(myInt), std::end(myInt));
	}

	void Show(Guide* guide, int index)
	{
		if (guide == nullptr)
		{
			return;
		}

		Close();
		guide->Show(index);
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
