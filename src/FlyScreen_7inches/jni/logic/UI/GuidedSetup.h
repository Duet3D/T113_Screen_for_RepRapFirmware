/*
 * GuidedSetup.h
 *
 *  Created on: 28 Feb 2024
 *      Author: andy
 */

#ifndef JNI_LOGIC_UI_GUIDEDSETUP_H_
#define JNI_LOGIC_UI_GUIDEDSETUP_H_

#include "window/ZKWindow.h"

#include "std_fixed/functional.h"
#include "uart/CommDef.h"
#include <map>
#include <vector>

namespace UI::GuidedSetup
{

	class Page
	{
	  public:
		const char* imagePath;			 // loaded when displaying this page
		function<void(void)> nextCb;	 // run this when next button pressed
		function<void(void)> previousCb; // run this when previous button pressed
		ZKWindow* window; // if not null, display this window as it might provide additional buttons or user input

		Page(const char* imagePath, function<void(void)> nextCb, function<void(void)> previousCb, ZKWindow* window)
			: imagePath(imagePath), nextCb(nextCb), previousCb(previousCb), window(window)
		{
		}
	};

	class Guide
	{
	  public:
		Guide(std::vector<Page> pages) : m_index(0), m_currentPage(nullptr), m_pages(pages) {}
		~Guide();
		void Init(std::vector<Page> pages) { m_pages = pages; }
		void NextPage();
		void PreviousPage();
		bool Show(int index = 0);
		void Close();
		size_t GetPageCount() { return m_pages.size(); }

	  private:
		bool SetWindowVisible(bool show);
		bool SetBackground();
		bool RunNextCallback();
		bool RunPreviousCallback();

		int m_index;
		Page* m_currentPage;
		std::vector<Page> m_pages;
	};

	void Init(ZKWindow* window);
	void Show(Guide* guide, int index = 0);
	void Close();
} // namespace UI::GuidedSetup

#endif /* JNI_LOGIC_UI_GUIDEDSETUP_H_ */
