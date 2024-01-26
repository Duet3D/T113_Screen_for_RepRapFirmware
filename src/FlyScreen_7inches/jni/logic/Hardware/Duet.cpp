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
#include "utils.h"
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

	void Duet::SendGcode(const char* gcode)
	{
		switch (m_communicationType)
		{
		case CommunicationType::uart:
			SerialIo::Sendf("%s\n", gcode);
			break;
		case CommunicationType::network: {
			RestClient::Response r = RestClient::get(std::string(m_ipAddress.c_str()) + "/rr_gcode?gcode=" + gcode);

			if (r.code != 200)
			{
				dbg("rr_gcode?gcode=%s failed, returned response %d", gcode, r.code);
				break;
			}
			RequestReply();
			ProcessReply();
			break;
		}
		case CommunicationType::usb:
			// TODO
			break;
		}
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
