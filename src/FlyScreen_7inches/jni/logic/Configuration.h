/*
 * Configuration.h
 *
 * Created: 16/01/2015 13:18:16
 *  Author: David
 */


#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <sys/types.h>
#include <cstddef>

const uint32_t DefaultBaudRate = 57600;
const uint32_t DimDisplayTimeout = 60000;				// dim this display after no activity for this number of milliseconds
const uint32_t DefaultScreensaverTimeout = 120000;		// enable screensaver after no activity for this number of milliseconds
const uint32_t ScreensaverMoveTime = 10000;				// Jog around screen saver text after this number of milliseconds
const uint32_t DefaultInfoTimeout = 5;					// information message timeout in seconds
const uint8_t DefaultBabystepAmountIndex = 1;			// default babystep amount of 0.02mm
const uint16_t DefaultFeedrate = 6000;					// default feedrate in mm/min

const uint32_t MinimumEncoderCommandInterval = 100;		// minimum time in milliseconds between serial commands sent due to encoder movement

const size_t MaxFilenameLength = 120;

#endif /* CONFIGURATION_H_ */
