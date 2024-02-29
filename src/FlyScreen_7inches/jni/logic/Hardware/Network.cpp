/*
 * Network.cpp
 *
 *  Created on: 30 Jan 2024
 *      Author: Andy Everitt
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_VERBOSE

#include "Debug.h"
#include "Network.h"
#include "curl/curl.h"
#include "restclient-cpp/connection.h"
#include "utils.h"
#include <system/Thread.h>
#include <vector>

namespace Comm
{
	// Duet 2 seems to only support 3 concurrent connections. We need 1 connection for synchronous requests, so we can
	// only have 2 threads.
	constexpr size_t maxThreadPoolSize = 2;

	struct AsyncGetData
	{
		std::string url;
		const char* subUrl;
		QueryParameters_t queryParameters;
		function<bool(RestClient::Response&)> callback;
		uint32_t sessionKey;
	};

	class AsyncGetThread : public Thread
	{
	  public:
		AsyncGetThread(std::string url,
					   const char* subUrl,
					   QueryParameters_t& queryParameters,
					   function<bool(RestClient::Response&)> callback,
					   uint32_t sessionKey)
			: url(url), subUrl(subUrl), queryParameters(queryParameters), sessionKey(sessionKey), callback(callback)
		{
			dbg("starting thread for %s%s", url.c_str(), subUrl);
			run();
		}
		virtual bool threadLoop()
		{
			verbose("%s%s", url.c_str(), subUrl);
			if (!Get(url, subUrl, r, queryParameters, sessionKey))
			{
				return false;
			}

			callback(r);
			return false;
		}

		void SetRequestParameters(std::string url,
								  const char* subUrl,
								  QueryParameters_t& queryParameters,
								  function<bool(RestClient::Response&)> callback,
								  uint32_t sessionKey)
		{
			this->url = url;
			this->subUrl = subUrl;
			this->queryParameters = queryParameters;
			this->callback = callback;
			this->sessionKey = sessionKey;
		}

	  private:
		std::string url;
		const char* subUrl;
		RestClient::Response r;
		QueryParameters_t queryParameters;
		int32_t sessionKey;
		function<bool(RestClient::Response&)> callback;
	};

	static std::vector<AsyncGetThread*> threadPool;
	static std::vector<AsyncGetData> queuedData;

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

	bool AsyncGet(std::string url,
				  const char* subUrl,
				  QueryParameters_t& queryParameters,
				  function<bool(RestClient::Response&)> callback,
				  uint32_t sessionKey,
				  bool queue)
	{
		// Attempts to use a thread from the pool if one is not currently in use
		for (auto thread : threadPool)
		{
			if (thread->isRunning())
				continue;

			verbose("Reusing thread from pool");
			thread->SetRequestParameters(url, subUrl, queryParameters, callback, sessionKey);
			return thread->run();
		}

		if (threadPool.size() >= maxThreadPoolSize)
		{
			if (!queue)
			{
				error("Thread pool is full, cannot add more threads");
				return false;
			}
			if (strncmp(subUrl, "/rr_connect", 11) == 0)
			{
				for (auto& data : queuedData)
				{
					if (data.url == url && data.subUrl == subUrl)
					{
						info("Request %s already queued, not adding again", (url + subUrl).c_str());
						return false;
					}
				}
			}
			queuedData.push_back({url, subUrl, queryParameters, callback, sessionKey});
			info("Queued request %s, size=%d", (url + subUrl).c_str(), queuedData.size());
			return true;
		}

		// Create a new thread and add it to the pool
		AsyncGetThread* thread = new AsyncGetThread(url, subUrl, queryParameters, callback, sessionKey);
		threadPool.push_back(thread);
		info("Added thread to pool, size=%d", threadPool.size());
		return true;
	}

	void ProcessQueuedAsyncRequests()
	{
		if (queuedData.empty())
			return;

		info("Processing queued requests, size=%d", queuedData.size());
		auto data = queuedData.begin();
		while (data != queuedData.end())
		{
			info("Processing queued request %s", (data->url + data->subUrl).c_str());
			if (!AsyncGet(data->url, data->subUrl, data->queryParameters, data->callback, data->sessionKey, false))
			{
				error("Failed to process queued request %s", (data->url + data->subUrl).c_str());
				return;
			}
			info("Processed queued request %s", (data->url + data->subUrl).c_str());
			data = queuedData.erase(data);
		}
		info("Processed all queued requests, size=%d", queuedData.size());
	}

	int ClearThreadPool()
	{
		int count = threadPool.size();
		threadPool.clear();
		return count - threadPool.size();
	}

	bool Get(std::string url,
			 const char* subUrl,
			 RestClient::Response& r,
			 QueryParameters_t& queryParameters,
			 uint32_t sessionKey)
	{
		url += subUrl;

		AddQueryParameters(url, queryParameters);

		// get a connection object
		RestClient::Connection* conn = new RestClient::Connection(url);

		// set connection timeout in seconds
		conn->SetTimeout(3);

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

		delete conn;
		return true;
	}

	bool Post(std::string url,
			  const char* subUrl,
			  RestClient::Response& r,
			  QueryParameters_t& queryParameters,
			  const std::string& data,
			  uint32_t sessionKey)
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
