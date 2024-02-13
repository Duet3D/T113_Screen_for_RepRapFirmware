/*
 * StateObservers.h
 *
 *  Created on: 9 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_UI_OBSERVERS_STATEOBSERVERS_HPP_
#define JNI_LOGIC_UI_OBSERVERS_STATEOBSERVERS_HPP_

#include "Debug.h"

#include "utils/TimeHelper.h"

#include "ObjectModel/Alert.h"
#include "ObjectModel/PrinterStatus.h"
#include "UI/OmObserver.h"
#include "UI/UserInterface.h"
#include "UI/UserInterfaceConstants.h"

/*
 * These functions are run when the OM field is received.
 * The function takes 2 arguments:
 * 	- val data from the OM where the type is automatically converted based on the chosen macro
 * 	- indices is an array of the indicies for the OM key
 * The _IF_CHANGED suffix only runs the function if the data is different from the previous
 * time function was called. This is unique to each combination of indices.
 */
static UI::Observer<UI::ui_field_update_cb> StateObserversField[] = {
	OBSERVER_CHAR("state:status",
				  [](OBSERVER_CHAR_ARGS) {
					  OM::SetStatus(val);
					  switch (OM::GetStatus())
					  {
					  case OM::PrinterStatus::printing:
					  case OM::PrinterStatus::simulating:
						  mPrintPauseBtnPtr->setVisible(true);
						  mPrintResumeBtnPtr->setVisible(false);
						  mPrintCancelBtnPtr->setVisible(false);
						  break;
					  case OM::PrinterStatus::paused:
					  case OM::PrinterStatus::pausing:
					  case OM::PrinterStatus::resuming:
						  mPrintPauseBtnPtr->setVisible(false);
						  mPrintResumeBtnPtr->setVisible(true);
						  mPrintCancelBtnPtr->setVisible(true);
						  break;
					  case OM::PrinterStatus::idle:
					  case OM::PrinterStatus::configuring:
					  case OM::PrinterStatus::connecting:
					  case OM::PrinterStatus::halted:
						  mPrintFileNamePtr->setText("");
						  mPrintElapsedTimePtr->setTextTr("elapsed");
						  mPrintEstimatedTimePtr->setTextTr("estimated");
						  mPrintPauseBtnPtr->setVisible(false);
						  mPrintResumeBtnPtr->setVisible(false);
						  mPrintCancelBtnPtr->setVisible(false);
						  break;
					  default:
						  break;
					  }
				  }),
	OBSERVER_INT("state:currentTool", [](OBSERVER_INT_ARGS) { OM::SetCurrentTool(val); }),
	OBSERVER_CHAR("state:messageBox",
				  [](OBSERVER_CHAR_ARGS) {
					  info("messageBox: %s", val);
					  if (val[0] != 0)
						  return;
					  UI::PopupWindow* popup = UI::PopupWindow::GetInstance();
					  if (!popup->IsOpen() || popup->IsResponse())
						  return;
					  popup->Close();
					  OM::currentAlert.Reset();
				  }),
	OBSERVER_UINT("state:messageBox:axisControls",
				  [](OBSERVER_UINT_ARGS) {
					  OM::currentAlert.controls = val;
					  OM::currentAlert.flags.SetBit(OM::Alert::GotControls);
				  }),
	OBSERVER_CHAR("state:messageBox:message",
				  [](OBSERVER_CHAR_ARGS) {
					  OM::currentAlert.text.copy(val);
					  OM::currentAlert.flags.SetBit(OM::Alert::GotText);
				  }),
	OBSERVER_INT("state:messageBox:mode",
				 [](OBSERVER_INT_ARGS) {
					 OM::currentAlert.mode = static_cast<OM::Alert::Mode>(val);
					 OM::currentAlert.flags.SetBit(OM::Alert::GotMode);
				 }),
	OBSERVER_UINT("state:messageBox:seq",
				  [](OBSERVER_UINT_ARGS) {
					  OM::currentAlert.seq = val;
					  OM::currentAlert.flags.SetBit(OM::Alert::GotSeq);
				  }),
	OBSERVER_FLOAT("state:messageBox:timeout",
				   [](OBSERVER_FLOAT_ARGS) {
					   OM::currentAlert.timeout = val;
					   OM::currentAlert.flags.SetBit(OM::Alert::GotTimeout);
				   }),
	OBSERVER_CHAR("state:messageBox:title",
				  [](OBSERVER_CHAR_ARGS) {
					  OM::currentAlert.title.copy(val);
					  OM::currentAlert.flags.SetBit(OM::Alert::GotTitle);
				  }),
	OBSERVER_CHAR("state:messageBox:min",
				  [](OBSERVER_CHAR_ARGS) {
					  Comm::GetInteger(val, OM::currentAlert.limits.numberInt.min);
					  Comm::GetFloat(val, OM::currentAlert.limits.numberFloat.min);
					  Comm::GetInteger(val, OM::currentAlert.limits.text.min);
				  }),
	OBSERVER_CHAR("state:messageBox:max",
				  [](OBSERVER_CHAR_ARGS) {
					  Comm::GetInteger(val, OM::currentAlert.limits.numberInt.max);
					  Comm::GetFloat(val, OM::currentAlert.limits.numberFloat.max);
					  Comm::GetInteger(val, OM::currentAlert.limits.text.max);
				  }),
	OBSERVER_CHAR("state:messageBox:default",
				  [](OBSERVER_CHAR_ARGS) {
					  Comm::GetInteger(val, OM::currentAlert.limits.numberInt.valueDefault);
					  Comm::GetFloat(val, OM::currentAlert.limits.numberFloat.valueDefault);
					  OM::currentAlert.limits.text.valueDefault.copy(val);
				  }),
	OBSERVER_CHAR("state:messageBox:cancelButton",
				  [](OBSERVER_CHAR_ARGS) { Comm::GetBool(val, OM::currentAlert.cancelButton); }),
	OBSERVER_CHAR("state:messageBox:choices^",
				  [](OBSERVER_CHAR_ARGS) {
					  if (indices[0] >= OM::alertMaxChoices)
						  return;
					  OM::currentAlert.choices[indices[0]].copy(val);
					  OM::currentAlert.choices_count = indices[0] + 1;
				  }),
	OBSERVER_CHAR("state:time",
				  [](OBSERVER_CHAR_ARGS) {
					  char* timeStr = (char*)val; // remove const
					  if (timeStr[0] == 0)
					  {
						  mDigitalClock1Ptr->setVisible(false);
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
					  mDigitalClock1Ptr->setVisible(true);
				  }),
};

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
static UI::Observer<UI::ui_array_end_update_cb> StateObserversArrayEnd[] = {};

#endif /* JNI_LOGIC_UI_OBSERVERS_STATEOBSERVERS_HPP_ */
