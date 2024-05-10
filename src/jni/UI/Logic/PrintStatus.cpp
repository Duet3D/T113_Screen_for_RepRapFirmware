/*
 * PrintStatus.cpp
 *
 *  Created on: 24 April 2024
 *      Author: Andy Everitt
 */

#include "Debug.h"

#include "PrintStatus.h"

#include "Hardware/Duet.h"
#include "ObjectModel/Fan.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Job.h"
#include "ObjectModel/PrinterStatus.h"
#include "Storage.h"
#include "comm/FileInfo.h"
#include <manager/LanguageManager.h>
#include <storage/StoragePreferences.h>

namespace UI::PrintStatus
{
	static ToolsList s_toolsList;
	static ZKTextView* s_printFileName = nullptr;
	static ZKButton* s_babyStepDown = nullptr;
	static ZKButton* s_babyStepUp = nullptr;
	static float s_babyStepAmount = DEFAULT_BABY_STEP_SIZE;

	static void SetBabyStepAmount(float amount)
	{
		if (amount <= 0.0f || amount >= 10.0f)
		{
			warn("Invalid babystep amount: %.3f", amount);
			return;
		}
		StoragePreferences::putFloat(ID_BABYSTEP_AMOUNT, amount);
		s_babyStepAmount = amount;
		s_babyStepDown->setTextf("-%.3f mm", s_babyStepAmount);
		s_babyStepUp->setTextf("%.3f mm", s_babyStepAmount);
	}

	class IMETextUpdateListener : public IMEContext::IIMETextUpdateListener
	{
	  public:
		virtual void onIMETextUpdate(const std::string& text)
		{
			float amount = atof(text.c_str());
			SetBabyStepAmount(amount);
		}
	};

	class BabyStepLongClickListener : public ZKBase::ILongClickListener
	{
		virtual void onLongClick(ZKBase* pBase)
		{
			static IMETextUpdateListener listener;
			static IMEContext::SIMETextInfo info;

			dbg("Long click on babystep button");

			info.imeTextType = IMEContext::E_IME_TEXT_TYPE_NUMBER;
			info.isPassword = false;
			info.text = utils::format("%.3f", s_babyStepAmount);

			EASYUICONTEXT->showIME(&info, &listener);
		}
	};

	void Init()
	{
		static BabyStepLongClickListener s_babyStepListener;

		info("Initialising PrintStatus UI...");

		ZKListView* toolList = UI::GetUIControl<ZKListView>(ID_MAIN_PrintTemperatureList);
		s_printFileName = UI::GetUIControl<ZKTextView>(ID_MAIN_PrintFileName);

		s_babyStepDown = UI::GetUIControl<ZKButton>(ID_MAIN_PrintBabystepDecBtn);
		s_babyStepUp = UI::GetUIControl<ZKButton>(ID_MAIN_PrintBabystepIncBtn);

		SetBabyStepAmount(StoragePreferences::getFloat(ID_BABYSTEP_AMOUNT, DEFAULT_BABY_STEP_SIZE));

		s_babyStepDown->setLongClickListener(&s_babyStepListener);
		s_babyStepUp->setLongClickListener(&s_babyStepListener);

		if (toolList == nullptr)
		{
			error("Failed to get tool list");
			return;
		}

		s_toolsList.Init(toolList);
	}

	void Open()
	{
		UI::WINDOW.OpenWindow(ID_MAIN_PrintWindow);
	}

	void BabyStepDown()
	{
		Comm::DUET.SendGcodef("M290 S-%.3f", s_babyStepAmount);
	}

	void BabyStepUp()
	{
		Comm::DUET.SendGcodef("M290 S%.3f", s_babyStepAmount);
	}

	void UpdateFileName()
	{
		const bool isPrinting = OM::PrintInProgress();
		if (isPrinting)
		{
			std::string jobName = OM::GetJobName();
			size_t pos = jobName.find_last_of("/");
			if (pos != std::string::npos)
			{
				jobName = jobName.substr(pos + 1);
			}
			s_printFileName->setTextTrf("filename_in_progress", jobName.c_str());
		}
		else if (!OM::GetLastJobName().empty())
		{
			std::string jobName = OM::GetLastJobName();
			size_t pos = jobName.find_last_of("/");
			if (pos != std::string::npos)
			{
				jobName = jobName.substr(pos + 1);
			}
			s_printFileName->setTextTrf("filename_finished", jobName.c_str());
		}
		else
		{
			s_printFileName->setTextTr("no_job_running");
		}
		s_printFileName->setLongMode(ZKTextView::E_LONG_MODE_SCROLL);
	}

	void UpdateEstimatedPrintTime()
	{
		dbg("Updating estimated print time");
		uint32_t seconds = OM::GetPrintRemaining(OM::RemainingTimeType::slicer);
		int percentage = 0;
		if (OM::GetPrintTime() != 0)
		{
			percentage =
				std::min<int>((100 * std::max<int>(0, (int)OM::GetPrintDuration() - (int)OM::GetWarmUpDuration())) /
								  OM::GetPrintTime(),
							  100);
		}
		tm time = Comm::ParseSeconds(seconds);
		UI::GetUIControl<ZKTextView>(ID_MAIN_PrintEstimatedTime)
			->setTextTrf("estimated", time.tm_hour, time.tm_min, time.tm_sec);
		UI::GetUIControl<ZKCircleBar>(ID_MAIN_PrintProgressBar)->setProgress(percentage);
	}

	void UpdateElapsedTime(uint32_t seconds)
	{
		tm time = Comm::ParseSeconds(seconds);
		UI::GetUIControl<ZKTextView>(ID_MAIN_PrintElapsedTime)
			->setTextTrf("elapsed", time.tm_hour, time.tm_min, time.tm_sec);
	}

	size_t GetFanListCount()
	{
		return OM::GetFanCount();
	}

	void SetFanListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		verbose("%d", index);
		OM::Fan* fan = OM::GetFanBySlot(index);
		if (fan == nullptr)
		{
			return;
		}
		pListItem->setTextTrf("fan_status", fan->index, (int)(100 * fan->requestedValue));
	}

	void FanListItemCallback(const int index)
	{
		OM::Fan* fan = OM::GetFanBySlot(index);
		if (fan == nullptr)
		{
			return;
		}
		size_t fanIndex = fan->index;
		UI::OpenSliderNumPad(
			utils::format(LANGUAGEMANAGER->getValue("fan_header").c_str(), fan->index).c_str(),
			"",
			"",
			"%",
			0,
			100,
			(int)(fan->requestedValue * 100),
			[fanIndex](int percent) {
				OM::Fan* fan = OM::GetFan(fanIndex);
				if (fan == nullptr)
				{
					return;
				}
				int fanSpeed = (percent * 255) / 100;
				Comm::DUET.SendGcodef("M106 P%d S%d\n", fan->index, fanSpeed);
			},
			true);
	}

	void PausePrint()
	{
		OM::FileSystem::PausePrint();
	}

	void StopPrint()
	{
		OM::FileSystem::StopPrint();
		UI::WINDOW.Home();
	}

	void ResumePrint()
	{
		OM::FileSystem::ResumePrint();
	}

	void PrintAgain()
	{
		OM::FileSystem::StartPrint(OM::GetLastJobName());
	}

	size_t GetAxisListCount()
	{
		return OM::Move::GetAxisCount();
	}

	void SetAxisListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		verbose("%d", index);
		ZKListView::ZKListSubItem* pMachinePosition =
			pListItem->findSubItemByID(ID_MAIN_PrintPositionListMachinePositionSubItem);
		ZKListView::ZKListSubItem* pUserPosition =
			pListItem->findSubItemByID(ID_MAIN_PrintPositionListUserPositionSubItem);
		OM::Move::Axis* axis = OM::Move::GetAxisBySlot(index);
		if (axis == nullptr)
			return;

		pListItem->setText(axis->letter);
		pUserPosition->setText(axis->userPosition);
		pMachinePosition->setTextf("(%.2f)", axis->machinePosition);
	}

	size_t GetExtruderListCount()
	{
		return OM::Move::GetExtruderAxisCount();
	}

	void SetExtruderListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		verbose("%d", index);
		ZKListView::ZKListSubItem* pSubItem1 = pListItem->findSubItemByID(ID_MAIN_PrintExtruderPositionListSubItem1);
		ZKListView::ZKListSubItem* pSubItem2 = pListItem->findSubItemByID(ID_MAIN_PrintExtruderPositionListSubItem2);
		OM::Move::ExtruderAxis* extruder = OM::Move::GetExtruderAxisBySlot(index);
		if (extruder == nullptr)
			return;

		pListItem->setText(extruder->index);
		pSubItem1->setText(extruder->position);
		pSubItem2->setTextf("%.1f%%", extruder->factor * 100);
	}

	void ExtruderListItemCallback(const int index)
	{
		OM::Move::ExtruderAxis* extruder = OM::Move::GetExtruderAxisBySlot(index);
		if (extruder == nullptr)
		{
			return;
		}
		size_t extruderIndex = extruder->index;
		UI::OpenSliderNumPad(
			utils::format(LANGUAGEMANAGER->getValue("extrusion_factor_header").c_str(), extruderIndex).c_str(),
			"",
			"",
			"%",
			1,
			200,
			(int)(extruder->factor * 100),
			[extruderIndex](int percent) {
				OM::Move::ExtruderAxis* extruder = OM::Move::GetExtruderAxis(extruderIndex);
				if (extruder == nullptr)
				{
					return;
				}
				Comm::DUET.SendGcodef("M221 D%d S%d\n", extruder->index, percent);
			});
	}

	void SpeedMultiplierCallback(const int progress)
	{
		Comm::DUET.SendGcodef("M220 S%d\n", progress);
	}

	void OpenPrintSpeedFactorPopup()
	{
		UI::OpenSliderNumPad(LANGUAGEMANAGER->getValue("speed_factor").c_str(),
							 "",
							 "",
							 "%",
							 10,
							 1000,
							 (int)(OM::Move::GetSpeedFactor() * 100),
							 [](int percent) { SpeedMultiplierCallback(percent); });
	}

	size_t GetToolsListCount()
	{
		return s_toolsList.GetTotalHeaterCount(false);
	}

	void SetToolsListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		verbose("%d", index);
		s_toolsList.ObtainListItemData(pListItem,
									   index,
									   ID_MAIN_PrintTemperatureListNameSubItem,
									   ID_MAIN_PrintTemperatureListStatusSubItem,
									   ID_MAIN_PrintTemperatureListCurrentSubItem,
									   ID_MAIN_PrintTemperatureListActiveSubItem,
									   ID_MAIN_PrintTemperatureListStandbySubItem);
	}

	void ToolsListItemCallback(const int index, const int id)
	{
		s_toolsList.OnListItemClick(index,
									id,
									NULL,
									NULL,
									ID_MAIN_PrintTemperatureListActiveSubItem,
									ID_MAIN_PrintTemperatureListStandbySubItem);
	}
} // namespace UI::PrintStatus
