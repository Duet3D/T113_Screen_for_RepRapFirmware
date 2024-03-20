#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "UI/Themes.h"

namespace UI::Theme
{
	enum Colors : uint32_t
	{
		Clear = 0x00FFFFFF,
		Black = 0xFF000000,
		White = 0xFFFFFFFF,
		LighterGray = 0xFFE0E0E0,
		LightGray = 0xFFC0C0C0,
		Gray = 0xFFA0A0A0,
		DarkGray = 0xFF808080,
		DarkerGray = 0xFF606060,
		Red = 0xFFFF0000,
		DarkRed = 0xFFFF8080,
		Blue = 0xFF0000FF,
		Blue2 = 0xFF5578FC,
		Blue3 = 0xFF00AAF4,
		Blue4 = 0xFF00CCFF,
		Yellow = 0xFFECBC35,
	};

	static ThemeColors darkTheme = {
		// .id = "dark",
		// Set colors for window
		.window =
			{
				.bgDefault = NULL,
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
				.bgImage = "hSliderBar.png",
				.validImage = nullptr,
				.thumb =
					{
						.normal = "hSliderKnob.png",
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
				.pointerImage = nullptr,
			},
		.digitalClock =
			{
				.bgDefault = Black,
				.bgImage = nullptr,
				.text = Colors::White,
			},
		.checkbox =
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
						.invalid = NULL,
					},
				.images =
					{
						.normal = "toggleOffLarge.png",
						.pressed = nullptr,
						.selected = "toggleOnLarge.png",
						.pressedAndSelected = nullptr,
						.invalid = nullptr,
					},
			},
		.list =
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
			},
		.listItem =
			{
				.bgDefault = Colors::Gray,
				.bgImage = nullptr,
				.background =
					{
						.normal = Colors::DarkGray,
						.pressed = Colors::Blue3,
						.selected = Colors::Blue2,
						.pressedAndSelected = Colors::Blue3,
						.invalid = Colors::DarkerGray,
					},
				.foreground =
					{
						.normal = Colors::White,
						.pressed = NULL,
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
		.listSubItem =
			{
				.bgDefault = NULL,
				.bgImage = nullptr,
				.background =
					{
						.normal = NULL,
						.pressed = Colors::Blue3,
						.selected = Colors::Clear,
						.pressedAndSelected = Colors::Blue3,
						.invalid = Colors::DarkerGray,
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
		.diagram =
			{
				.bgDefault = NULL,
				.bgImage = "GraphBg.png",
				.colors =
					{
						0xFF0000FF, // Blue
						0xFF00FF00, // Green
						0xFFFF0000, // Red
						0xFFFFFF00, // Yellow
						0xFF00FFFF, // Cyan
						0xFFFF00FF, // Magenta
						0xFF800000, // Maroon
						0xFF008000, // Green
						0xFF000080, // Navy
						0xFF808000, // Olive
						0xFF800080, // Purple
						0xFF008080, // Teal
						0xFF808080, // Gray
						0xFFC0C0C0, // Silver
						0xFFFFA500, // Orange
						0xFFA52A2A, // Brown
						0xFF800000, // Maroon
						0xFF008000, // Green
						0xFF000080, // Navy
						0xFF808000, // Olive
						0xFF800080, // Purple
						0xFF008080, // Teal
						0xFF808080, // Gray
						0xFFC0C0C0, // Silver
						0xFFFFA500, // Orange
						0xFFA52A2A, // Brown
						0xFF800000, // Maroon
						0xFF008000, // Green
						0xFF000080, // Navy
						0xFF808000, // Olive
						0xFF800080, // Purple
						0xFF008080, // Teal
					},
			},
	};

	static Theme theme(
		"dark",
		&darkTheme,
		[]() {
			/* Overrides */
			// Windows
			UI::GetRootWindow()->setBackgroundColor(Colors::Black);
			UI::GetUIControl<ZKWindow>(ID_MAIN_Window1)->setBackgroundColor(Colors::Blue);
			UI::GetUIControl<ZKWindow>(ID_MAIN_TemperatureGraphWindow)->setBackgroundColor(Colors::DarkGray);
			UI::GetUIControl<ZKWindow>(ID_MAIN_FilamentLoadUnloadWindow)->setBackgroundColor(Colors::DarkerGray);
			UI::GetUIControl<ZKWindow>(ID_MAIN_DuetCommSettingWindow)->setBackgroundColor(Colors::Blue4);
			UI::GetUIControl<ZKWindow>(ID_MAIN_GuideSelectionWindow)->setBackgroundColor(Colors::Black);
			UI::GetUIControl<ZKWindow>(ID_MAIN_ThemeSelectionWindow)->setBackgroundColor(Colors::Black);
			UI::GetUIControl<ZKWindow>(ID_MAIN_GuidedSetupWindow)->setBackgroundColor(Colors::Black);
			UI::GetUIControl<ZKWindow>(ID_MAIN_ScreensaverSettingWindow)->setBackgroundColor(Colors::DarkerGray);
			UI::GetUIControl<ZKWindow>(ID_MAIN_SliderWindow)->setBackgroundColor(Colors::DarkerGray);
			UI::GetUIControl<ZKWindow>(ID_MAIN_NumPadWindow)->setBackgroundColor(Colors::DarkerGray);
			UI::GetUIControl<ZKWindow>(ID_MAIN_NoTouchWindow)->setBackgroundColor(Colors::Clear);
			UI::GetUIControl<ZKButton>(ID_MAIN_OverlayModalZone)->setBackgroundColor(NULL);
			UI::GetUIControl<ZKButton>(ID_MAIN_OverlayModalZone)->setBgStatusColor(ZK_CONTROL_STATUS_NORMAL, NULL);
			UI::GetUIControl<ZKButton>(ID_MAIN_OverlayModalZone)->setBgStatusColor(ZK_CONTROL_STATUS_PRESSED, NULL);
			UI::GetUIControl<ZKWindow>(ID_MAIN_PopupWindow)->setBackgroundColor(Colors::DarkerGray);
			UI::GetUIControl<ZKWindow>(ID_MAIN_DebugWindow)->setBackgroundColor(Colors::DarkerGray);

			// Scrollable text
			// UI::GetUIControl<ZKTextView>(ID_MAIN_PopupText)->setLongMode(ZKTextView::ELongMode::E_LONG_MODE_SCROLL);

			// EStop Button
			UI::GetUIControl<ZKButton>(ID_MAIN_EStopBtn)->setBgStatusColor(ZK_CONTROL_STATUS_NORMAL, Colors::Red);
			UI::GetUIControl<ZKButton>(ID_MAIN_EStopBtn)->setBgStatusColor(ZK_CONTROL_STATUS_PRESSED, Colors::DarkRed);
		},
		[](ZKListView* pListView, ZKListView::ZKListItem* pListItem, int index) {
			if (pListView == nullptr || pListItem == nullptr)
				return;

			/* ListItem Overrides */
			switch (pListView->getID())
			{
			case ID_MAIN_FileListView: {
				pListItem->setBgStatusColor(ZK_CONTROL_STATUS_SELECTED, Colors::Yellow);
				break;
			}
			case ID_MAIN_AxisControlListView: {
				ZKListView::ZKListSubItem* pHomeBtn = pListItem->findSubItemByID(ID_MAIN_AxisControlHomeSubItem);
				pHomeBtn->setBgStatusColor(ZK_CONTROL_STATUS_NORMAL, Colors::Yellow);
				pHomeBtn->setBgStatusColor(ZK_CONTROL_STATUS_SELECTED, Colors::Blue2);
				break;
			}
			default:
				break;
			}
		});
} // namespace UI::Theme