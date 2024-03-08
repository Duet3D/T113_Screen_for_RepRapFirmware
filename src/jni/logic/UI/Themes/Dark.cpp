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
		Red = 0xFFFF0000,
		Blue = 0xFF0000FF,
		Blue2 = 0xFF5578FC,
		Blue3 = 0xFF00AAF4,
		Blue4 = 0xFF00CCFF,
	};

	static ThemeColors darkTheme = {
		// .id = "dark",
		// Set colors for window
		.window =
			{
				.bgDefault = Colors::Black,
				.bgImage = nullptr,
			},
		.text =
			{
				.bgDefault = Colors::Clear,
				.bgImage = nullptr,
				.background =
					{
						.normal = Colors::Clear,
						.pressed = Colors::Clear,
						.selected = Colors::Clear,
						.pressedAndSelected = Colors::Clear,
						.invalid = Colors::Clear,
					},
				.foreground =
					{
						.normal = Colors::White,
						.pressed = Colors::LightGray,
						.selected = Colors::LighterGray,
						.pressedAndSelected = Colors::LightGray,
						.invalid = Colors::Gray,
					},
			},
		.button =
			{
				.bgDefault = Colors::Blue2,
				.bgImage = nullptr,
				.background =
					{
						.normal = Colors::Blue2,
						.pressed = Colors::Blue3,
						.selected = Colors::Blue4,
						.pressedAndSelected = Colors::Blue3,
						.invalid = Colors::Gray,
					},
				.foreground =
					{
						.normal = Colors::White,
						.pressed = Colors::White,
						.selected = Colors::White,
						.pressedAndSelected = Colors::White,
						.invalid = Colors::White,
					},
				.images =
					{
						.normal = nullptr,
						.pressed = nullptr,
						.selected = nullptr,
						.pressedAndSelected = nullptr,
						.invalid = nullptr,
					},
			},
		.input =
			{
				.bgDefault = Colors::White,
				.bgImage = nullptr,
				.background =
					{
						.normal = Colors::White,
						.pressed = Colors::White,
						.selected = Colors::White,
						.pressedAndSelected = Colors::White,
						.invalid = Colors::White,
					},
				.foreground =
					{
						.normal = Colors::Black,
						.pressed = Colors::Black,
						.selected = Colors::Black,
						.pressedAndSelected = Colors::Black,
						.invalid = Colors::Black,
					},
				.hint = Colors::LighterGray,
			},
		.slider =
			{
				.bgDefault = Colors::Clear,
				.bgImage = nullptr,
				.validImage = nullptr,
				.thumb =
					{
						.normal = nullptr,
						.pressed = nullptr,
					},
			},
		.circularBar =
			{
				.bgDefault = Colors::Clear,
				.bgImage = nullptr,
				.validImage = nullptr,
				.text = Colors::White,
				.thumb =
					{
						.normal = nullptr,
						.pressed = nullptr,
					},
			},
		.pointer =
			{
				.bgDefault = Colors::Clear,
				.bgImage = nullptr,
			},
		.digitalClock =
			{
				.bgDefault = Colors::Clear,
				.bgImage = nullptr,
				.text = Colors::White,
			},
		.checkbox =
			{
				.bgDefault = Colors::Clear,
				.bgImage = nullptr,
				.background =
					{
						.normal = Colors::Clear,
						.pressed = Colors::Clear,
						.selected = Colors::Clear,
						.pressedAndSelected = Colors::Clear,
						.invalid = Colors::Clear,
					},
				.foreground =
					{
						.normal = Colors::White,
						.pressed = Colors::White,
						.selected = Colors::White,
						.pressedAndSelected = Colors::White,
						.invalid = Colors::White,
					},
				.images =
					{
						.normal = nullptr,
						.pressed = nullptr,
						.selected = nullptr,
						.pressedAndSelected = nullptr,
						.invalid = nullptr,
					},
			},
		.list =
			{
				.bgDefault = Colors::Clear,
				.bgImage = nullptr,
			},
		.listItem =
			{
				.bgDefault = Colors::DarkGray,
				.bgImage = nullptr,
				.background =
					{
						.normal = Colors::LightGray,
						.pressed = Colors::Blue3,
						.selected = Colors::Blue4,
						.pressedAndSelected = Colors::Blue3,
						.invalid = Colors::Gray,
					},
				.foreground =
					{
						.normal = Colors::White,
						.pressed = Colors::White,
						.selected = Colors::White,
						.pressedAndSelected = Colors::White,
						.invalid = Colors::White,
					},
				.images =
					{
						.normal = nullptr,
						.pressed = nullptr,
						.selected = nullptr,
						.pressedAndSelected = nullptr,
						.invalid = nullptr,
					},
			},
		.listSubItem =
			{
				.bgDefault = Colors::Clear,
				.bgImage = nullptr,
				.background =
					{
						.normal = Colors::Clear,
						.pressed = Colors::Clear,
						.selected = Colors::Clear,
						.pressedAndSelected = Colors::Clear,
						.invalid = Colors::Clear,
					},
				.foreground =
					{
						.normal = Colors::White,
						.pressed = Colors::White,
						.selected = Colors::White,
						.pressedAndSelected = Colors::White,
						.invalid = Colors::White,
					},
				.images =
					{
						.normal = nullptr,
						.pressed = nullptr,
						.selected = nullptr,
						.pressedAndSelected = nullptr,
						.invalid = nullptr,
					},
			},
		.slideWindow =
			{
				.bgDefault = Colors::Clear,
				.bgImage = nullptr,
			},
		.slideWindowItem =
			{
				.foreground =
					{
						.normal = Colors::White,
						.pressed = Colors::White,
						.selected = Colors::White,
						.pressedAndSelected = Colors::White,
						.invalid = Colors::White,
					},
				.images =
					{
						.normal = nullptr,
						.pressed = nullptr,
						.selected = nullptr,
						.pressedAndSelected = nullptr,
						.invalid = nullptr,
					},
			}
		/*
				// Set colors for button

				// Set colors for slider
				.slider.base = Colors::DarkGray, // Dark gray
				*/
	};

	static Theme theme("dark", &darkTheme);
} // namespace UI::Theme