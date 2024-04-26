/*
 * Themes.h
 *
 *  Created on: 2 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_UI_COLORS_HPP_
#define JNI_UI_COLORS_HPP_

#include "Configuration.h"
#include "UI/UserInterface.h"
#include "std_fixed/functional.h"
#include <sys/types.h>

namespace UI::Theme
{
	struct ControlState
	{
		uint32_t normal;			 // Default colour (applied to other states if they are not set)
		uint32_t pressed;			 // Colour when element is pressed
		uint32_t selected;			 // Colour when element is selected
		uint32_t pressedAndSelected; // Colour when element is pressed and selected
		uint32_t invalid;			 // Colour when element is invalid
	};

	struct Images
	{
		const char* normal;				// Default image (applied to other states if they are not set)
		const char* pressed;			// Image when element is pressed
		const char* selected;			// Image when element is selected
		const char* pressedAndSelected; // Image when element is pressed and selected
		const char* invalid;			// Image when element is invalid
	};

	struct ThumbImage
	{
		const char* normal;	 // Default image (applied to other states if they are not set)
		const char* pressed; // Image when element is pressed
	};

	struct ThemeColors
	{
		struct
		{
			uint32_t bgDefault;	 // Background colour
			const char* bgImage; // Background image
		} window;
		struct
		{
			uint32_t bgDefault;		 // Default background colour
			const char* bgImage;	 // Default background image
			ControlState background; // Background colour states (overrides bgDefault)
			ControlState foreground; // Text colour states
		} text;
		struct
		{
			uint32_t bgDefault;		 // Default background colour
			const char* bgImage;	 // Default background image
			ControlState background; // Background colour states (overrides bgDefault)
			ControlState foreground; // Text colour states
			Images images;			 // Image states (applied on top of bgImage)
		} button;
		struct
		{
			uint32_t bgDefault;		 // Default background colour
			const char* bgImage;	 // Default background image
			ControlState background; // Background colour states (overrides bgDefault)
			ControlState foreground; // Text colour states
			uint32_t hint;			 //! Hint colour (not used)
		} input;
		struct
		{
			uint32_t bgDefault;		// Default background colour
			const char* bgImage;	// Default background image
			const char* validImage; // Image when element is valid
			ThumbImage thumb;		// Thumb image states
		} slider;
		struct
		{
			uint32_t bgDefault;		// Default background colour
			const char* bgImage;	// Default background image
			const char* validImage; // Image when element is valid
			uint32_t text;			// Text colour
			ThumbImage thumb;		// Thumb image states
		} circularBar;
		struct
		{
			uint32_t bgDefault;		  // Default background colour
			const char* bgImage;	  // Default background image
			const char* pointerImage; //! Pointer image (not used)
		} pointer;
		struct
		{
			uint32_t bgDefault;	 // Default background colour
			const char* bgImage; // Default background image
			uint32_t text;		 //! Text colour (api doesn't work?)
		} digitalClock;
		struct
		{
			uint32_t bgDefault;		 // Default background colour
			const char* bgImage;	 // Default background image
			ControlState background; // Background colour states (overrides bgDefault)
			ControlState foreground; // Text colour states
			Images images;			 // Image states (applied on top of bgImage)
		} checkbox;
		struct
		{
			uint32_t bgDefault;		 // Default background colour
			const char* bgImage;	 // Default background image
			ControlState background; // Background colour states (overrides bgDefault)
		} list;
		struct
		{
			uint32_t bgDefault;		 // Default background colour
			const char* bgImage;	 // Default background image
			ControlState background; // Background colour states (overrides bgDefault)
			ControlState foreground; // Text colour states
			Images images;			 // Image states (applied on top of bgImage)
		} listItem;
		struct
		{
			uint32_t bgDefault;		 // Default background colour
			const char* bgImage;	 // Default background image
			ControlState background; // Background colour states (overrides bgDefault)
			ControlState foreground; // Text colour states
			Images images;			 // Image states (applied on top of bgImage)
		} listSubItem;
		struct
		{
			uint32_t bgDefault;	 // Default background colour
			const char* bgImage; // Default background image
		} slideWindow;
		struct
		{
			uint32_t bgDefault;			  // Default background colour
			const char* bgImage;		  // Default background image
			uint32_t colors[MAX_SENSORS]; // Array of colors for each waveform
		} diagram;
		struct
		{
			uint32_t bgDefault; // Colour of no data
			uint32_t gridColor; // Colour of the grid
		} heightmap;			// Heightmap canvas
		struct
		{
			uint32_t bgDefault;	  // Default background colour
			uint32_t bgCancelled; // Color when the object is cancelled
			uint32_t bgCurrent;	  // Color for the current object
			uint32_t bgBorder;	  // Border color
		} objectCancel;			  // Object cancel canvas

		// Home screen window select images. key must match the window id
		std::map<const char*, Images> homeScreenWindowSelect;

		// Settings screen window select images. key must match the window id
		std::map<const char*, Images> settingsScreenWindowSelect;
	};

	class Theme
	{
	  public:
		Theme(
			const char* id,
			ThemeColors* colors,
			function<void(void)> overrides,
			function<void(ZKListView*, ZKListView::ZKListItem*, int)> listItemOverrides =
				[](ZKListView* pListView, ZKListView::ZKListItem* pListItem, int index) {});

		std::string id;
		ThemeColors* colors;
		function<void(void)> overrides;
		function<void(ZKListView*, ZKListView::ZKListItem*, int)> listItemOverrides;
	};

	void CreateTheme(Theme* theme);
	int GetThemeCount();
	const Theme* const GetCurrentTheme();
	Theme* GetThemeByIndex(int index);
	void SetTheme(const std::string& id);
	void SetTheme(Theme* theme);
	void ThemeListItem(ZKListView* pListView, ZKListView::ZKListItem* pListItem, int index);
} // namespace UI::Theme

#endif /* JNI_UI_COLORS_HPP_ */
