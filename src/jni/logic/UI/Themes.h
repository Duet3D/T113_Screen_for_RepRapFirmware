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

	class ThemeColors;

	void Init();
	void CreateTheme(const char* id, const ThemeColors* theme);
	void SetTheme(const char* id);

	class ThemeColors
	{
	  public:
		ThemeColors(const char* id) { CreateTheme(id, this); }
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
			uint32_t hint;
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
			ThumbImage thumb;
		} circularBar;
		struct
		{
			uint32_t bgDefault;
			const char* bgImage;
		} pointer;
		struct
		{
			uint32_t bgDefault;
			const char* bgImage;
			uint32_t text;
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
} // namespace UI

#endif /* JNI_LOGIC_UI_COLORS_HPP_ */
