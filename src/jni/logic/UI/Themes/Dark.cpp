#pragma once

#include "UI/Themes.h"

namespace UI::Theme
{
	enum Colors : uint32_t
	{
		Clear = 0x00000000,
		Black = 0xFF000000,
		White = 0xFFFFFFFF,
		LightGray = 0xFFE0E0E0,
		LighterGray = 0xFF808080,
		Gray = 0xFFC0C0C0,
		DarkGray = 0xFFA0A0A0,
		DarkerGray = 0xFF808080,
		Red = 0xFFFF0000,
		Blue = 0xFF0000FF,
		Button
	};

	static ThemeColors darkTheme("dark");

	// Set colors for window
	darkTheme.window.bgDefault = Colors::Black;
	darkTheme.window.bgImage = nullptr;

	// Set colors for text
	darkTheme.text.bgDefault = Colors::Clear;
	darkTheme.text.bgImage = nullptr;
	darkTheme.text.background.normal = Colors::Clear;
	darkTheme.text.background.pressed = Colors::Clear;
	darkTheme.text.background.selected = Colors::Clear;
	darkTheme.text.background.pressedAndSelected = Colors::Clear;
	darkTheme.text.background.invalid = Colors::Clear;
	darkTheme.text.foreground.normal = Colors::White;
	darkTheme.text.foreground.pressed = Colors::LightGray;
	darkTheme.text.foreground.selected = Colors::LighterGray;
	darkTheme.text.foreground.pressedAndSelected = Colors::LightGray;
	darkTheme.text.foreground.invalid = Colors::Gray;

	// Set colors for button
	darkTheme.button.bgDefault = Colors::Clear;
	darkTheme.text.bgImage = nullptr;
	darkTheme.button.background.normal = Colors::Clear;
	darkTheme.button.background.pressed = Colors::Clear;
	darkTheme.button.background.selected = Colors::Clear;
	darkTheme.button.background.pressedAndSelected = Colors::Clear;
	darkTheme.button.background.invalid = Colors::Clear;
	darkTheme.button.foreground.normal = Colors::White;
	darkTheme.button.foreground.pressed = Colors::LightGray;
	darkTheme.button.foreground.selected = Colors::LighterGray;
	darkTheme.button.foreground.pressedAndSelected = Colors::LightGray;
	darkTheme.button.foreground.invalid = Colors::Gray;

	// Set colors for input
	darkTheme.input.bgDefault = Colors::Clear;
	darkTheme.text.bgImage = nullptr;
	darkTheme.input.background.normal = Colors::DarkGray;
	darkTheme.input.background.pressed = Colors::Gray;
	darkTheme.input.background.selected = Colors::LightGray;
	darkTheme.input.background.pressedAndSelected = Colors::LighterGray;
	darkTheme.input.background.invalid = Colors::Red;
	darkTheme.input.hint = Colors::LighterGray;

	// Set colors for slider
	darkTheme.slider.base = Colors::DarkGray; // Dark gray
} // namespace UI::Theme