/*
 * AdcHelper.h
 *
 *  Created on: May 15, 2019
 *      Author: ZKSWE Develop Team
 */

#ifndef _UTILS_ADC_HELPER_H_
#define _UTILS_ADC_HELPER_H_

#include <stdio.h>

/**
 * @brief ADC operation helper class
 */
class AdcHelper {
public:
	static bool setChannel(int ch);

	/**
	 * @brief Set the ADC enable status
	 * @param isEnable true to enable, false to disable
	 * @return true if successful, false otherwise
	 */
	static bool setEnable(bool isEnable);

	/**
	 * @brief Read the ADC value
	 * @return the ADC value if successful, -1 otherwise
	 */
	static int getVal();
};

#endif /* _UTILS_ADC_HELPER_H_ */
