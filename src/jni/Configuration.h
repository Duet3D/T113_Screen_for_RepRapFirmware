/*
 * Configuration.h
 *
 *  Created on: 15 Mar 2024
 *      Author: andy
 */

#ifndef JNI_CONFIGURATION_H_
#define JNI_CONFIGURATION_H_

#include <sys/types.h>

constexpr const char* UPGRADE_FILE_NAME = "DuetScreen.bin";

/* Duet */
constexpr uint32_t DEFAULT_PRINTER_POLL_INTERVAL = 500;
constexpr uint32_t MIN_PRINTER_POLL_INTERVAL = 250;
constexpr uint32_t PRINTER_REQUEST_TIMEOUT = 2000;
constexpr int DEFAULT_COMMUNICATION_TYPE = 0;
constexpr const char* DEFAULT_IP_ADDRESS = "192.168.0.";
constexpr size_t MAX_IP_LENGTH = 50;
constexpr size_t MAX_HOSTNAME_LENGTH = 64;
constexpr unsigned long long TIME_SYNC_INTERVAL = 10e3; // Interval to resynchronize time with the Duet in milliseconds
constexpr size_t MAX_UART_UPLOAD_SIZE = 1024;
constexpr const char* DEFAULT_FILAMENTS_FILE = "filaments.csv";
constexpr const char* DEFAULT_HEIGHTMAPS_FILE = "heightmaps.csv";

/* Thumbnails */
constexpr int32_t FILE_CACHE_REQUEST_TIMEOUT = 5000;
constexpr size_t MAX_THUMBNAIL_CACHE_PIXELS = 64; // Largest pixel width/height thumbnail that is allowed to be cached
constexpr int32_t BACKGROUND_FILE_CACHE_POLL_INTERVAL = 2000;

/* Json Decoder */
constexpr size_t MAX_ARRAY_NESTING = 4;
constexpr size_t MAX_JSON_ID_LENGTH = 200;
constexpr size_t MAX_JSON_VALUE_LENGTH = 4096;

/* Network */
// Duet 2 seems to only support 3 concurrent connections. We need 1 connection for synchronous requests, so we can
// only have 2 threads.
constexpr size_t MAX_THREAD_POOL_SIZE = 2;

/* Object Model */
constexpr size_t MAX_TOTAL_AXES = 15; // This needs to be kept in sync with the maximum in RRF
constexpr size_t MAX_EXTRUDERS_PER_TOOL = 8;
constexpr size_t MAX_HEATERS_PER_TOOL = 8;
constexpr unsigned int MAX_TOOL_NAME_LENGTH = 10;
constexpr unsigned int MAX_FILAMENT_NAME_LENGTH = 100;
constexpr unsigned int MAX_FILENAME_LENGTH = 100;
constexpr unsigned int MAX_SLOTS = 32;
constexpr unsigned int MAX_FANS = 12;
constexpr unsigned int MAX_HEATERS = 32;
constexpr unsigned int MAX_SENSORS = 32;
constexpr unsigned int MAX_ENDSTOPS = 20;

/* Heightmap */
constexpr double HEIGHTMAP_FIXED_MAX = 0.25;
constexpr double HEIGHTMAP_FIXED_MIN = -0.25;

// Increasing this too high will cause a delay when rendering the heightmap.
// Recommended max of 100
constexpr size_t HEIGHTMAP_COLORBAR_SAMPLES = 100;

/* Console */
constexpr unsigned int MAX_COMMAND_LENGTH = 50;
constexpr unsigned int MAX_RESPONSE_LINE_LENGTH = 80;
constexpr unsigned int MAX_RESPONSE_LINES = 500;

/* Misc UI */
constexpr bool DEFAULT_SHOW_SETUP_ON_STARTUP = true;
constexpr const char* DEFAULT_LANGUAGE_CODE = "en_US";

/* Popup */
constexpr uint32_t DEFAULT_POPUP_TIMEOUT = 5000;

/* Alert */
constexpr size_t ALERT_TEXT_LENGTH = 165;		 // maximum characters in the alert text
constexpr size_t ALERT_TITLE_LENGTH = 50;		 // maximum characters in the alert title
constexpr size_t ALERT_RESPONSE_LENGTH = 50;	 // maximum characters in the alert response
constexpr size_t ALERT_MAX_CHOICES = 40;		 // maximum number of choices in the alert
constexpr size_t ALERT_CHOICES_TEXT_LENGTH = 50; // maximum characters in the alert choice text

/* Temperature Graph */
constexpr float DEFAULT_TEMP_GRAPH_MAX = 300;
constexpr size_t DEFAULT_TEMP_GRAPH_TIME_RANGE = 60;
constexpr float TEMP_GRAPH_Y_AXIS_PADDING = 10;
constexpr size_t GRAPH_DATAPOINTS = 200;

#endif /* JNI_CONFIGURATION_H_ */
