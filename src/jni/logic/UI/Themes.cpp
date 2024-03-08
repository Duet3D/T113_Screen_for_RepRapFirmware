#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "Colors.h"
#include "Themes.h"
#include <map>
#include <string>
#include <typeinfo>

namespace UI::Theme
{
	static std::map<const char*, Theme*> s_themes;
	static std::string s_currentTheme;

	Theme::Theme(const char* id, ThemeColors* colors, function<void(void)> overrides)
		: id(id), colors(colors), overrides(overrides)
	{
		CreateTheme(id, this);
	}

	void CreateTheme(const char* id, Theme* theme)
	{
		if (s_themes.find(id) != s_themes.end())
		{
			warn("Theme %s already exists", id);
			return;
		}
		info("Creating theme %s", id);
		s_themes[id] = theme;
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
		}
	}

	void SetTheme(const char* id)
	{
		if (s_themes.find(id) == s_themes.end())
		{
			warn("Theme %s not found", id);
			return;
		}
		s_currentTheme = id;
		Theme* theme = s_themes[id];

		ZKWindow* window = UI::GetRootWindow();
		std::vector<ZKBase*> children;
		window->getAllControls(children);
		ThemeControls(theme->colors, children);
		theme->overrides();
	}

} // namespace UI::Theme
