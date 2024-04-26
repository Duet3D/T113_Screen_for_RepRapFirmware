#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "Storage.h"
#include "Themes.h"
#include "control/ZKSlideText.h"
#include <map>
#include <storage/StoragePreferences.h>
#include <string>
#include <typeinfo>

namespace UI::Theme
{
	static std::map<std::string, Theme*> s_themes;
	static std::map<ZKListView::ZKListItem*, Theme*> s_themedListItems;
	static Theme* s_currentTheme = nullptr;

	Theme::Theme(const char* id,
				 ThemeColors* colors,
				 function<void(void)> overrides,
				 function<void(ZKListView*, ZKListView::ZKListItem*, int)> listItemOverrides)
		: id(id), colors(colors), overrides(overrides), listItemOverrides(listItemOverrides)
	{
		CreateTheme(this);
	}

	void CreateTheme(Theme* theme)
	{
		if (s_themes.find(theme->id) != s_themes.end())
		{
			warn("Theme %s already exists", theme->id.c_str());
			return;
		}
		info("Creating theme %s", theme->id.c_str());
		s_themes[theme->id] = theme;
	}

	int GetThemeCount()
	{
		return s_themes.size();
	}

	const Theme* const GetCurrentTheme()
	{
		if (s_currentTheme == nullptr)
		{
			warn("No theme set, using first available theme");
			Theme* theme = GetThemeByIndex(0);
			SetTheme(theme);
			return theme;
		}
		return s_currentTheme;
	}

	Theme* GetThemeByIndex(int index)
	{
		if (index < 0 || index >= (int)s_themes.size())
		{
			return nullptr;
		}
		auto it = s_themes.begin();
		for (int i = 0; i < index; i++)
		{
			it++;
		}
		return it->second;
	}

	static void ThemeControls(ThemeColors* colors, std::vector<ZKBase*> controls)
	{
		dbg("%d controls in window to theme", controls.size());
		for (auto control : controls)
		{
			dbg("Theming control ID %d, type %s, typeid %s",
				control->getID(),
				control->getClassName(),
				typeid(*control).name());
			if (typeid(*control) == typeid(ZKWindow))
			{
				ZKWindow* window = static_cast<ZKWindow*>(control);
				window->setBackgroundColor(colors->window.bgDefault);
				window->setBackgroundPic(colors->window.bgImage);
				continue;
			}
			if (typeid(*control) == typeid(ZKTextView))
			{
				ZKTextView* text = static_cast<ZKTextView*>(control);
				// Background
				text->setBackgroundColor(colors->text.bgDefault);
				text->setBackgroundPic(colors->text.bgImage);
				text->setBgStatusColor(ZK_CONTROL_STATUS_NORMAL, colors->text.background.normal);
				text->setBgStatusColor(ZK_CONTROL_STATUS_PRESSED, colors->text.background.pressed);
				text->setBgStatusColor(ZK_CONTROL_STATUS_SELECTED, colors->text.background.selected);
				text->setBgStatusColor(ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED,
									   colors->text.background.pressedAndSelected);
				text->setBgStatusColor(ZK_CONTROL_STATUS_INVALID, colors->text.background.invalid);

				// Foreground
				text->setTextStatusColor(ZK_CONTROL_STATUS_NORMAL, colors->text.foreground.normal);
				text->setTextStatusColor(ZK_CONTROL_STATUS_PRESSED, colors->text.foreground.pressed);
				text->setTextStatusColor(ZK_CONTROL_STATUS_SELECTED, colors->text.foreground.selected);
				text->setTextStatusColor(ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED,
										 colors->text.foreground.pressedAndSelected);
				text->setTextStatusColor(ZK_CONTROL_STATUS_INVALID, colors->text.foreground.invalid);

				text->setLongMode(ZKTextView::E_LONG_MODE_DOTS);
				continue;
			}
			if (typeid(*control) == typeid(ZKButton))
			{
				ZKButton* button = static_cast<ZKButton*>(control);
				// Background
				button->setBackgroundColor(colors->button.bgDefault);
				button->setBackgroundPic(colors->button.bgImage);
				button->setBgStatusColor(ZK_CONTROL_STATUS_NORMAL, colors->button.background.normal);
				button->setBgStatusColor(ZK_CONTROL_STATUS_PRESSED, colors->button.background.pressed);
				button->setBgStatusColor(ZK_CONTROL_STATUS_SELECTED, colors->button.background.selected);
				button->setBgStatusColor(ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED,
										 colors->button.background.pressedAndSelected);
				button->setBgStatusColor(ZK_CONTROL_STATUS_INVALID, colors->button.background.invalid);

				// Foreground
				button->setTextStatusColor(ZK_CONTROL_STATUS_NORMAL, colors->button.foreground.normal);
				button->setTextStatusColor(ZK_CONTROL_STATUS_PRESSED, colors->button.foreground.pressed);
				button->setTextStatusColor(ZK_CONTROL_STATUS_SELECTED, colors->button.foreground.selected);
				button->setTextStatusColor(ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED,
										   colors->button.foreground.pressedAndSelected);
				button->setTextStatusColor(ZK_CONTROL_STATUS_INVALID, colors->button.foreground.invalid);

				// Pictures
				button->setButtonStatusPic(ZK_CONTROL_STATUS_NORMAL, colors->button.images.normal);
				button->setButtonStatusPic(ZK_CONTROL_STATUS_PRESSED, colors->button.images.pressed);
				button->setButtonStatusPic(ZK_CONTROL_STATUS_SELECTED, colors->button.images.selected);
				button->setButtonStatusPic(ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED,
										   colors->button.images.pressedAndSelected);
				button->setButtonStatusPic(ZK_CONTROL_STATUS_INVALID, colors->button.images.invalid);

				button->setLongMode(ZKTextView::E_LONG_MODE_DOTS);
				continue;
			}
			if (typeid(*control) == typeid(ZKEditText))
			{
				ZKEditText* text = static_cast<ZKEditText*>(control);
				// Background
				text->setBackgroundColor(colors->input.bgDefault);
				text->setBackgroundPic(colors->input.bgImage);
				text->setBgStatusColor(ZK_CONTROL_STATUS_NORMAL, colors->input.background.normal);
				text->setBgStatusColor(ZK_CONTROL_STATUS_PRESSED, colors->input.background.pressed);
				text->setBgStatusColor(ZK_CONTROL_STATUS_SELECTED, colors->input.background.selected);
				text->setBgStatusColor(ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED,
									   colors->input.background.pressedAndSelected);
				text->setBgStatusColor(ZK_CONTROL_STATUS_INVALID, colors->input.background.invalid);

				// Foreground
				text->setTextStatusColor(ZK_CONTROL_STATUS_NORMAL, colors->input.foreground.normal);
				text->setTextStatusColor(ZK_CONTROL_STATUS_PRESSED, colors->input.foreground.pressed);
				text->setTextStatusColor(ZK_CONTROL_STATUS_SELECTED, colors->input.foreground.selected);
				text->setTextStatusColor(ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED,
										 colors->input.foreground.pressedAndSelected);
				text->setTextStatusColor(ZK_CONTROL_STATUS_INVALID, colors->input.foreground.invalid);

				continue;
			}
			if (typeid(*control) == typeid(ZKSeekBar))
			{
				ZKSeekBar* seek = static_cast<ZKSeekBar*>(control);
				// Background
				seek->setBackgroundColor(colors->slider.bgDefault);
				seek->setBackgroundPic(colors->slider.bgImage);

				// Foreground
				seek->setProgressPic(colors->slider.validImage);
				seek->setThumbPic(ZK_CONTROL_STATUS_NORMAL, colors->slider.thumb.normal);
				seek->setThumbPic(ZK_CONTROL_STATUS_PRESSED, colors->slider.thumb.pressed);
				continue;
			}
			if (typeid(*control) == typeid(ZKCircleBar))
			{
				ZKCircleBar* circleBar = static_cast<ZKCircleBar*>(control);
				// Background
				circleBar->setBackgroundColor(colors->circularBar.bgDefault);
				circleBar->setBackgroundPic(colors->circularBar.bgImage);

				// Foreground
				circleBar->setTextColor(colors->circularBar.text);
				circleBar->setProgressPic(colors->circularBar.validImage);
				circleBar->setThumbPic(ZK_CONTROL_STATUS_NORMAL, colors->circularBar.thumb.normal);
				circleBar->setThumbPic(ZK_CONTROL_STATUS_PRESSED, colors->circularBar.thumb.pressed);
				continue;
			}
			if (typeid(*control) == typeid(ZKPointer))
			{
				ZKPointer* pointer = static_cast<ZKPointer*>(control);
				// Background
				pointer->setBackgroundColor(colors->pointer.bgDefault);
				pointer->setBackgroundPic(colors->pointer.bgImage);

				// Foreground
				// TODO Work out how to set pointer picture
				continue;
			}
			if (typeid(*control) == typeid(ZKDigitalClock))
			{
				ZKDigitalClock* digitalClock = static_cast<ZKDigitalClock*>(control);
				// Background
				digitalClock->setBackgroundColor(colors->digitalClock.bgDefault);
				digitalClock->setBackgroundPic(colors->digitalClock.bgImage);

				// Foreground
				digitalClock->setTextColor(colors->digitalClock.text); // TODO This doesn't work
				continue;
			}
			if (typeid(*control) == typeid(ZKCheckBox))
			{
				ZKCheckBox* checkbox = static_cast<ZKCheckBox*>(control);
				// Background
				checkbox->setBackgroundColor(colors->checkbox.bgDefault);
				checkbox->setBackgroundPic(colors->checkbox.bgImage);
				checkbox->setBgStatusColor(ZK_CONTROL_STATUS_NORMAL, colors->checkbox.background.normal);
				checkbox->setBgStatusColor(ZK_CONTROL_STATUS_PRESSED, colors->checkbox.background.pressed);
				checkbox->setBgStatusColor(ZK_CONTROL_STATUS_SELECTED, colors->checkbox.background.selected);
				checkbox->setBgStatusColor(ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED,
										   colors->checkbox.background.pressedAndSelected);
				checkbox->setBgStatusColor(ZK_CONTROL_STATUS_INVALID, colors->checkbox.background.invalid);

				// Foreground
				checkbox->setTextStatusColor(ZK_CONTROL_STATUS_NORMAL, colors->checkbox.foreground.normal);
				checkbox->setTextStatusColor(ZK_CONTROL_STATUS_PRESSED, colors->checkbox.foreground.pressed);
				checkbox->setTextStatusColor(ZK_CONTROL_STATUS_SELECTED, colors->checkbox.foreground.selected);
				checkbox->setTextStatusColor(ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED,
											 colors->checkbox.foreground.pressedAndSelected);
				checkbox->setTextStatusColor(ZK_CONTROL_STATUS_INVALID, colors->checkbox.foreground.invalid);

				// Pictures
				checkbox->setButtonStatusPic(ZK_CONTROL_STATUS_NORMAL, colors->checkbox.images.normal);
				checkbox->setButtonStatusPic(ZK_CONTROL_STATUS_PRESSED, colors->checkbox.images.pressed);
				checkbox->setButtonStatusPic(ZK_CONTROL_STATUS_SELECTED, colors->checkbox.images.selected);
				checkbox->setButtonStatusPic(ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED,
											 colors->checkbox.images.pressedAndSelected);
				checkbox->setButtonStatusPic(ZK_CONTROL_STATUS_INVALID, colors->checkbox.images.invalid);

				checkbox->setLongMode(ZKTextView::E_LONG_MODE_DOTS);
				continue;
			}
			if (typeid(*control) == typeid(ZKListView))
			{
				ZKListView* list = static_cast<ZKListView*>(control);
				// Background
				list->setBackgroundColor(colors->list.bgDefault);
				list->setBackgroundPic(colors->list.bgImage);

				list->setBgStatusColor(ZK_CONTROL_STATUS_NORMAL, colors->list.background.normal);
				list->setBgStatusColor(ZK_CONTROL_STATUS_PRESSED, colors->list.background.pressed);
				list->setBgStatusColor(ZK_CONTROL_STATUS_SELECTED, colors->list.background.selected);
				list->setBgStatusColor(ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED,
									   colors->list.background.pressedAndSelected);
				list->setBgStatusColor(ZK_CONTROL_STATUS_INVALID, colors->list.background.invalid);
				continue;
			}
			if (typeid(*control) == typeid(ZKDiagram))
			{
				ZKDiagram* diagram = static_cast<ZKDiagram*>(control);
				// Background
				diagram->setBackgroundColor(colors->diagram.bgDefault);
				diagram->setBackgroundPic(colors->diagram.bgImage);

				for (size_t i = 0; i < sizeof(colors->diagram.colors) / sizeof(colors->diagram.colors[0]); i++)
				{
					diagram->setPenColor(i, colors->diagram.colors[i]);
				}
				continue;
			}
			if (typeid(*control) == typeid(ZKSlideWindow))
			{
				ZKSlideWindow* slideWindow = static_cast<ZKSlideWindow*>(control);
				slideWindow->setBackgroundColor(colors->slideWindow.bgDefault);
				slideWindow->setBackgroundPic(colors->slideWindow.bgImage);
				continue;
			}
		}
	}

	void SetTheme(const std::string& id)
	{
		if (s_themes.find(id) == s_themes.end())
		{
			std::string validThemes = "";
			bool first = true;
			for (auto theme : s_themes)
			{
				if (!first)
					validThemes += ", ";

				validThemes += theme.first;
				first = false;
			}
			warn("Theme %s not found. Valid themes are: %s", id.c_str(), validThemes.c_str());
			return;
		}
		SetTheme(s_themes[id]);
	}

	void SetTheme(Theme* theme)
	{
		if (theme == nullptr)
			return;

		s_currentTheme = theme;
		StoragePreferences::putString(ID_THEME, s_currentTheme->id);
		info("Set theme to \"%s\"", s_currentTheme->id.c_str());

		s_themedListItems.clear();
		ZKWindow* window = UI::GetRootWindow();
		std::vector<ZKBase*> children;
		window->getAllControls(children);
		ThemeControls(s_currentTheme->colors, children);
		s_currentTheme->overrides();
	}

	void ThemeListItem(ZKListView* pListView, ZKListView::ZKListItem* pListItem, int index)
	{
		if (s_currentTheme == nullptr)
		{
			warn("No theme set");
			return;
		}

		if (pListView == nullptr)
		{
			warn("List view is null");
			return;
		}

		if (pListItem == nullptr)
		{
			warn("List item is null");
			return;
		}

		// For this list we want to apply different colours to each item
		if (pListView->getID() == ID_MAIN_TemperatureGraphLegend)
		{
			goto applyTheme;
		}

		// Some lists are themed elsewhere
		switch (pListView->getID())
		{
		case ID_MAIN_ObjectCancelObjectsList:
			return;
		default:
			break;
		}

		if (s_themedListItems.find(pListItem) == s_themedListItems.end())
		{
			s_themedListItems[pListItem] = s_currentTheme;
		}
		else if (s_themedListItems[pListItem] != s_currentTheme)
		{
			s_themedListItems[pListItem] = s_currentTheme;
		}
		else
		{
			verbose("Already themed list item %p", pListItem);
			return;
		}

	applyTheme:
		ThemeColors* colors = s_currentTheme->colors;
		dbg("Applying %s theme to list item %p", s_currentTheme->id.c_str(), pListItem);

		pListItem->setBackgroundColor(colors->listItem.bgDefault);
		pListItem->setBackgroundPic(colors->listItem.bgImage);
		pListItem->setBgStatusColor(ZK_CONTROL_STATUS_NORMAL, colors->listItem.background.normal);
		pListItem->setBgStatusColor(ZK_CONTROL_STATUS_PRESSED, colors->listItem.background.pressed);
		pListItem->setBgStatusColor(ZK_CONTROL_STATUS_SELECTED, colors->listItem.background.selected);
		pListItem->setBgStatusColor(ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED,
									colors->listItem.background.pressedAndSelected);
		pListItem->setBgStatusColor(ZK_CONTROL_STATUS_INVALID, colors->listItem.background.invalid);

		// Foreground
		pListItem->setTextStatusColor(ZK_CONTROL_STATUS_NORMAL, colors->listItem.foreground.normal);
		pListItem->setTextStatusColor(ZK_CONTROL_STATUS_PRESSED, colors->listItem.foreground.pressed);
		pListItem->setTextStatusColor(ZK_CONTROL_STATUS_SELECTED, colors->listItem.foreground.selected);
		pListItem->setTextStatusColor(ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED,
									  colors->listItem.foreground.pressedAndSelected);
		pListItem->setTextStatusColor(ZK_CONTROL_STATUS_INVALID, colors->listItem.foreground.invalid);

		// Pictures
		pListItem->setButtonStatusPic(ZK_CONTROL_STATUS_NORMAL, colors->listItem.images.normal);
		pListItem->setButtonStatusPic(ZK_CONTROL_STATUS_PRESSED, colors->listItem.images.pressed);
		pListItem->setButtonStatusPic(ZK_CONTROL_STATUS_SELECTED, colors->listItem.images.selected);
		pListItem->setButtonStatusPic(ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED,
									  colors->listItem.images.pressedAndSelected);
		pListItem->setButtonStatusPic(ZK_CONTROL_STATUS_INVALID, colors->listItem.images.invalid);

		// pListItem->setLongMode(ZKTextView::E_LONG_MODE_DOTS);

		for (int i = 0; i < pListItem->getSubItemCount(); i++)
		{
			ZKListView::ZKListSubItem* pSubItem = pListItem->getSubItem(i);
			if (pSubItem == nullptr)
			{
				warn("Sub item is null");
				continue;
			}
			pSubItem->setBackgroundColor(colors->listSubItem.bgDefault);
			pSubItem->setBackgroundPic(colors->listSubItem.bgImage);
			pSubItem->setBgStatusColor(ZK_CONTROL_STATUS_NORMAL, colors->listSubItem.background.normal);
			pSubItem->setBgStatusColor(ZK_CONTROL_STATUS_PRESSED, colors->listSubItem.background.pressed);
			pSubItem->setBgStatusColor(ZK_CONTROL_STATUS_SELECTED, colors->listSubItem.background.selected);
			pSubItem->setBgStatusColor(ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED,
									   colors->listSubItem.background.pressedAndSelected);
			pSubItem->setBgStatusColor(ZK_CONTROL_STATUS_INVALID, colors->listSubItem.background.invalid);

			// Foreground
			pSubItem->setTextStatusColor(ZK_CONTROL_STATUS_NORMAL, colors->listSubItem.foreground.normal);
			pSubItem->setTextStatusColor(ZK_CONTROL_STATUS_PRESSED, colors->listSubItem.foreground.pressed);
			pSubItem->setTextStatusColor(ZK_CONTROL_STATUS_SELECTED, colors->listSubItem.foreground.selected);
			pSubItem->setTextStatusColor(ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED,
										 colors->listSubItem.foreground.pressedAndSelected);
			pSubItem->setTextStatusColor(ZK_CONTROL_STATUS_INVALID, colors->listSubItem.foreground.invalid);

			// Pictures
			pSubItem->setButtonStatusPic(ZK_CONTROL_STATUS_NORMAL, colors->listSubItem.images.normal);
			pSubItem->setButtonStatusPic(ZK_CONTROL_STATUS_PRESSED, colors->listSubItem.images.pressed);
			pSubItem->setButtonStatusPic(ZK_CONTROL_STATUS_SELECTED, colors->listSubItem.images.selected);
			pSubItem->setButtonStatusPic(ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED,
										 colors->listSubItem.images.pressedAndSelected);
			pSubItem->setButtonStatusPic(ZK_CONTROL_STATUS_INVALID, colors->listSubItem.images.invalid);

			// pSubItem->setLongMode(ZKTextView::E_LONG_MODE_DOTS);
		}

		// Lists used as graph axes
		switch (pListView->getID())
		{
		case ID_MAIN_TempGraphXLabels:
		case ID_MAIN_TempGraphYLabels:
		case ID_MAIN_HeightMapXAxis:
		case ID_MAIN_HeightMapYAxis:
		case ID_MAIN_HeightMapScaleList:
		case ID_MAIN_ObjectCancelXAxis:
		case ID_MAIN_ObjectCancelYAxis:
			pListView->setBackgroundColor(NULL);
			pListItem->setBgStatusColor(ZK_CONTROL_STATUS_NORMAL, NULL);
			pListItem->setBackgroundColor(NULL);
			break;
		default:
			break;
		}

		// Temperature graph legend
		if (pListView->getID() == ID_MAIN_TemperatureGraphLegend)
		{
			if (index >= (int)(sizeof(colors->diagram.colors) / sizeof(colors->diagram.colors[0])))
			{
				pListItem->setBgStatusColor(ZK_CONTROL_STATUS_NORMAL, colors->diagram.bgDefault);
			}
			pListItem->setBgStatusColor(ZK_CONTROL_STATUS_NORMAL, colors->diagram.colors[index]);
		}

		// Overrides
		s_currentTheme->listItemOverrides(pListView, pListItem, index);
	}
} // namespace UI::Theme
