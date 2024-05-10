#include "Debug.h"

#include "UI/GuidedSetup.h"
#include "UI/UserInterface.h"

namespace UI::GuidedSetup
{
	Guide setupGuide("setup", true, ID_MAIN_SetupGuideWindow);

	Page setupPages[] = {Page{
							 "setup",		  // guideId
							 "boot_logo.JPG", // imagePath
							 []() {			  // nextCb
								 dbg("page1 nextCb");
							 },
							 []() { // previousCb
								 dbg("page1 previousCb");
							 },
							 NULL // window
						 },
						 Page{
							 "setup",		 // guideId
							 "fileicon.png", // imagePath
							 []() {			 // nextCb
								 dbg("page2 nextCb");
							 },
							 []() { // previousCb
								 dbg("page2 previousCb");
							 },
							 ID_MAIN_PageOverlayExampleWindow // window
						 }};

} // namespace UI::GuidedSetup
