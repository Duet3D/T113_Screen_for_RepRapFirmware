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

	static ThemeColors darkTheme = {
		// .id = "dark",
		// Set colors for window
		.window =
			{
				.bgDefault = Colors::Black,
				.bgImage = nullptr,
			},

		/*
				// Set colors for text
				.text.bgDefault = Colors::Clear,
				.text.bgImage = nullptr,
				.text.background.normal = Colors::Clear,
				.text.background.pressed = Colors::Clear,
				.text.background.selected = Colors::Clear,
				.text.background.pressedAndSelected = Colors::Clear,
				.text.background.invalid = Colors::Clear,
				.text.foreground.normal = Colors::White,
				.text.foreground.pressed = Colors::LightGray,
				.text.foreground.selected = Colors::LighterGray,
				.text.foreground.pressedAndSelected = Colors::LightGray,
				.text.foreground.invalid = Colors::Gray,

				// Set colors for button
				.button.bgDefault = Colors::Clear,
				.text.bgImage = nullptr,
				.button.background.normal = Colors::Clear,
				.button.background.pressed = Colors::Clear,
				.button.background.selected = Colors::Clear,
				.button.background.pressedAndSelected = Colors::Clear,
				.button.background.invalid = Colors::Clear,
				.button.foreground.normal = Colors::White,
				.button.foreground.pressed = Colors::LightGray,
				.button.foreground.selected = Colors::LighterGray,
				.button.foreground.pressedAndSelected = Colors::LightGray,
				.button.foreground.invalid = Colors::Gray,

				// Set colors for input
				.input.bgDefault = Colors::Clear,
				.text.bgImage = nullptr,
				.input.background.normal = Colors::DarkGray,
				.input.background.pressed = Colors::Gray,
				.input.background.selected = Colors::LightGray,
				.input.background.pressedAndSelected = Colors::LighterGray,
				.input.background.invalid = Colors::Red,
				.input.hint = Colors::LighterGray,

				// Set colors for slider
				.slider.base = Colors::DarkGray, // Dark gray
				*/
	};

	static Theme theme("dark", &darkTheme);
} // namespace UI::Theme