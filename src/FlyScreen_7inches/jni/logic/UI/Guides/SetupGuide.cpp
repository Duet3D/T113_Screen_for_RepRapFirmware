#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "UI/GuidedSetup.h"

namespace UI::GuidedSetup
{
	Guide setupGuide("setup", false);

	Page setupPages[] = {Page{
							 "setup",		  // guideId
							 "boot_logo.JPG", // imagePath
							 []() {			  // nextCb
								 dbg("page1 nextCb");
							 },
							 []() { // previousCb
								 dbg("page1 previousCb");
							 },
							 nullptr // window
						 },
						 Page{
							 "setup",			   // guideId
							 "fileicon.png", // imagePath
							 []() {				   // nextCb
								 dbg("page2 nextCb");
							 },
							 []() { // previousCb
								 dbg("page2 previousCb");
							 },
							 nullptr // window
						 }};

} // namespace UI::GuidedSetup
