/*
 * Themes.h
 *
 *  Created on: 2 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_UI_COLORS_HPP_
#define JNI_LOGIC_UI_COLORS_HPP_

#include "UI/UserInterface.h"
#include "std_fixed/functional.h"
#include <sys/types.h>

namespace UI::Theme
{
	struct ControlState
	{
		uint32_t normal;
		uint32_t pressed;
		uint32_t selected;
		uint32_t pressedAndSelected;
		uint32_t invalid;
	};

	struct Images
	{
		const char* normal;
		const char* pressed;
		const char* selected;
		const char* pressedAndSelected;
		const char* invalid;
	};

	struct ThumbImage
	{
		const char* normal;
		const char* pressed;
	};

	struct ThemeColors
	{
		struct
		{
			uint32_t bgDefault;
			const char* bgImage;
		} window;
		struct
		{
			uint32_t bgDefault;
			const char* bgImage;
			ControlState background;
			ControlState foreground;
		} text;
		struct
		{
			uint32_t bgDefault;
			const char* bgImage;
			ControlState background;
			ControlState foreground;
			Images images;
		} button;
		struct
		{
			uint32_t bgDefault;
			const char* bgImage;
			ControlState background;
			ControlState foreground;
			uint32_t hint; //* This isn't used
		} input;
		struct
		{
			uint32_t bgDefault;
			const char* bgImage;
			const char* validImage;
			ThumbImage thumb;
		} slider;
		struct
		{
			uint32_t bgDefault;
			const char* bgImage;
			const char* validImage;
			uint32_t text;
			ThumbImage thumb;
		} circularBar;
		struct
		{
			uint32_t bgDefault;
			const char* bgImage;
			const char* pointerImage; //* This isn't used
		} pointer;
		struct
		{
			uint32_t bgDefault;
			const char* bgImage;
			uint32_t text; // TODO api doesn't work?
		} digitalClock;
		struct
		{
			uint32_t bgDefault;
			const char* bgImage;
			ControlState background;
			ControlState foreground;
			Images images;
		} checkbox;
		struct
		{
			uint32_t bgDefault;
			const char* bgImage;
			ControlState background;
		} list;
		struct
		{
			uint32_t bgDefault;
			const char* bgImage;
			ControlState background;
			ControlState foreground;
			Images images;
		} listItem;
		struct
		{
			uint32_t bgDefault;
			const char* bgImage;
			ControlState background;
			ControlState foreground;
			Images images;
		} listSubItem;
		struct
		{
			uint32_t bgDefault;
			const char* bgImage;
		} slideWindow;
		struct
		{
			ControlState foreground;
			Images images;
		} slideWindowItem;
	};

	class Theme
	{
	  public:
		Theme(const char* id, ThemeColors* colors, function<void(void)> overrides);

		const char* id;
		ThemeColors* colors;
		function<void(void)> overrides;
	};

	void CreateTheme(const char* id, Theme* theme);
	void SetTheme(const char* id);
} // namespace UI

#endif /* JNI_LOGIC_UI_COLORS_HPP_ */
