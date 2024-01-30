/*
 * Network.cpp
 *
 *  Created on: 30 Jan 2024
 *      Author: Andy Everitt
 */

#define DEBUG (1)

#include "Network.h"
#include "Debug.h"
#include "curl/curl.h"
#include "restclient-cpp/connection.h"
#include "utils.h"

namespace Comm
{
	bool Get(std::string url,
			 const char* subUrl,
			 RestClient::Response& r,
			 QueryParameters_t& queryParameters,
			 int32_t sessionKey)
	{
		url += subUrl;

		if (queryParameters.size() > 0)
			url += "?";

		bool first = true;
		for (auto& query : queryParameters)
		{
			utils::replaceSubstring(query.second, " ", "%20");
			utils::replaceSubstring(query.second, "\"", "%22");
			utils::replaceSubstring(query.second, ":", "%3A");
			utils::replaceSubstring(query.second, "\\", "%5C");
			utils::replaceSubstring(query.second, "/", "%2F");
			utils::removeCharFromString(query.second, '\n');
			utils::removeCharFromString(query.second, '\r');

			if (!first)
			{
				url += "&";
			}
			url += query.first;
			url += "=";
			url += query.second;

			first = false;
		}

		// get a connection object
		RestClient::Connection* conn = new RestClient::Connection(url);

		// set connection timeout to 1s
		conn->SetTimeout(1);

		// enable following of redirects (default is off)
		conn->FollowRedirects(true);
		// and limit the number of redirects (default is -1, unlimited)
		conn->FollowRedirects(true, 3);

		// set headers
		if (sessionKey != -1)
			conn->AppendHeader("X-Session-Key", utils::format("%d", sessionKey));

		// if using a non-standard Certificate Authority (CA) trust file
		// conn->SetCAInfoFilePath(ConfigManager::getInstance()->getResFilePath("cacert.pem"));

		r = conn->get("");
		if (r.code != 200)
		{
			dbg("%s failed, returned response %d", url.c_str(), r.code);
			return false;
		}
		dbg("%s succeeded, returned response %d", url.c_str(), r.code);
		// dbg("Response body: %s", r.body.c_str());
		return true;
	}
} // namespace Comm
