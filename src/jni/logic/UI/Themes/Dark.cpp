#include "UI/Themes.h"

namespace UI::Theme
{
	enum Colors : uint32_t
	{
		Clear = 0x00FFFFFF,
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
				.bgDefault = NULL,
				.bgImage = nullptr,
				.background =
					{
						.normal = NULL,
						.pressed = NULL,
						.selected = NULL,
						.pressedAndSelected = NULL,
						.invalid = NULL,
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
				.bgDefault = NULL,
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
						.pressed = NULL,
						.selected = NULL,
						.pressedAndSelected = NULL,
						.invalid = NULL,
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
						.normal = NULL,
						.pressed = NULL,
						.selected = NULL,
						.pressedAndSelected = NULL,
						.invalid = NULL,
					},
				.foreground =
					{
						.normal = Colors::Black,
						.pressed = NULL,
						.selected = NULL,
						.pressedAndSelected = NULL,
						.invalid = NULL,
					},
				.hint = Colors::LighterGray,
			},
		.slider =
			{
				.bgDefault = NULL,
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
				.bgDefault = NULL,
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
				.bgDefault = NULL,
				.bgImage = nullptr,
			},
		.digitalClock =
			{
				.bgDefault = NULL,
				.bgImage = nullptr,
				.text = Colors::White,
			},
		.checkbox =
			{
				.bgDefault = NULL,
				.bgImage = nullptr,
				.background =
					{
						.normal = Colors::Gray,
						.pressed = Colors::LightGray,
						.selected = Colors::Blue4,
						.pressedAndSelected = Colors::Blue3,
						.invalid = Colors::Clear,
					},
				.foreground =
					{
						.normal = Colors::White,
						.pressed = NULL,
						.selected = NULL,
						.pressedAndSelected = NULL,
						.invalid = NULL,
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
				.bgDefault = NULL,
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
						.pressed = NULL,
						.selected = NULL,
						.pressedAndSelected = NULL,
						.invalid = NULL,
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
				.bgDefault = NULL,
				.bgImage = nullptr,
				.background =
					{
						.normal = NULL,
						.pressed = NULL,
						.selected = NULL,
						.pressedAndSelected = NULL,
						.invalid = NULL,
					},
				.foreground =
					{
						.normal = Colors::White,
						.pressed = NULL,
						.selected = NULL,
						.pressedAndSelected = NULL,
						.invalid = Colors::LighterGray,
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
				.bgDefault = NULL,
				.bgImage = nullptr,
			},
		.slideWindowItem =
			{
				.foreground =
					{
						.normal = Colors::White,
						.pressed = Colors::LightGray,
						.selected = NULL,
						.pressedAndSelected = NULL,
						.invalid = Colors::Gray,
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
	};

	static Theme theme("dark", &darkTheme, []() {
		UI::GetUIControl<ZKWindow>(ID_MAIN_Window1)->setBackgroundColor(Colors::Blue);
	});
} // namespace UI::Theme