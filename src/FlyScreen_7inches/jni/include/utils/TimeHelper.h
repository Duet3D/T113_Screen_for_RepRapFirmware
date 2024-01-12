/*
 * TimeHelper.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: May 22, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _UTILS_TIME_HELPER_H_
#define _UTILS_TIME_HELPER_H_

#include <time.h>

/**
 * @brief Time manipulation helper class
 */
class TimeHelper
{
  public:
	/**
	 * @brief Get the current date and time in milliseconds
	 */
	static long long getCurrentTime();

	/**
	 * @brief Get the current date
	 */
	static struct tm* getDateTime();

	/**
	 * @brief Set the date
	 */
	static bool setDateTime(struct tm* ptm);

	/**
	 * @brief Set the date
	 * @param pDate Format: 2017-09-13 16:00:00
	 */
	static bool setDateTime(const char* pDate);
};

#endif /* _UTILS_TIME_HELPER_H_ */
