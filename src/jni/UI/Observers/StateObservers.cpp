/*
 * StateObservers.cpp
 *
 *  Created on: 9 Jan 2024
 *      Author: Andy Everitt
 */
#include "Debug.h"

#include "Configuration.h"
#include "UI/Logic/PrintStatus.h"
#include "UI/OmObserver.h"
#include "UI/UserInterface.h"
#include "control/ZKDigitalClock.h"
#include "utils/TimeHelper.h"

#include "Hardware/Duet.h"
#include "ObjectModel/Alert.h"
#include "ObjectModel/Job.h"
#include "ObjectModel/PrinterStatus.h"
#include <manager/LanguageManager.h>

/*
 * These functions are run when the OM field is received.
 * The function takes 2 arguments:
 * 	- val data from the OM where the type is automatically converted based on the chosen macro
 * 	- indices is an array of the indicies for the OM key
 * The _IF_CHANGED suffix only runs the function if the data is different from the previous
 * time function was called. This is unique to each combination of indices.
 */
static UI::Observer<UI::ui_field_update_cb> StateObserversField[] = {
	OBSERVER_CHAR("network:name",
				  [](OBSERVER_CHAR_ARGS)
				  {
					  OM::SetPrinterName(val);
					  UI::GetUIControl<ZKTextView>(ID_MAIN_PrinterName)->setText(val);
				  }),
	OBSERVER_CHAR("network:interfaces^:actualIP", [](OBSERVER_CHAR_ARGS) { Comm::DUET.SetIPAddress(val); }),
	OBSERVER_CHAR("state:status",
				  [](OBSERVER_CHAR_ARGS)
				  {
					  OM::PrinterStatus prevStatus = OM::GetStatus();
					  OM::SetStatus(val);
					  switch (OM::GetStatus())
					  {
					  case OM::PrinterStatus::printing:
					  case OM::PrinterStatus::simulating:
					  case OM::PrinterStatus::resuming:
						  if (prevStatus == OM::PrinterStatus::idle || prevStatus == OM::PrinterStatus::paused)
						  {
							  UI::PrintStatus::Open();
						  }
						  UI::GetUIControl<ZKButton>(ID_MAIN_PrintResumeBtn)->setVisible(false);
						  UI::GetUIControl<ZKButton>(ID_MAIN_PrintPauseBtn)->setVisible(true);
						  UI::GetUIControl<ZKButton>(ID_MAIN_PrintCancelBtn)->setVisible(true);
						  UI::GetUIControl<ZKButton>(ID_MAIN_PrintAgainBtn)->setVisible(false);

						  UI::GetUIControl<ZKButton>(ID_MAIN_PrintPauseBtn)->setInvalid(false);
						  UI::GetUIControl<ZKButton>(ID_MAIN_PrintCancelBtn)->setInvalid(true);
						  break;
					  case OM::PrinterStatus::paused:
					  case OM::PrinterStatus::pausing:
						  UI::GetUIControl<ZKButton>(ID_MAIN_PrintResumeBtn)->setVisible(true);
						  UI::GetUIControl<ZKButton>(ID_MAIN_PrintPauseBtn)->setVisible(false);
						  UI::GetUIControl<ZKButton>(ID_MAIN_PrintCancelBtn)->setVisible(true);
						  UI::GetUIControl<ZKButton>(ID_MAIN_PrintAgainBtn)->setVisible(false);

						  UI::GetUIControl<ZKButton>(ID_MAIN_PrintResumeBtn)->setInvalid(false);
						  UI::GetUIControl<ZKButton>(ID_MAIN_PrintCancelBtn)->setInvalid(false);
						  break;
					  case OM::PrinterStatus::idle:
					  case OM::PrinterStatus::configuring:
					  case OM::PrinterStatus::connecting:
					  case OM::PrinterStatus::halted:
						  UI::PrintStatus::UpdateFileName();
						  UI::GetUIControl<ZKTextView>(ID_MAIN_PrintElapsedTime)->setTextTrf("elapsed", 0, 0, 0);
						  UI::GetUIControl<ZKTextView>(ID_MAIN_PrintEstimatedTime)->setTextTrf("estimated", 0, 0, 0);

						  UI::GetUIControl<ZKButton>(ID_MAIN_PrintResumeBtn)->setVisible(false);
						  UI::GetUIControl<ZKButton>(ID_MAIN_PrintPauseBtn)->setVisible(true);
						  UI::GetUIControl<ZKButton>(ID_MAIN_PrintCancelBtn)->setVisible(false);
						  UI::GetUIControl<ZKButton>(ID_MAIN_PrintAgainBtn)->setVisible(true);

						  UI::GetUIControl<ZKButton>(ID_MAIN_PrintPauseBtn)->setInvalid(true);
						  UI::GetUIControl<ZKButton>(ID_MAIN_PrintResumeBtn)->setInvalid(true);
						  UI::GetUIControl<ZKButton>(ID_MAIN_PrintCancelBtn)->setInvalid(true);
						  UI::GetUIControl<ZKButton>(ID_MAIN_PrintAgainBtn)->setInvalid(OM::GetLastJobName().empty());
						  break;
					  default:
						  break;
					  }
				  }),
	OBSERVER_INT("state:currentTool", [](OBSERVER_INT_ARGS) { OM::SetCurrentTool(val); }),
	OBSERVER_CHAR("state:messageBox",
				  [](OBSERVER_CHAR_ARGS)
				  {
					  info("messageBox: %s", val);
					  if (val[0] != 0)
						  return;
					  UI::PopupWindow& popup = UI::PopupWindow::GetInstance();
					  OM::g_currentAlert.Reset();
					  if (!popup.IsOpen() || popup.IsResponse())
					  {
						  return;
					  }
					  popup.Close();
				  }),
	OBSERVER_UINT("state:messageBox:axisControls",
				  [](OBSERVER_UINT_ARGS)
				  {
					  OM::g_currentAlert.controls = val;
					  OM::g_currentAlert.flags.SetBit(OM::Alert::GotControls);
				  }),
	OBSERVER_CHAR("state:messageBox:message",
				  [](OBSERVER_CHAR_ARGS)
				  {
					  OM::g_currentAlert.text.copy(val);
					  OM::g_currentAlert.flags.SetBit(OM::Alert::GotText);
				  }),
	OBSERVER_INT("state:messageBox:mode",
				 [](OBSERVER_INT_ARGS)
				 {
					 dbg("Received alert mode %d", val);
					 OM::g_currentAlert.mode = static_cast<OM::Alert::Mode>(val);
					 OM::g_currentAlert.flags.SetBit(OM::Alert::GotMode);
				 }),
	OBSERVER_UINT("state:messageBox:seq",
				  [](OBSERVER_UINT_ARGS)
				  {
					  OM::g_currentAlert.seq = val;
					  OM::g_currentAlert.flags.SetBit(OM::Alert::GotSeq);
				  }),
	OBSERVER_FLOAT("state:messageBox:timeout",
				   [](OBSERVER_FLOAT_ARGS)
				   {
					   OM::g_currentAlert.timeout = val;
					   OM::g_currentAlert.flags.SetBit(OM::Alert::GotTimeout);
				   }),
	OBSERVER_CHAR("state:messageBox:title",
				  [](OBSERVER_CHAR_ARGS)
				  {
					  OM::g_currentAlert.title.copy(val);
					  OM::g_currentAlert.flags.SetBit(OM::Alert::GotTitle);
				  }),
	OBSERVER_CHAR("state:messageBox:min",
				  [](OBSERVER_CHAR_ARGS)
				  {
					  Comm::GetInteger(val, OM::g_currentAlert.limits.numberInt.min);
					  Comm::GetFloat(val, OM::g_currentAlert.limits.numberFloat.min);
					  Comm::GetInteger(val, OM::g_currentAlert.limits.text.min);
				  }),
	OBSERVER_CHAR("state:messageBox:max",
				  [](OBSERVER_CHAR_ARGS)
				  {
					  Comm::GetInteger(val, OM::g_currentAlert.limits.numberInt.max);
					  Comm::GetFloat(val, OM::g_currentAlert.limits.numberFloat.max);
					  Comm::GetInteger(val, OM::g_currentAlert.limits.text.max);
				  }),
	OBSERVER_CHAR("state:messageBox:default",
				  [](OBSERVER_CHAR_ARGS)
				  {
					  Comm::GetInteger(val, OM::g_currentAlert.limits.numberInt.valueDefault);
					  Comm::GetFloat(val, OM::g_currentAlert.limits.numberFloat.valueDefault);
					  OM::g_currentAlert.limits.text.valueDefault.copy(val);
				  }),
	OBSERVER_CHAR("state:messageBox:cancelButton",
				  [](OBSERVER_CHAR_ARGS) { Comm::GetBool(val, OM::g_currentAlert.cancelButton); }),
	OBSERVER_CHAR("state:messageBox:choices^",
				  [](OBSERVER_CHAR_ARGS)
				  {
					  if (indices[0] >= ALERT_MAX_CHOICES)
						  return;
					  OM::g_currentAlert.choices[indices[0]].copy(val);
					  OM::g_currentAlert.choices_count = indices[0] + 1;
				  }),
	OBSERVER_CHAR("state:time",
				  [](OBSERVER_CHAR_ARGS)
				  {
					  static unsigned long long lastUpdated = 0;
					  char* timeStr = (char*)val; // remove const
					  if (timeStr[0] == 0)
					  {
						  UI::GetUIControl<ZKDigitalClock>(ID_MAIN_DigitalClock1)->setVisible(false);
						  return;
					  }

					  if (TimeHelper::getCurrentTime() - lastUpdated < TIME_SYNC_INTERVAL)
					  {
						  return;
					  }

					  dbg("Setting system time to %s", val);
					  for (size_t i = 0; timeStr[i] != 0; ++i)
					  {
						  if (timeStr[i] == 'T')
						  {
							  timeStr[i] = ' ';
							  break;
						  }
					  }
					  TimeHelper::setDateTime(timeStr);
					  lastUpdated = TimeHelper::getCurrentTime();
					  UI::GetUIControl<ZKDigitalClock>(ID_MAIN_DigitalClock1)->setVisible(true);
				  }),
};

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
static UI::Observer<UI::ui_array_end_update_cb> StateObserversArrayEnd[] = {};
