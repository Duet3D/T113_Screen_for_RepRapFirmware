/*
 * UserInterfaceConstants.hpp
 *
 *  Created on: 10 Jan 2017
 *      Author: David
 */

#ifndef SRC_UI_USERINTERFACECONSTANTS_HPP_
#define SRC_UI_USERINTERFACECONSTANTS_HPP_

//#include <UI/DisplaySize.hpp>
//#include <Configuration.hpp>

static const char* _ecv_array const axisNames[] = { "X", "Y", "Z", "U", "V", "W" };
constexpr size_t MaxTotalAxes = 15;		// This needs to be kept in sync with the maximum in RRF for any build configuration
constexpr size_t MaxHeatersPerTool = 8;
static const char* _ecv_array const babystepAmounts[] = { "0.01", "0.02", "0.05", "0.1" };
static const float _ecv_array babystepAmountsF[] = { 0.01, 0.02, 0.05, 0.1 };
constexpr int8_t NoTool = -1;

const unsigned int MaxSlots = 7;
const unsigned int MaxHeaters = 32;
#define MaxDisplayableAxes	(6)

#define MaxCommandLength (50)

#endif /* SRC_UI_USERINTERFACECONSTANTS_HPP_ */
