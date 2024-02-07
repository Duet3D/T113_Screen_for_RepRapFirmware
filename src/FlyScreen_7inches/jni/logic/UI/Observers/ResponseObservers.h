/*
 * PushObservers.h
 *
 *  Created on: 2 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_UI_OBSERVERS_RESPONSEOBSERVERS_HPP_
#define JNI_LOGIC_UI_OBSERVERS_RESPONSEOBSERVERS_HPP_

#include "Debug.h"
#include "Duet3D/General/String.h"
#include "UI/OmObserver.h"
#include "UI/UserInterface.h"
#include "UI/UserInterfaceConstants.h"
#include "manager/LanguageManager.h"
#include "restclient-cpp/restclient.h"
#include <algorithm>
#include <string>

/*
 * These functions are run when the OM field is received.
 * The function takes 2 arguments:
 * 	- val data from the OM where the type is automatically converted based on the chosen macro
 * 	- indices is an array of the indicies for the OM key
 * The _IF_CHANGED suffix only runs the function if the data is different from the previous
 * time function was called. This is unique to each combination of indices.
 */

static UI::Observer<UI::ui_field_update_cb> PushObserversField[] = {
	OBSERVER_CHAR("resp",
				  [](OBSERVER_CHAR_ARGS) {
					  info("resp: %s", val);
					  static std::string str;
					  size_t substrlen;

					  str = val;
					  for (size_t i = 0; i < str.length(); i += MaxResponseLineLength)
					  {
						  String<MaxResponseLineLength> line;
						  substrlen = std::min(str.length() - i, MaxResponseLineLength);
						  dbg("resp: str.length()=%d, i=%d, substrlen=%d", str.length(), i, substrlen);
						  line.copy(str.substr(i, substrlen).c_str());
						  UI::CONSOLE->AddResponse(line.GetRef());
					  }

					  // Only show if console is not visible and a M291 message box is not showing
					  if (!mConsoleWindowPtr->isVisible() && UI::POPUP_WINDOW->IsResponse())
					  {
						  UI::POPUP_WINDOW->Open([]() {
							  UI::WINDOW->CloseLastWindow();
							  UI::WINDOW->OpenWindow(mConsoleWindowPtr);
						  });
						  UI::POPUP_WINDOW->SetText(val);
						  UI::POPUP_WINDOW->SetOkBtnText(LANGUAGEMANAGER->getValue("open_console").c_str());
					  }
					  UI::CONSOLE->Refresh();
				  }),
	OBSERVER_CHAR("message", [](OBSERVER_CHAR_ARGS) { info("message: %s", val); }),
	OBSERVER_CHAR("seq", [](OBSERVER_CHAR_ARGS) { info("seq: %s", val); }),
	OBSERVER_INT_IF_CHANGED("seqs:reply",
							[](OBSERVER_INT_ARGS) {
								dbg("seqs.reply: %d", val);
								if (Comm::duet.GetCommunicationType() == Comm::Duet::CommunicationType::network)
								{
									info("New reply available");
									RestClient::Response r;
									Comm::duet.RequestReply(r);
									Comm::duet.ProcessReply(r);
								}
							}),
};

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
static UI::Observer<UI::ui_array_end_update_cb> PushObserversArrayEnd[] = {
	OBSERVER_ARRAY_END("", [](OBSERVER_ARRAY_END_ARGS) {}),
};

#endif /* JNI_LOGIC_UI_OBSERVERS_RESPONSEOBSERVERS_HPP_ */
