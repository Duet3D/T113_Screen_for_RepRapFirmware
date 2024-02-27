/*
 * Duet.cpp
 *
 *  Created on: 26 Jan 2024
 *      Author: Andy Everitt
 */
#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_VERBOSE
#include "timer.h"

#include "Communication.h"
#include "Debug.h"
#include "Duet.h"
#include "Hardware/JsonDecoder.h"
#include "Hardware/SerialIo.h"
#include "ObjectModel/PrinterStatus.h"
#include "UI/UserInterface.h"
#include "manager/ConfigManager.h"
#include "storage/StoragePreferences.h"
#include "uart/UartContext.h"
#include "utils.h"
#include "utils/TimeHelper.h"
#include "json/json.h"
#include <map>
#include <string>

namespace Comm
{
	Duet duet;

	Duet::Duet()
		: m_communicationType(CommunicationType::none), m_ipAddress(""), m_hostname(""), m_password(""),
		  m_sessionTimeout(0), m_lastRequestTime(0), m_sessionKey(noSessionKey),
		  m_pollInterval(defaultPrinterPollInterval)
	{
	}

	void Duet::Init()
	{
		SetPollInterval((uint32_t)StoragePreferences::getInt("poll_interval", defaultPrinterPollInterval));
		SetBaudRate((unsigned int)StoragePreferences::getInt("baud_rate", CONFIGMANAGER->getUartBaudRate()));
		SetIPAddress(StoragePreferences::getString("ip_address", "192.168.0."));
		SetHostname(StoragePreferences::getString("hostname", ""));
		SetPassword(StoragePreferences::getString("password", ""));
		SetCommunicationType((CommunicationType)StoragePreferences::getInt("communication_type", 0));
	}

	void Duet::Reset()
	{
		verbose("");
		m_sessionKey = noSessionKey;
		m_sessionTimeout = 0;
		m_lastRequestTime = 0;
	}

	void Duet::Reconnect()
	{
		warn("");
		Disconnect();
		Connect();
	}

	void Duet::SetCommunicationType(CommunicationType type)
	{
		if (type == m_communicationType)
			return;
		info("Setting communication type to %d", (int)type);
		StoragePreferences::putInt("communication_type", (int)type);
		Disconnect();

		m_communicationType = type;
		Connect();
	}

	void Duet::SetPollInterval(uint32_t interval)
	{
		if (interval < minPrinterPollInterval)
		{
			info("Poll interval too low, setting to %d", minPrinterPollInterval);
			interval = minPrinterPollInterval;
		}
		info("Setting poll interval to %d", interval);
		StoragePreferences::putInt("poll_interval", (int)interval);
		resetUserTimer(TIMER_UPDATE_DATA, (int)interval);
		m_pollInterval = interval;
	}

	void Duet::ScalePollInterval(float scale)
	{
		SetPollInterval(static_cast<uint32_t>(m_pollInterval * scale));
	}

	bool Duet::AsyncGet(const char* subUrl,
						QueryParameters_t& queryParameters,
						function<bool(RestClient::Response&)> callback,
						bool queue)
	{
		if (m_sessionKey == noSessionKey || (TimeHelper::getCurrentTime() - m_lastRequestTime > m_sessionTimeout))
		{
			if (!Connect())
			{
				warn("Failed to connect to Duet, cannot send get request %s", subUrl);
				return false;
			}
		}
		if (!Comm::AsyncGet(m_ipAddress, subUrl, queryParameters, callback, m_sessionKey, queue))
		{
			warn("Failed to send async get request %s", subUrl);
			return false;
		}
		m_lastRequestTime = TimeHelper::getCurrentTime();
		return true;
	}

	/*
	Tries to make a get request to Duet, if it returns 401 or 403 then it will run `rr_connect` and send the request
	again
	*/
	bool Duet::Get(const char* subUrl, RestClient::Response& r, QueryParameters_t& queryParameters)
	{
		if (m_sessionKey == noSessionKey || (TimeHelper::getCurrentTime() - m_lastRequestTime > m_sessionTimeout))
		{
			if (!Connect())
			{
				warn("Failed to connect to Duet, cannot send get request %s", subUrl);
				r.code = -1;
				return false;
			}
		}
		if (!Comm::Get(m_ipAddress, subUrl, r, queryParameters, m_sessionKey))
		{
			if (r.code == 401 || r.code == 403)
			{
				error("HTTP error %d: Likely invalid sessionKey %u. Running rr_connect", r.code, m_sessionKey);
				Connect();
				return Comm::Get(m_ipAddress, subUrl, r, queryParameters, m_sessionKey);
			}
			return false;
		}
		m_lastRequestTime = TimeHelper::getCurrentTime();
		return true;
	}

	/*
	Tries to make a post request to Duet, if it returns 401 or 403 then it will run `rr_connect` and send the request
	again
	*/
	bool Duet::Post(const char* subUrl,
					RestClient::Response& r,
					QueryParameters_t& queryParameters,
					const std::string& data)
	{
		if (m_sessionKey == noSessionKey || (TimeHelper::getCurrentTime() - m_lastRequestTime > m_sessionTimeout))
		{
			if (!Connect())
			{
				warn("Failed to connect to Duet, cannot send post request %s", subUrl);
				return false;
			}
		}
		if (!Comm::Post(m_ipAddress, subUrl, r, queryParameters, data, m_sessionKey))
		{
			if (r.code == 401 || r.code == 403)
			{
				error("HTTP error %d: Likely invalid sessionKey %d. Running rr_connect", r.code, m_sessionKey);
				Connect();
				return Comm::Post(m_ipAddress, subUrl, r, queryParameters, data, m_sessionKey);
			}
			return false;
		}
		m_lastRequestTime = TimeHelper::getCurrentTime();
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
			RestClient::Response r;
			QueryParameters_t query;
			query["gcode"] = gcode;
			AsyncGet(
				"/rr_gcode",
				query,
				[this, gcode](RestClient::Response& r) {
					if (r.code != 200)
					{
						UI::CONSOLE->AddResponse(
							utils::format("HTTP error %d: Failed to send gcode: %s", r.code, gcode).c_str());
						return false;
					}
					RequestReply(r);
					ProcessReply(r);
					return true;
				},
				true);
			break;
		}
		case CommunicationType::usb:
			// TODO
			break;
        default:
            break;
		}
	}

	bool Duet::UploadFile(const char* filename, std::string& contents)
	{
		info("Uploading file %s: %d bytes", filename, contents.size());
		UI::POPUP_WINDOW->Open();
		UI::POPUP_WINDOW->SetTextf(LANGUAGEMANAGER->getValue("uploading_file").c_str(), filename);
		switch (m_communicationType)
		{
		case CommunicationType::uart: {
			SendGcodef("M28 \"%s\"", filename);
			size_t prevPosition = 0;
			size_t position = contents.find("\n"); // Find the first occurrence of \n
			std::string line;
			while (position != std::string::npos)
			{
				line = contents.substr(prevPosition, position - prevPosition);
				prevPosition = position + 1;
				position = contents.find("\n", position + 1); // Find the next occurrence, if any
				SendGcode(line.c_str());
			}
			SendGcode("M29");
			break;
		}
		case CommunicationType::network: {
			RestClient::Response r;
			QueryParameters_t query;
			query["name"] = filename;
			if (!Post("/rr_upload", r, query, contents))
			{
				UI::CONSOLE->AddResponse(
					utils::format("HTTP error %d %s: Failed to upload file: %s", r.code, r.body, filename).c_str());
				return false;
			}
			break;
		}
		default:
			break;
		}
		return true;
	}

	void Duet::RequestModel(const char* flags)
	{
		switch (m_communicationType)
		{
		case CommunicationType::uart:
			SendGcodef("M409 F\"%s\"\n", flags);
			break;
		case CommunicationType::network: {
			RestClient::Response r;
			QueryParameters_t query;
			query["flags"] = flags;
			AsyncGet("/rr_model", query, [this, flags](RestClient::Response& r) {
				JsonDecoder decoder;
				if (r.code != 200)
				{
					UI::CONSOLE->AddResponse(
						utils::format("HTTP error %d: Failed to get model update for flags: %s", r.code, flags)
							.c_str());
					return false;
				}
				decoder.CheckInput((const unsigned char*)r.body.c_str(), r.body.length() + 1);
				return true;
			});
			break;
		}
		default:
			break;
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
			RestClient::Response r;
			QueryParameters_t query;
			query["key"] = key;
			query["flags"] = flags;
			AsyncGet("/rr_model", query, [this, key, flags](RestClient::Response& r) {
				JsonDecoder decoder;
				if (r.code != 200)
				{
					UI::CONSOLE->AddResponse(
						utils::format(
							"HTTP error %d: Failed to get model update for key: %s, flags: %s", r.code, key, flags)
							.c_str());
					return false;
				}
				decoder.CheckInput((const unsigned char*)r.body.c_str(), r.body.length() + 1);
				return true;
			});
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
			JsonDecoder decoder;
			RestClient::Response r;
			QueryParameters_t query;
			query["dir"] = dir;
			query["first"] = utils::format("%d", first);
			if (!Get("/rr_filelist", r, query))
				break;
			decoder.CheckInput((const unsigned char*)r.body.c_str(), r.body.length() + 1);
			break;
		}
		default:
			break;
		}
		return;
	}

	void Duet::RequestFileInfo(const char* filename)
	{
		stopThumbnailRequest = false;

		switch (m_communicationType)
		{
		case CommunicationType::uart:
			SendGcodef("M36 \"%s\"", filename);
			break;
		case CommunicationType::network: {
			JsonDecoder decoder;
			QueryParameters_t query;
			query["name"] = filename;
			UI::GetThumbnail()->setText("Loading...");

#if 0
			AsyncGet(
				"/rr_fileinfo",
				query,
				[this](RestClient::Response& r) -> bool {
					JsonDecoder decoder;
					if (r.code != 200)
					{
						return false;
					}
					decoder.CheckInput((const unsigned char*)r.body.c_str(), r.body.length() + 1);
					return true;
				},
				true);

			break;
#endif

/* This way is quicker but duplicates code to decode the received data */
#if 1
			std::string name(filename);
			AsyncGet(
				"/rr_fileinfo",
				query,
				[this, name](RestClient::Response& r) -> bool {
					Json::Reader reader;
					Json::Value body;
					const char* filename = name.c_str();
					dbg("Name = %s", filename);
					if (r.code != 200)
					{
						UI::CONSOLE->AddResponse(
							utils::format("HTTP error %d: Failed to get file info for file: %s", r.code, r.body)
								.c_str());
						return false;
					}
					reader.parse(r.body, body);
					if (body.isMember("err") && body["err"].asInt() != 0)
					{
						UI::CONSOLE->AddResponse(
							utils::format(
								"Failed to get file info for file: %s, returned error %d", r.body, body["err"].asInt())
								.c_str());
						return false;
					}
					if (!body.isMember("thumbnails"))
					{
						info("No thumbnails found for %s", filename);
						return false;
					}
					Json::Value thumbnailsJson = body["thumbnails"];
					for (Json::ArrayIndex i = 0; i < thumbnailsJson.size(); i++)
					{
						Thumbnail thumbnail;
						ThumbnailContext context;
						ThumbnailInit(thumbnail);
						if (!thumbnailsJson[i].isMember("width"))
						{
							continue;
						}
						thumbnail.width = thumbnailsJson[i]["width"].asInt();

						if (!thumbnailsJson[i].isMember("height"))
						{
							continue;
						}
						thumbnail.height = thumbnailsJson[i]["height"].asInt();

						if (!thumbnailsJson[i].isMember("offset"))
						{
							continue;
						}
						context.next = thumbnailsJson[i]["offset"].asInt();

						if (!thumbnailsJson[i].isMember("format"))
						{
							continue;
						}
						std::string format = thumbnailsJson[i]["format"].asString();
						if (!thumbnail.SetImageFormat(format.c_str()))
						{
							warn("Unsupported thumbnail format: %s", format.c_str());
							continue;
						}
						thumbnail.New(thumbnail.width, thumbnail.height, "/tmp/thumbnail");

						info("File %s has thumbnail %d: %dx%d", filename, i, thumbnail.width, thumbnail.height);

						QueryParameters_t query;
						query["name"] = filename;
						while (context.next != 0)
						{
							if (stopThumbnailRequest)
							{
								warn("Thumbnail request cancelled");
								stopThumbnailRequest = false;
								return false;
							}
							// Request thumbnail data
							query["offset"] = utils::format("%d", context.next);
							info("Requesting thumbnail data for %s at offset %d\n", filename, context.next);
							if (!Get("/rr_thumbnail", r, query))
							{
								error("Failed to get thumbnail data for %s at offset %d", filename, context.next);
								continue;
							}
							dbg("Parsing rr_thumbnail response");
							reader.parse(r.body, body);

							if (body.isMember("err") && body["err"].asInt() != 0)
							{
								error("Failed to get thumbnail data for %s at offset %d: %d",
									  filename,
									  context.next,
									  body["err"].asInt());
								continue;
							}

							if (body.isMember("next"))
							{
								context.next = body["next"].asInt();
								dbg("Next thumbnail offset: %d", context.next);
							}

							dbg("Decoding thumbnail data");
							if (body.isMember("data"))
							{
								ThumbnailData data;
								data.size = std::min(body["data"].asString().size(), sizeof(data.buffer));
								memcpy(data.buffer, body["data"].asString().c_str(), data.size);
								ThumbnailDecodeChunk(thumbnail, data);
							}
						}
						thumbnail.Close();
						UI::GetThumbnail()->setBackgroundPic("/tmp/thumbnail");
					}
					UI::GetThumbnail()->setText("");
					return true;
				},
				true);
			break;
#endif
		}
		default:
			break;
		}
		return;
	}

	void Duet::RequestThumbnail(const char* filename, uint32_t offset)
	{
		UI::GetThumbnail()->setText("Loading...");
		switch (m_communicationType)
		{
		case CommunicationType::uart:
			SendGcodef("M36.1 P\"%s\" S%d", filename, offset);
			break;
		case CommunicationType::network: {
			QueryParameters_t query;
			query["name"] = filename;
			query["offset"] = utils::format("%d", offset);
			AsyncGet(
				"/rr_thumbnail",
				query,
				[this](RestClient::Response& r) -> bool {
					JsonDecoder decoder;
					if (r.code != 200)
					{
						return false;
					}
					decoder.SetPrefix("thumbnail:");
					decoder.CheckInput((const unsigned char*)r.body.c_str(), r.body.size() + 1);
					return true;
				},
				true);
			break;
		}
		}
	}

	void Duet::ProcessReply(const RestClient::Response& reply)
	{
		if (m_communicationType != CommunicationType::network)
			return;

		if (reply.body.empty())
		{
			warn("Empty reply received");
			return;
		}

		JsonDecoder decoder;
		if (reply.body[0] != '{')
		{
			dbg("Reply not json: assuming it is a gcode response");
			size_t prevPosition = 0;
			size_t position = reply.body.find("\n"); // Find the first occurrence of \n

			// Split reply by new line and handle each as its own response.
			// The replicates the uart behaviour and is required because rr_reply will group multiple
			// replies together into a single response.
			while (position != std::string::npos)
			{
				StringRef ref((char*)"resp", 5);
				std::string line = reply.body.substr(prevPosition, position - prevPosition);
				dbg("line: %s", line.c_str());
				prevPosition = position + 1;
				position = reply.body.find("\n", position + 1); // Find the next occurrence, if any
				if (line.empty())
					continue;

				// Can skip checking the input since we know it's a gcode response
				decoder.ProcessReceivedValue(ref, line.c_str(), {});
			}
			return;
		}
		decoder.CheckInput((const unsigned char*)reply.body.c_str(), reply.body.length() + 1);
		return;
	}

	void Duet::RequestReply(RestClient::Response& r)
	{
		QueryParameters_t query;
		Get("/rr_reply", r, query);
	}

	const bool Duet::Connect(bool useSessionKey)
	{
		Disconnect();
		Reset();

		switch (m_communicationType)
		{
		case CommunicationType::uart: {
			info("Opening UART %s at %u", CONFIGMANAGER->getUartName().c_str(), m_baudRate.rate);
			return UARTCONTEXT->openUart(CONFIGMANAGER->getUartName().c_str(), m_baudRate.internal);
		}
		case CommunicationType::network: {
			info("Connecting to Duet at %s", m_ipAddress.c_str());

			RestClient::Response r;
			QueryParameters_t query;
			query["password"] = std::string("\"") + m_password + "\"";
			if (useSessionKey)
				query["sessionKey"] = "yes";

			return Comm::AsyncGet(
				m_ipAddress,
				"/rr_connect",
				query,
				[this](RestClient::Response& r) {
					verbose("parsing rr_connect response");
					Json::Reader reader;
					Json::Value body;

					if (!reader.parse(r.body, body))
					{
						error("Failed to parse JSON response from rr_connect");
						return false;
					}

					if (body.isMember("err") && body["err"].asInt() != 0)
					{
						error("rr_connect failed, returned error %d", body["err"].asInt());
						return false;
					}

					if (body.isMember("sessionTimeout"))
					{
						m_sessionTimeout = body["sessionTimeout"].asInt();
						m_lastRequestTime = TimeHelper::getCurrentTime();
						info("Duet session timeout set to %d", m_sessionTimeout);
					}

					if (body.isMember("sessionKey"))
					{
						m_sessionKey = body["sessionKey"].asUInt();
						info("Duet session key = %u", m_sessionKey);
					}
					info("rr_connect succeeded");
					return true;
				},
				0,
				true);
		}
		default:
			break;
		}

		return false;
	}

	const Duet::error_code Duet::Disconnect()
	{
		SetStatus(OM::PrinterStatus::connecting);
		switch (m_communicationType)
		{
		case CommunicationType::uart:
			UARTCONTEXT->closeUart();
			Thread::sleep(100);
			break;
		case CommunicationType::network: {
			ClearThreadPool();
			if (m_sessionKey == noSessionKey)
			{
				Reset();
				return 0;
			}
			RestClient::Response r;
			QueryParameters_t query;
			if (!Comm::Get(m_ipAddress, "/rr_disconnect", r, query, m_sessionKey))
			{
				error("rr_disconnect failed, returned response %d", r.code);
				return r.code;
			}
			Reset();
			return r.code;
		}
		default:
			break;
		}
		return 0;
	}

	void Duet::SetBaudRate(const unsigned int baudRateCode)
	{
		for (unsigned int i = 0; i < ARRAY_SIZE(baudRates); i++)
		{
			if (baudRates[i].internal == baudRateCode)
			{
				SetBaudRate(baudRates[i]);
				return;
			}
		}
		warn("Baud rate %u not found", baudRateCode);
	}

	void Duet::SetBaudRate(const baudrate_t& baudRate)
	{
		info("Setting baud rate to %u (%u)", baudRate.rate, baudRate.internal);
		StoragePreferences::putInt("baud_rate", baudRate.internal);
		m_baudRate = baudRate;
		if (UARTCONTEXT->isOpen())
		{
			UARTCONTEXT->closeUart();
			Thread::sleep(50);
			UARTCONTEXT->openUart(CONFIGMANAGER->getUartName().c_str(), baudRate.internal);
		}
	}

	void Duet::SetIPAddress(const std::string& ipAddress)
	{
		if (m_communicationType == CommunicationType::network)
			Disconnect();

		StoragePreferences::putString("ip_address", ipAddress);
		m_ipAddress = ipAddress;

		if (m_communicationType == CommunicationType::network)
			Connect();
	}

	void Duet::SetHostname(const std::string& hostname)
	{
		StoragePreferences::putString("hostname", hostname);
		m_hostname.clear();
		if (!m_hostname.rfind("http://", 0))
		{
			m_hostname += "http://";
		}
		m_hostname += hostname;
	}

	void Duet::SetPassword(const std::string& password)
	{
		StoragePreferences::putString("password", password);
		m_password = password;
	}

	void Duet::SetSessionKey(const uint32_t key)
	{
		m_sessionKey = key;
		info("Set Duet session key = %u", m_sessionKey);
	}
} // namespace Comm
