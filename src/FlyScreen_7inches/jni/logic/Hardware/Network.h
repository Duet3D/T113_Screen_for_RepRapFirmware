/*
 * Network.h
 *
 *  Created on: 30 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_HARDWARE_NETWORK_H_
#define JNI_LOGIC_HARDWARE_NETWORK_H_

#include "restclient-cpp/restclient.h"
#include "std_fixed/functional.h"
#include "sys/types.h"
#include <map>
#include <string>

namespace Comm
{
	typedef std::map<const char*, std::string> QueryParameters_t;

	bool AsyncGet(std::string url,
				  const char* subUrl,
				  QueryParameters_t& queryParameters,
				  function<bool(RestClient::Response&)> callback,
				  uint32_t sessionKey = 0,
				  bool queue = false);

	void ProcessQueuedAsyncRequests();

	bool Get(std::string url,
			 const char* subUrl,
			 RestClient::Response& r,
			 QueryParameters_t& queryParameters,
			 uint32_t sessionKey = 0);

	bool Post(std::string url,
			  const char* subUrl,
			  RestClient::Response& r,
			  QueryParameters_t& queryParameters,
			  const std::string& data,
			  uint32_t sessionKey = 0);
} // namespace Comm

#endif /* JNI_LOGIC_HARDWARE_NETWORK_H_ */
