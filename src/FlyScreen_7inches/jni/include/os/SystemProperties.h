/*
 * SystemProperties.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Mar 6, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _OS_SYSTEM_PROPERTIES_H_
#define _OS_SYSTEM_PROPERTIES_H_

class SystemProperties {
public:
	static int setInt(const char *key, const int value);
	static int getInt(const char *key, int *value, const int defValue);

	static int setString(const char *key, const char *value);
	static int getString(const char *key, char *value, const char *defValue);
};

#endif /* _OS_SYSTEM_PROPERTIES_H_ */
