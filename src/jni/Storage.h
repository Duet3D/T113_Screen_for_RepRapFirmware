/*
 * Storage.h
 *
 *  Created on: 16 Apr 2024
 *      Author: andy
 */

#ifndef JNI_STORAGE_H_
#define JNI_STORAGE_H_

#include <sys/types.h>

constexpr const char* ID_HEIGHTMAP_RENDER_MODE = "heightmap_render_mode";

constexpr const char* ID_DUET_BAUD_RATE = "baud_rate";
constexpr const char* ID_DUET_HOSTNAME = "hostname";
constexpr const char* ID_DUET_PASSWORD = "password";
constexpr const char* ID_DUET_COMMUNICATION_TYPE = "communication_type";
constexpr const char* ID_DUET_POLL_INTERVAL = "poll_interval";

constexpr const char* ID_THEME = "theme";
constexpr const char* ID_SHOW_SETUP_ON_STARTUP = "show_setup_on_startup";
constexpr const char* ID_SCREENSAVER_ENABLE = "screensaver_enable";
constexpr const char* ID_SCREENSAVER_TIMEOUT = "screensaver_timeout";
constexpr const char* ID_INFO_TIMEOUT = "info_timeout";
constexpr const char* ID_UPGRADE_FILE_LAST_MODIFIED = "upgrade_file_last_modified";
constexpr const char* ID_BUZZER_ENABLED = "buzzer_enabled";

constexpr const char* ID_SYS_LANG_CODE_KEY = "sys_lang_code_key";
constexpr const char* ID_SYS_BRIGHTNESS_KEY = "sys_brightness_key";

constexpr const char* ID_WEBCAM_URL = "webcam_url_%d";
constexpr const char* ID_ACTIVE_WEBCAM_INDEX = "webcam_active_index";
constexpr const char* ID_WEBCAM_UPADTE_ITERVAL = "webcam_update_interval";

constexpr const char* ID_USB_HOST_MODE = "usb_host_mode";
constexpr const char* ID_CONSOLE_SYSTEM_COMMANDS = "console_system_commands";

constexpr const char* ID_BABYSTEP_AMOUNT = "baby_step_amount";
constexpr const char* ID_MOVE_FEEDRATE = "move_feedrate_%d";
constexpr const char* ID_MOVE_SELECTED_FEEDRATE = "move_selected_feedrate";
constexpr const char* ID_EXTRUSION_DISTANCE = "extrusion_distance_%d";
constexpr const char* ID_EXTRUSION_SELECTED_DISTANCE = "extrusion_selected_distance";
constexpr const char* ID_EXTRUSION_FEEDRATE = "extrusion_feedrate_%d";
constexpr const char* ID_EXTRUSION_SELECTED_FEEDRATE = "extrusion_selected_feedrate";

#endif /* JNI_STORAGE_H_ */
