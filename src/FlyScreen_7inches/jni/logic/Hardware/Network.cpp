/*
 * Network.cpp
 *
 *  Created on: 30 Jan 2024
 *      Author: Andy Everitt
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG

#include "Network.h"
#include "Debug.h"
#include "curl/curl.h"
#include "restclient-cpp/connection.h"
#include "utils.h"

namespace Comm
{
	static void AddQueryParameters(std::string& url, QueryParameters_t& queryParameters)
	{
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
	}

	bool Get(std::string url,
			 const char* subUrl,
			 RestClient::Response& r,
			 QueryParameters_t& queryParameters,
			 int32_t sessionKey)
	{
		url += subUrl;

		AddQueryParameters(url, queryParameters);

		// get a connection object
		RestClient::Connection* conn = new RestClient::Connection(url);

		// set connection timeout to 1s
		conn->SetTimeout(1);

		// enable following of redirects (default is off)
		conn->FollowRedirects(true);
		// and limit the number of redirects (default is -1, unlimited)
		conn->FollowRedirects(true, 3);

		// set headers
		conn->AppendHeader("X-Session-Key", utils::format("%u", sessionKey));

		// if using a non-standard Certificate Authority (CA) trust file
		// conn->SetCAInfoFilePath(ConfigManager::getInstance()->getResFilePath("cacert.pem"));

		dbg("Get: \"%s\", sessionKey=%u", url.c_str(), sessionKey);
		r = conn->get("");
		if (r.code != 200)
		{
			error("%s failed, returned response %d", url.c_str(), r.code);
			return false;
		}
		dbg("%s succeeded, returned response %d", url.c_str(), r.code);
		verbose("Response body: %s", r.body.c_str());
		return true;
	}

	bool Post(std::string url,
			  const char* subUrl,
			  RestClient::Response& r,
			  QueryParameters_t& queryParameters,
			  const std::string& data,
			  int32_t sessionKey)
	{
		url += subUrl;

		AddQueryParameters(url, queryParameters);

		// get a connection object
		RestClient::Connection* conn = new RestClient::Connection(url);

		// set connection timeout in seconds
		conn->SetTimeout(180);

		// enable following of redirects (default is off)
		conn->FollowRedirects(true);
		// and limit the number of redirects (default is -1, unlimited)
		conn->FollowRedirects(true, 3);

		// set headers
		conn->AppendHeader("X-Session-Key", utils::format("%u", sessionKey));

		conn->AppendHeader("Content-Type", "text/plain");
		// if using a non-standard Certificate Authority (CA) trust file
		// conn->SetCAInfoFilePath(ConfigManager::getInstance()->getResFilePath("cacert.pem"));

		verbose("Post: \"%s\", data=\"%s\"", url.c_str(), data.substr(0, 50).c_str());
		r = conn->post("", data);
		if (r.code != 200)
		{
			error("%s failed, returned response %d %s", url.c_str(), r.code, r.body.c_str());
			return false;
		}
		dbg("%s succeeded, returned response %d", url.c_str(), r.code);
		verbose("Response body: %s", r.body.c_str());
		return true;
	}
} // namespace Comm
