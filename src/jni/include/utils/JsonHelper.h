/*
 * JsonHelper.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Aug 19, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _UTILS_JSON_HELPER_H_
#define _UTILS_JSON_HELPER_H_

#include "json/json.h"

using namespace std;

class JsonHelper {
public:
	static bool readJsonFile(const char *pJsonFile, Json::Value &json);
	static bool readJsonData(const char *pData, int len, Json::Value &json);
	static bool writeJsonFile(const char *pJsonFile, const Json::Value &json);
};

#endif /* _UTILS_JSON_HELPER_H_ */
