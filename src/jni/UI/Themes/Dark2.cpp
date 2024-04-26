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
		Green = 0xFF00FF00,
		VDarkBlue = 0xFF0B0D2B,
		VVDarkBlue = 0xFF0b0d17,
		DarkBlue = 0xFF0b0d56,
	};

	static ThemeColors s_dark2Theme = {
		// .id = "dark2",
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
				.bgImage = "Dark2/Background_Rounded_VDarkBlue.png",
				.background =
					{
						.normal = Colors::Clear,
						.pressed = Colors::DarkBlue,
						.selected = Colors::DarkBlue,
						.pressedAndSelected = Colors::DarkBlue,
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
						.normal = Colors::VVDarkBlue,
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
				.bgImage = "Dark2/hSliderBar.png",
				.validImage = nullptr,
				.thumb =
					{
						.normal = "Dark2/hSliderKnob.png",
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
						.normal = "Dark2/baseline_toggle_off_white_48dp.png",
						.pressed = nullptr,
						.selected = "Dark2/baseline_toggle_on_white_48dp.png",
						.pressedAndSelected = nullptr,
						.invalid = nullptr,
					},
			},
		.list =
			{
				.bgDefault = Colors::VVDarkBlue,
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
						.normal = Colors::Clear,
						.pressed = Colors::DarkBlue,
						.selected = Colors::DarkBlue,
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
						.pressed = Colors::DarkBlue,
						.selected = Colors::Clear,
						.pressedAndSelected = Colors::DarkBlue,
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
		.heightmap =
			{
				.bgDefault = Colors::VVDarkBlue,
				.gridColor = Colors::LightGray,
			},
		.objectCancel =
			{
				.bgDefault = Colors::VVDarkBlue,
				.bgCancelled = Colors::Red,
				.bgCurrent = Colors::Green,
				.bgBorder = Colors::Gray,
			},
		.homeScreenWindowSelect =
			std::map<const char*, Images>{
				{"move",
				 {
					 .normal = nullptr,
					 .pressed = nullptr,
				 }},
				{"extrude",
				 {
					 .normal = nullptr,
					 .pressed = nullptr,
				 }},
				{"status",
				 {
					 .normal = nullptr,
					 .pressed = nullptr,
				 }},
				{"heightmap",
				 {
					 .normal = nullptr,
					 .pressed = nullptr,
				 }},
				{"fans",
				 {
					 .normal = nullptr,
					 .pressed = nullptr,
				 }},
				{"files",
				 {
					 .normal = nullptr,
					 .pressed = nullptr,
				 }},
				{"object_cancel",
				 {
					 .normal = nullptr,
					 .pressed = nullptr,
				 }},
				{"webcam",
				 {
					 .normal = nullptr,
					 .pressed = nullptr,
				 }},
				{"network",
				 {
					 .normal = nullptr,
					 .pressed = nullptr,
				 }},
				{"settings",
				 {
					 .normal = nullptr,
					 .pressed = nullptr,
				 }},
			},
		.settingsScreenWindowSelect =
			std::map<const char*, Images>{
				{"language", {}},
				{"duet", {}},
				{"update", {}},
				{"restart", {}},
				{"dev", {}},
				{"power_off", {}},
				{"zk_setting", {}},
				{"touch_calibration", {}},
				{"guides", {}},
				{"brightness", {}},
				{"theme", {}},
				{"screensaver", {}},
				{"buzzer", {}},
				{"webcam", {}},
			},
	};

	static Theme s_theme(
		"dark2",
		&s_dark2Theme,
		[]() {
			/* Overrides */
			// Windows
			UI::GetRootWindow()->setBackgroundColor(Colors::Black);
			UI::GetUIControl<ZKWindow>(ID_MAIN_TemperatureGraphWindow)->setBackgroundColor(Colors::VVDarkBlue);
			UI::GetUIControl<ZKWindow>(ID_MAIN_FilamentLoadUnloadWindow)->setBackgroundColor(Colors::VVDarkBlue);
			UI::GetUIControl<ZKWindow>(ID_MAIN_DuetCommSettingWindow)->setBackgroundColor(Colors::Clear);
			UI::GetUIControl<ZKWindow>(ID_MAIN_DuetCommSettingWindow)->setBackgroundPic("Dark2/Background_Rounded_Large_VDarkBlue.png");
			UI::GetUIControl<ZKWindow>(ID_MAIN_GuideSelectionWindow)->setBackgroundColor(Colors::VVDarkBlue);
			UI::GetUIControl<ZKWindow>(ID_MAIN_ThemeSelectionWindow)->setBackgroundColor(Colors::VVDarkBlue);
			UI::GetUIControl<ZKWindow>(ID_MAIN_GuidedSetupWindow)->setBackgroundColor(Colors::VVDarkBlue);
			UI::GetUIControl<ZKWindow>(ID_MAIN_ScreensaverSettingWindow)->setBackgroundColor(Colors::VVDarkBlue);
			UI::GetUIControl<ZKWindow>(ID_MAIN_ScreensaverSettingWindow)->setBackgroundPic("Dark2/Background_Rounded_Large_VDarkBlue.png");
			UI::GetUIControl<ZKWindow>(ID_MAIN_BuzzerSettingWindow)->setBackgroundColor(Colors::VVDarkBlue);
			UI::GetUIControl<ZKWindow>(ID_MAIN_WebcamSettingWindow)->setBackgroundColor(Colors::VVDarkBlue);
			UI::GetUIControl<ZKWindow>(ID_MAIN_SliderWindow)->setBackgroundColor(Colors::VVDarkBlue);
			UI::GetUIControl<ZKWindow>(ID_MAIN_NumPadWindow)->setBackgroundColor(Colors::VVDarkBlue);
			UI::GetUIControl<ZKWindow>(ID_MAIN_NoTouchWindow)->setBackgroundColor(Colors::Clear);
			UI::GetUIControl<ZKWindow>(ID_MAIN_PopupWindow)->setBackgroundColor(Colors::VVDarkBlue);
			UI::GetUIControl<ZKWindow>(ID_MAIN_DebugWindow)->setBackgroundColor(Colors::VVDarkBlue);

			// Popup
			UI::GetUIControl<ZKSeekBar>(ID_MAIN_PopupProgress)->setBackgroundPic("ProgressBarEmpty.png");
			UI::GetUIControl<ZKSeekBar>(ID_MAIN_PopupProgress)->setProgressPic("ProgressBarFull.png");
			UI::GetUIControl<ZKSeekBar>(ID_MAIN_PopupProgress)->setThumbPic(ZK_CONTROL_STATUS_NORMAL, nullptr);
			// Popup rear overlay
			UI::GetUIControl<ZKButton>(ID_MAIN_OverlayModalZone)->setBackgroundColor(NULL);
			UI::GetUIControl<ZKButton>(ID_MAIN_OverlayModalZone)->setBgStatusColor(ZK_CONTROL_STATUS_NORMAL, NULL);
			UI::GetUIControl<ZKButton>(ID_MAIN_OverlayModalZone)->setBgStatusColor(ZK_CONTROL_STATUS_PRESSED, NULL);
			UI::GetUIControl<ZKButton>(ID_MAIN_OverlayModalZone)->setBackgroundPic(nullptr);

			// Sidebar Buttons
			UI::GetUIControl<ZKButton>(ID_MAIN_BackBtn)->setButtonStatusPic(ZK_CONTROL_STATUS_NORMAL,"Dark2/baseline_arrow_back_white_48dp.png");
			UI::GetUIControl<ZKButton>(ID_MAIN_HomeBtn)->setButtonStatusPic(ZK_CONTROL_STATUS_NORMAL,"Dark2/baseline_home_white_48dp.png");
			UI::GetUIControl<ZKButton>(ID_MAIN_MacroBtn)->setButtonStatusPic(ZK_CONTROL_STATUS_NORMAL,"Dark2/baseline_rule_folder_white_48dp.png");
			UI::GetUIControl<ZKButton>(ID_MAIN_ConsoleBtn)->setButtonStatusPic(ZK_CONTROL_STATUS_NORMAL,"Dark2/baseline_keyboard_command_key_white_48dp.png");

			// Scrollable text
			// UI::GetUIControl<ZKTextView>(ID_MAIN_PopupText)->setLongMode(ZKTextView::ELongMode::E_LONG_MODE_SCROLL);

			// EStop Button
			UI::GetUIControl<ZKButton>(ID_MAIN_EStopBtn)->setBackgroundPic(nullptr);
			UI::GetUIControl<ZKButton>(ID_MAIN_EStopBtn)->setBgStatusColor(ZK_CONTROL_STATUS_NORMAL, Colors::Red);
			UI::GetUIControl<ZKButton>(ID_MAIN_EStopBtn)->setBgStatusColor(ZK_CONTROL_STATUS_PRESSED, Colors::DarkRed);
			UI::GetUIControl<ZKButton>(ID_MAIN_EStopBtn)->setButtonStatusPic(ZK_CONTROL_STATUS_NORMAL,"Dark2/baseline_electric_bolt_white_48dp.png");
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
			case ID_MAIN_TemperatureGraphLegend: {
				pListItem->setBgStatusColor(ZK_CONTROL_STATUS_SELECTED, Colors::Gray);
				break;
			}
			default:
				break;
			}
		});
} // namespace UI::Theme
