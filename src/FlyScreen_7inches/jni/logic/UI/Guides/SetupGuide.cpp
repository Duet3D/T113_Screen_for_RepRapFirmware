#include "UI/GuidedSetup.h"

namespace UI::GuidedSetup
{
	Guide setupGuide("setup");

	Page setupPages[] = {Page{
							 "setup",	   // guideId
							 "imagePath1", // imagePath
							 []() {		   // nextCb

							 },
							 []() { // previousCb

							 },
							 nullptr // window
						 },
						 Page{
							 "setup",
							 "imagePath2",
							 []() { // nextCb

							 },
							 []() { // previousCb

							 },
							 nullptr // window
						 }};

} // namespace UI::GuidedSetup
