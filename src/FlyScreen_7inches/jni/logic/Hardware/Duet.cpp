/*
 * Duet.cpp
 *
 *  Created on: 26 Jan 2024
 *      Author: Andy Everitt
 */

#define DEBUG (1)

#include "Duet.h"
#include "Debug.h"
#include "Hardware/SerialIo.h"
#include "manager/ConfigManager.h"
#include "storage/StoragePreferences.h"
#include "utils.h"
#include <map>
#include <string>

namespace Comm
{
	Duet duet;

	Duet::Duet() : m_communicationType(CommunicationType::uart), m_pollInterval(defaultPrinterPollInterval)
	{
		m_baudRate = (BaudRate)CONFIGMANAGER->getUartBaudRate();
		Reset();
	}

	void Duet::Reset() {}

	void Duet::SetCommunicationType(CommunicationType type)
	{
		StoragePreferences::putInt("communication_type", (int)type);
		m_communicationType = type;
	}

	void Duet::SetPollInterval(uint32_t interval)
	{
		m_pollInterval = interval;
	}

	void Duet::ScalePollInterval(float scale)
	{
		SetPollInterval(static_cast<uint32_t>(m_pollInterval * scale));
	}

	bool Get(std::string& url, RestClient::Response& r, std::map<const char*, std::string>& queryParameters)
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

		r = RestClient::get(url);
		if (r.code != 200)
		{
			dbg("%s failed, returned response %d", url.c_str(), r.code);
			return false;
		}
		dbg("%s succeeded, returned response %d", url.c_str(), r.code);
		return true;
	}

	void Duet::SendGcode(const char* gcode)
	{
		switch (m_communicationType)
		{
		case CommunicationType::uart:
			SerialIo::Sendf("%s\n", gcode);
			break;
		case CommunicationType::network: {
			std::string requestString = std::string("http://") + m_ipAddress.c_str() + "/rr_gcode";
			RestClient::Response r;
			std::map<const char*, std::string> query;
			query["gcode"] = gcode;
			if (!Get(requestString, r, query))
				break;
			RequestReply();
			ProcessReply();
			break;
		}
		case CommunicationType::usb:
			// TODO
			break;
		}
	}

	void Duet::RequestModel(const char* flags)
	{
		switch (m_communicationType)
		{
		case CommunicationType::uart:
			SendGcodef("M409 F\"%s\"\n", flags);
			break;
		case CommunicationType::network: {
			std::string requestString = std::string("http://") + m_ipAddress.c_str() + "/rr_model";
			RestClient::Response r;
			std::map<const char*, std::string> query;
			query["flags"] = flags;
			if (!Get(requestString, r, query))
				break;
			SerialIo::CheckInput((const unsigned char*)r.body.c_str(), r.body.length() + 1);
			break;
		}
		}
	}

	void Duet::RequestModel(const char* key, const char* flags)
	{
		switch (m_communicationType)
		{
		case CommunicationType::uart:
			SendGcodef("M409 K\"%s\" F\"%s\"\n", key, flags);
			break;
		case CommunicationType::network: {
			std::string requestString = std::string("http://") + m_ipAddress.c_str() + "/rr_model";
			RestClient::Response r;
			std::map<const char*, std::string> query;
			query["key"] = key;
			query["flags"] = flags;
			if (!Get(requestString, r, query))
				break;
			SerialIo::CheckInput((const unsigned char*)r.body.c_str(), r.body.length() + 1);
			break;
		}
		default:
			break;
		}
		return;
	}

	void Duet::RequestFileList(const char* dir, const size_t first)
	{
		switch (m_communicationType)
		{
		case CommunicationType::uart:
			SendGcodef("M20 S3 P\"%s\" R%d\n", dir, first);
			break;
		case CommunicationType::network: {
			std::string requestString = std::string("http://") + m_ipAddress.c_str() + "/rr_filelist";
			RestClient::Response r;
			std::map<const char*, std::string> query;
			query["dir"] = dir;
			query["first"] = utils::format("%d", first);
			if (!Get(requestString, r, query))
				break;
			SerialIo::CheckInput((const unsigned char*)r.body.c_str(), r.body.length() + 1);
			break;
		}
		default:
			break;
		}
		return;
	}

	void Duet::ProcessReply()
	{
		switch (m_communicationType)
		{
		case CommunicationType::uart:
			break;
		case CommunicationType::network: {
			if (m_reply.body.empty())
			{
				dbg("Empty reply received");
				break;
			}
			if (m_reply.body[0] != '{')
			{
				dbg("Reply not json: assuming it is a gcode response");
				size_t prevPosition = 0;
				size_t position = m_reply.body.find("\n"); // Find the first occurrence of \n

				while (position != std::string::npos)
				{
					std::string line = std::string("{\"resp\": \"") +
									   m_reply.body.substr(prevPosition, position - prevPosition) + "\"}\n";
					SerialIo::CheckInput((const unsigned char*)line.c_str(), line.length() + 1);
					prevPosition = position + 1;
					position = m_reply.body.find("\n", position + 1); // Find the next occurrence, if any
				}
				break;
			}
			SerialIo::CheckInput((const unsigned char*)m_reply.body.c_str(), m_reply.body.length() + 1);
			break;
		}
		case CommunicationType::usb:
			// TODO
			break;
		}
	}

	const RestClient::Response* Duet::RequestReply()
	{
		m_reply = RestClient::get(std::string(m_ipAddress.c_str()) + "/rr_reply");
		if (m_reply.code != 200)
		{
			dbg("rr_reply failed, returned response %d", m_reply.code);
		}
		return &m_reply;
	}

} // namespace Comm
