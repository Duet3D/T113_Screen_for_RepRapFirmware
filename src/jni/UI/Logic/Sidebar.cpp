/*
 * Sidebar.cpp
 *
 *  Created on: 24 April 2024
 *      Author: Andy Everitt
 */

#include "Debug.h"

#include "UI/UserInterface.h"

#include "Hardware/Duet.h"
#include "ObjectModel/BedOrChamber.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Tool.h"
#include "Sidebar.h"

namespace UI::Sidebar
{
	static ZKButton* s_toolTempSnapshot = nullptr;
	static ZKButton* s_bedTempSnapshot = nullptr;
	static ZKButton* s_chamberTempSnapshot = nullptr;

	void Init()
	{
		s_toolTempSnapshot = UI::GetUIControl<ZKButton>(ID_MAIN_ToolTempSnapshot);
		s_bedTempSnapshot = UI::GetUIControl<ZKButton>(ID_MAIN_BedTempSnapshot);
		s_chamberTempSnapshot = UI::GetUIControl<ZKButton>(ID_MAIN_ChamberTempSnapshot);
		UpdateTemperatureSnapshot();
	}

	void OpenMacros()
	{
		static ZKWindow* window = UI::GetUIControl<ZKWindow>(ID_MAIN_FilesWindow);

		info("Sidebar: Open Macros");
		OM::FileSystem::RequestFiles("0:/macros");
		UI::WINDOW.OpenWindow(window);
	}

	void OpenConsole()
	{
		static ZKWindow* window = UI::GetUIControl<ZKWindow>(ID_MAIN_ConsoleWindow);

		info("Sidebar: Open Console");
		UI::WINDOW.OpenWindow(window);
	}

	void EStop()
	{
		warn("EStop Pressed!");
		UI::POPUP_WINDOW.Close();
		Comm::DUET.SendGcode("M112 ;"
							 "\xF0"
							 "\x0F");
		warn("Emergency Stop sent to Duet");
		Thread::sleep(1000);
		Comm::DUET.SendGcode("M999");
		warn("Restart sent to Duet");
	}

	void UpdateTemperatureSnapshot()
	{
		static size_t s_spacing = 5;
		static size_t s_iconWidth = 25;

		std::string toolText;
		std::string bedText;
		std::string chamberText;

		size_t i = 0;
		OM::IterateToolsWhile(
			[&](OM::Tool*& tool, size_t)
			{
				if (tool == nullptr)
				{
					return true;
				}

				if (tool->status != OM::ToolStatus::active)
				{
					return true;
				}

				toolText += utils::format("T%d: ", tool->index);

				tool->IterateHeaters(
					[&](OM::ToolHeater* th, size_t)
					{
						if (th == nullptr)
						{
							return;
						}
						if (i > 0)
						{
							toolText += " / ";
						}
						toolText += utils::format("%.0f", th->heater->current);
						i++;
					});

				return true;
			});

		i = 0;
		OM::IterateBedsWhile(
			[&](OM::Bed*& bed, size_t)
			{
				if (bed == nullptr)
				{
					return true;
				}
				OM::Heat::Heater* heater = OM::Heat::GetHeater(bed->heater);
				if (heater == nullptr || heater->status != OM::Heat::HeaterStatus::active)
				{
					return true;
				}
				if (i > 0)
				{
					bedText += " / ";
				}
				bedText += utils::format("%.1f", heater->current);
				i++;
				return true;
			});

		i = 0;
		OM::IterateChambersWhile(
			[&](OM::Chamber*& chamber, size_t)
			{
				if (chamber == nullptr)
				{
					return true;
				}
				OM::Heat::Heater* heater = OM::Heat::GetHeater(chamber->heater);
				if (heater == nullptr || heater->status != OM::Heat::HeaterStatus::active)
				{
					return true;
				}
				if (i > 0)
				{
					chamberText += " / ";
				}
				chamberText += utils::format("%.1f", heater->current);
				i++;
				return true;
			});

		// Add suffixes
		if (!toolText.empty())
		{
			toolText += "°C";
		}
		if (!bedText.empty())
		{
			bedText += "°C";
		}
		if (!chamberText.empty())
		{
			chamberText += "°C";
		}

		LayoutPosition pos;
		pos.mLeft = 0;
		pos.mTop = 0;
		pos.mHeight = 25;

		int width, height;
		s_toolTempSnapshot->setText(toolText);
		s_toolTempSnapshot->getTextExtent(toolText.c_str(), width, height);
		pos.mWidth = toolText.empty() ? 0 : width + s_iconWidth + 2 * s_spacing + 10;
		s_toolTempSnapshot->setPosition(pos);
		dbg("Tool Text: \"%s\" (%d)", toolText.c_str(), width);
		dbg("Position: left=%d, width=%d, right=%d", pos.mLeft, pos.mWidth, pos.mLeft + pos.mWidth);

		s_bedTempSnapshot->setText(bedText);
		s_bedTempSnapshot->getTextExtent(bedText.c_str(), width, height);
		pos.mLeft += pos.mWidth + s_spacing;
		pos.mWidth = bedText.empty() ? 0 : width + s_iconWidth + 2 * s_spacing + 10;
		s_bedTempSnapshot->setPosition(pos);
		dbg("Bed Text: \"%s\" (%d)", bedText.c_str(), width);
		dbg("Position: left=%d, width=%d, right=%d", pos.mLeft, pos.mWidth, pos.mLeft + pos.mWidth);

		s_chamberTempSnapshot->setText(chamberText);
		s_chamberTempSnapshot->getTextExtent(chamberText.c_str(), width, height);
		pos.mLeft += pos.mWidth + s_spacing;
		pos.mWidth = chamberText.empty() ? 0 : width + s_iconWidth + 2 * s_spacing + 10;
		s_chamberTempSnapshot->setPosition(pos);
		dbg("Chamber Text: \"%s\" (%d)", chamberText.c_str(), width);
		dbg("Position: left=%d, width=%d, right=%d", pos.mLeft, pos.mWidth, pos.mLeft + pos.mWidth);
	}
} // namespace UI::Sidebar
