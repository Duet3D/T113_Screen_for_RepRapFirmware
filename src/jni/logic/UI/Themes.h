/*
 * Themes.h
 *
 *  Created on: 2 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_UI_COLORS_HPP_
#define JNI_LOGIC_UI_COLORS_HPP_

#include "UI/UserInterface.h"
#include <sys/types.h>

namespace UI
{
	enum class Theme
	{
		dark = 0,
		light,
	};

	struct Colors
	{
		uint32_t normal;
		uint32_t pressed;
		uint32_t selected;
		uint32_t pressedAndSelected;
		uint32_t invalid;
	};

	struct ItemColor
	{
		uint32_t base;
		Colors background;
		Colors foreground;
	};

	struct ThemeColors
	{
		struct
		{
			uint32_t base;
			Colors background;
			Colors foreground;
		} text;
		struct
		{
			uint32_t base;
			Colors background;
			Colors foreground;
		} button;
		struct
		{
			uint32_t base;
			Colors background;
			Colors foreground;
			uint32_t hint;
		} input;
		struct
		{
			uint32_t base;
		} slider;
	};

	namespace UI
	{
		ThemeColors createDarkTheme()
		{
			ThemeColors darkTheme;

			// Set colors for text
			darkTheme.text.base = 0xFF000000;						   // Black
			darkTheme.text.background.normal = 0xFF000000;			   // Black
			darkTheme.text.background.pressed = 0xFF202020;			   // Dark gray
			darkTheme.text.background.selected = 0xFF404040;		   // Gray
			darkTheme.text.background.pressedAndSelected = 0xFF606060; // Light gray
			darkTheme.text.background.invalid = 0xFFFF0000;			   // Red

			// Set colors for button
			darkTheme.button.base = 0xFF202020;							 // Dark gray
			darkTheme.button.background.normal = 0xFF202020;			 // Dark gray
			darkTheme.button.background.pressed = 0xFF404040;			 // Gray
			darkTheme.button.background.selected = 0xFF606060;			 // Light gray
			darkTheme.button.background.pressedAndSelected = 0xFF808080; // Lighter gray
			darkTheme.button.background.invalid = 0xFFFF0000;			 // Red

			// Set colors for input
			darkTheme.input.base = 0xFF202020;							// Dark gray
			darkTheme.input.background.normal = 0xFF202020;				// Dark gray
			darkTheme.input.background.pressed = 0xFF404040;			// Gray
			darkTheme.input.background.selected = 0xFF606060;			// Light gray
			darkTheme.input.background.pressedAndSelected = 0xFF808080; // Lighter gray
			darkTheme.input.background.invalid = 0xFFFF0000;			// Red
			darkTheme.input.hint = 0xFF808080;							// Light gray

			// Set colors for slider
			darkTheme.slider.base = 0xFF202020; // Dark gray

			return darkTheme;
		}

		ThemeColors createLightTheme()
		{
			ThemeColors lightTheme;
			return lightTheme;
		}
	} // namespace UI
	ThemeColors themes[] = {
		{
			// Set colors for text
			.text =
				{
					.base = 0xFF000000, // Black
					.background =
						{
							.normal = 0xFFFFFFFF,			  // White
							.pressed = 0xFFE0E0E0,			  // Light gray
							.selected = 0xFFC0C0C0,			  // Gray
							.pressedAndSelected = 0xFFA0A0A0, // Dark gray
							.invalid = 0xFFFF0000,			  // Red
						},
					.foreground =
						{
							.normal = 0xFF000000,			  // Black
							.pressed = 0xFF000000,			  // Black
							.selected = 0xFF000000,			  // Black
							.pressedAndSelected = 0xFF000000, // Black
							.invalid = 0xFFFF0000,			  // Red
						},
				},

			// Set colors for button
			.button =
				{
					.base = 0xFFE0E0E0, // Light gray
					.background =
						{
							.normal = 0xFFE0E0E0,			  // Light gray
							.pressed = 0xFFC0C0C0,			  // Gray
							.selected = 0xFFA0A0A0,			  // Dark gray
							.pressedAndSelected = 0xFF808080, // Darker gray
							.invalid = 0xFFFF0000,			  // Red
						},
				},

			// Set colors for input
			.input =
				{
					.base = 0xFFE0E0E0, // Light gray
					.background =
						{
							.normal = 0xFFE0E0E0,			  // Light gray
							.pressed = 0xFFC0C0C0,			  // Gray
							.selected = 0xFFA0A0A0,			  // Dark gray
							.pressedAndSelected = 0xFF808080, // Darker gray
							.invalid = 0xFFFF0000,			  // Red
						},
					.hint = 0xFF808080, // Dark gray
				},

			// Set colors for slider
			.slider =
				{
					.base = 0xFFE0E0E0, // Light gray
				},
		},
	};
} // namespace UI

#endif /* JNI_LOGIC_UI_COLORS_HPP_ */
