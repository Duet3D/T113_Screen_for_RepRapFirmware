/*
 * UserInterfaceConstants.h
 *
 *  Created on: 10 Jan 2017
 *      Author: David
 */

#ifndef SRC_UI_USERINTERFACECONSTANTS_HPP_
#define SRC_UI_USERINTERFACECONSTANTS_HPP_

//#include <UI/DisplaySize.h>
//#include <Configuration.h>

static const char* _ecv_array const axisNames[] = { "X", "Y", "Z", "U", "V", "W" };
constexpr size_t MaxTotalAxes = 15;		// This needs to be kept in sync with the maximum in RRF for any build configuration
constexpr size_t MaxExtrudersPerTool = 8;
constexpr size_t MaxHeatersPerTool = 8;
static const char* _ecv_array const babystepAmounts[] = { "0.01", "0.02", "0.05", "0.1" };
static const float _ecv_array babystepAmountsF[] = { 0.01, 0.02, 0.05, 0.1 };
constexpr int8_t NoTool = -1;

const unsigned int MaxSlots = 32;
const unsigned int MaxFans = 12;
const unsigned int MaxHeaters = 32;
constexpr unsigned int MaxDisplayableAxes = 6;

constexpr unsigned int MaxCommandLength = 50;
constexpr unsigned int MaxResponseLineLength = 65;
constexpr unsigned int MaxResponseLines = 50;

constexpr unsigned int MaxFileNameLength = 100;

#endif /* SRC_UI_USERINTERFACECONSTANTS_HPP_ */
