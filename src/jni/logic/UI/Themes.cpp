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
	static std::map<const char*, ThemeColors*> s_themes;
	static std::string s_currentTheme;

	void CreateTheme(const char* id, const ThemeColors* theme)
	{
		if (s_themes.find(id) != s_themes.end())
		{
			warn("Theme %s already exists", id);
			return;
		}
		s_themes[id] = theme;
	}

	static void ThemeControls(ThemeColors* theme, std::vector<ZKBase*> controls)
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
				dbg("Window");
				ZKWindow* window = static_cast<ZKWindow*>(control);
				window->setBackgroundColor(theme->text.base);
				// std::vector<ZKBase*> children;
				// window->getAllControls(children);
				// ThemeControls(theme, children);
				continue;
			}
			if (typeid(*control) == typeid(ZKTextView))
			{
				ZKTextView* text = static_cast<ZKTextView*>(control);
				dbg("Text control");
				// Background
				text->setBackgroundColor(theme->text.base);
				text->setBgStatusColor(ZK_CONTROL_STATUS_NORMAL, theme->text.background.normal);
				text->setBgStatusColor(ZK_CONTROL_STATUS_PRESSED, theme->text.background.pressed);
				text->setBgStatusColor(ZK_CONTROL_STATUS_SELECTED, theme->text.background.selected);
				text->setBgStatusColor(ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED,
									   theme->text.background.pressedAndSelected);
				text->setBgStatusColor(ZK_CONTROL_STATUS_INVALID, theme->text.background.invalid);

				// Foreground
				text->setTextStatusColor(ZK_CONTROL_STATUS_NORMAL, theme->text.foreground.normal);
				text->setTextStatusColor(ZK_CONTROL_STATUS_PRESSED, theme->text.foreground.pressed);
				text->setTextStatusColor(ZK_CONTROL_STATUS_SELECTED, theme->text.foreground.selected);
				text->setTextStatusColor(ZK_CONTROL_STATUS_PRESSED | ZK_CONTROL_STATUS_SELECTED,
										 theme->text.foreground.pressedAndSelected);
				text->setTextStatusColor(ZK_CONTROL_STATUS_INVALID, theme->text.foreground.invalid);
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
		ThemeColors* theme = s_themes[id];

		ZKWindow* window = UI::GetRootWindow();
		std::vector<ZKBase*> children;
		window->getAllControls(children);
		ThemeControls(theme, children);
	}

} // namespace UI::Theme
