/*
 * Network.h
 *
 *  Created on: 30 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_HARDWARE_NETWORK_H_
#define JNI_LOGIC_HARDWARE_NETWORK_H_

#include "restclient-cpp/restclient.h"
#include "sys/types.h"
#include <map>
#include <string>

namespace Comm
{
	typedef std::map<const char*, std::string> QueryParameters_t;

	bool Get(std::string url,
			 const char* subUrl,
			 RestClient::Response& r,
			 QueryParameters_t& queryParameters,
			 int32_t sessionKey = -1);
} // namespace Comm

#endif /* JNI_LOGIC_HARDWARE_NETWORK_H_ */
