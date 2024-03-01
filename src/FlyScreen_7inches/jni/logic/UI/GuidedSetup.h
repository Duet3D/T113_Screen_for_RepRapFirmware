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

		Page(const char* guideId,
			 const char* imagePath,
			 function<void(void)> nextCb,
			 function<void(void)> previousCb,
			 ZKWindow* window);
	};

	class Guide
	{
	  public:
		Guide(const char* id, bool closeAnytime = false);
		~Guide();
		void AddPage(Page& page) { m_pages.push_back(page); }
		void NextPage();
		void PreviousPage();
		bool Show(size_t index = 0);
		void Close();
		size_t GetPageCount() { return m_pages.size(); }

	  private:
		bool SetWindowVisible(bool show);
		bool SetBackground();
		bool RunNextCallback();
		bool RunPreviousCallback();

		size_t m_index;
		Page* m_currentPage;
		std::vector<Page> m_pages;
	};

	void Init(ZKWindow* window);
	void AddPage(const char* guideId, Page& page);
	void Show(const char* guideId, size_t index = 0);
	void Show(Guide* guide, size_t index = 0);
	Guide* GetGuide(const char* id);
	Guide* GetCurrentGuide();
	void Close();
} // namespace UI::GuidedSetup

#endif /* JNI_LOGIC_UI_GUIDEDSETUP_H_ */
