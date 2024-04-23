/*
 * Duet.cpp
 *
 *  Created on: 26 Jan 2024
 *      Author: Andy Everitt
 */
#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_VERBOSE
#include "timer.h"

#include "Comm/Communication.h"
#include "Comm/FileInfo.h"
#include "Comm/JsonDecoder.h"
#include "Debug.h"
#include "Duet.h"
#include "Hardware/SerialIo.h"
#include "ObjectModel/PrinterStatus.h"
#include "ObjectModel/Utils.h"
#include "Storage.h"
#include "UI/Graph.h"
#include "UI/UserInterface.h"
#include "manager/ConfigManager.h"
#include "storage/StoragePreferences.h"
#include "uart/UartContext.h"
#include "utils/TimeHelper.h"
#include "utils/utils.h"
#include "json/json.h"
#include <map>
#include <string>

namespace Comm
{
	Duet::Duet()
		: m_communicationType(CommunicationType::none), m_hostname(""), m_password(""), m_sessionTimeout(0),
		  m_lastRequestTime(0), m_sessionKey(sm_noSessionKey), m_pollInterval(DEFAULT_PRINTER_POLL_INTERVAL)
	{
	}

	void Duet::Init()
	{
		SetPollInterval((uint32_t)StoragePreferences::getInt(ID_DUET_POLL_INTERVAL, DEFAULT_PRINTER_POLL_INTERVAL));
		SetBaudRate((unsigned int)StoragePreferences::getInt(ID_DUET_BAUD_RATE, CONFIGMANAGER->getUartBaudRate()));
		SetIPAddress("");
		SetHostname(StoragePreferences::getString(ID_DUET_HOSTNAME, ""));
		SetPassword(StoragePreferences::getString(ID_DUET_PASSWORD, ""));
		SetCommunicationType(
			(CommunicationType)StoragePreferences::getInt(ID_DUET_COMMUNICATION_TYPE, (int)DEFAULT_COMMUNICATION_TYPE));
	}

	void Duet::Reset()
	{
		verbose("");
		m_sessionKey = sm_noSessionKey;
		m_sessionTimeout = 0;
		m_lastRequestTime = 0;
		ClearIPAddress();

		OM::RemoveAll();
		Comm::ResetSeqs();
		FILEINFO_CACHE->ClearCache();
		UI::TEMPERATURE_GRAPH.ClearAll();
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
		StoragePreferences::putInt(ID_DUET_COMMUNICATION_TYPE, (int)type);
		Disconnect();

		m_communicationType = type;
		Connect();
	}

	void Duet::SetPollInterval(uint32_t interval)
	{
		if (interval < MIN_PRINTER_POLL_INTERVAL)
		{
			info("Poll interval too low, setting to %d", MIN_PRINTER_POLL_INTERVAL);
			interval = MIN_PRINTER_POLL_INTERVAL;
		}
		info("Setting poll interval to %d", interval);
		StoragePreferences::putInt(ID_DUET_POLL_INTERVAL, (int)interval);
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
		if (m_sessionKey == sm_noSessionKey || (TimeHelper::getCurrentTime() - m_lastRequestTime > m_sessionTimeout))
		{
			if (!Connect())
			{
				warn("Failed to connect to Duet, cannot send get request %s", subUrl);
				return false;
			}
		}
		if (!Comm::AsyncGet(GetBaseUrl(), subUrl, queryParameters, callback, m_sessionKey, queue))
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
		if (m_sessionKey == sm_noSessionKey || (TimeHelper::getCurrentTime() - m_lastRequestTime > m_sessionTimeout))
		{
			if (!Connect())
			{
				warn("Failed to connect to Duet, cannot send get request %s", subUrl);
				r.code = -1;
				return false;
			}
		}
		if (!Comm::Get(GetBaseUrl(), subUrl, r, queryParameters, m_sessionKey))
		{
			if (r.code == 401 || r.code == 403)
			{
				error("HTTP error %d: Likely invalid sessionKey %u. Running rr_connect", r.code, m_sessionKey);
				Connect();
				return Comm::Get(GetBaseUrl(), subUrl, r, queryParameters, m_sessionKey);
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
		if (m_sessionKey == sm_noSessionKey || (TimeHelper::getCurrentTime() - m_lastRequestTime > m_sessionTimeout))
		{
			if (!Connect())
			{
				warn("Failed to connect to Duet, cannot send post request %s", subUrl);
				return false;
			}
		}
		if (!Comm::Post(GetBaseUrl(), subUrl, r, queryParameters, data, m_sessionKey))
		{
			if (r.code == 401 || r.code == 403)
			{
				error("HTTP error %d: Likely invalid sessionKey %d. Running rr_connect", r.code, m_sessionKey);
				Connect();
				return Comm::Post(GetBaseUrl(), subUrl, r, queryParameters, data, m_sessionKey);
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

	void Duet::SendGcodef(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		SendGcode(utils::format(fmt, args).c_str());
		va_end(args);
	}

	bool Duet::UploadFile(const char* filename, const std::string& contents)
	{
		info("Uploading file %s: %d bytes", filename, contents.size());
		// TODO work out why the UI doesn't actually get updated in this function
		UI::POPUP_WINDOW->Open();
		UI::POPUP_WINDOW->SetTitle(LANGUAGEMANAGER->getValue("uploading_file").c_str());
		UI::POPUP_WINDOW->SetText(filename);
		UI::POPUP_WINDOW->SetTextScrollable(false);
		UI::POPUP_WINDOW->CancelTimeout();
		UI::POPUP_WINDOW->PreventClosing(true);
		Thread::sleep(50);

		switch (m_communicationType)
		{
		case CommunicationType::uart: {
			/* UART is too slow to support uploading files */
			if (contents.size() > MAX_UART_UPLOAD_SIZE)
			{
				warn("File too large (%u) to upload via UART, limit is %u", contents.size(), MAX_UART_UPLOAD_SIZE);
				UI::CONSOLE->AddResponse(LANGUAGEMANAGER->getValue("file_too_large_uart").c_str());
				UI::POPUP_WINDOW->Open();
				UI::POPUP_WINDOW->SetTitle(LANGUAGEMANAGER->getValue("file_too_large_uart").c_str());
				UI::POPUP_WINDOW->SetText(filename);
				return false;
			}

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
				UI::POPUP_WINDOW->SetProgress((int)((100 * prevPosition / contents.size())));
			}
			SendGcode("M29");
			break;
		}
		case CommunicationType::network: {
			registerDelayedCallback("upload_file_progress", 1000, []() {
				static int s_progress = 0;
				if (s_progress >= 90)
					return false;
				UI::POPUP_WINDOW->SetProgress(s_progress);
				s_progress += 10;
				return true;
			});
			RestClient::Response r;
			QueryParameters_t query;
			query["name"] = filename;
			if (!Post("/rr_upload", r, query, contents))
			{
				UI::CONSOLE->AddResponse(
					utils::format("HTTP error %d %s: Failed to upload file: %s", r.code, r.body, filename).c_str());
				unregisterDelayedCallback("upload_file_progress");
				UI::POPUP_WINDOW->Open();
				UI::POPUP_WINDOW->SetTitle(LANGUAGEMANAGER->getValue("upload_failed").c_str());
				UI::POPUP_WINDOW->SetText(r.body.c_str());
				return false;
			}
			break;
		}
		default:
			break;
		}
		UI::POPUP_WINDOW->Open();
		UI::POPUP_WINDOW->SetTitle(LANGUAGEMANAGER->getValue("finished_uploading").c_str());
		UI::POPUP_WINDOW->SetText(filename);
		UI::POPUP_WINDOW->SetProgress(100);
		return true;
	}

	bool Duet::DownloadFile(const char* filename, std::string& contents)
	{
		info("Downloading file %s", filename);
		switch (m_communicationType)
		{
		case CommunicationType::network: {
			RestClient::Response r;
			QueryParameters_t query;
			query["name"] = filename;
			if (!Get("/rr_download", r, query))
			{
				UI::CONSOLE->AddResponse(
					utils::format("HTTP error %d: Failed to download file: %s", r.code, filename).c_str());
				return false;
			}
			contents = r.body;
			break;
		}
		default:
			warn("Communication type not supported for downloading files");
			return false;
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
	    dbg("for %s", filename);
		switch (m_communicationType)
		{
		case CommunicationType::uart:
			SendGcodef("M36 \"%s\"", filename);
			break;
		case CommunicationType::network: {
			JsonDecoder decoder;
			QueryParameters_t query;
			query["name"] = filename;

#if 1
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
#if 0
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
						thumbnail.meta.width = thumbnailsJson[i]["width"].asInt();

						if (!thumbnailsJson[i].isMember("height"))
						{
							continue;
						}
						thumbnail.meta.height = thumbnailsJson[i]["height"].asInt();

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
						if (!thumbnail.meta.SetImageFormat(format.c_str()))
						{
							warn("Unsupported thumbnail format: %s", format.c_str());
							continue;
						}
						thumbnail.image.New(thumbnail.meta, filename);

						info("File %s has thumbnail %d: %dx%d", filename, i, thumbnail.width, thumbnail.height);

						QueryParameters_t query;
						query["name"] = filename;
						while (context.next != 0)
						{
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
								ThumbnailBuf data;
								data.size = std::min(body["data"].asString().size(), sizeof(data.buffer));
								memcpy(data.buffer, body["data"].asString().c_str(), data.size);
								ThumbnailDecodeChunk(thumbnail, data);
							}
						}
						thumbnail.Close();
						OM::FileSystem::GetListView()->refreshListView();
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
		dbg("for %s, offset=%u", filename, offset);
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
		default:
		    break;
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
			info("Connecting to Duet at %s", GetBaseUrl().c_str());

			RestClient::Response r;
			QueryParameters_t query;
			query["password"] = std::string("\"") + m_password + "\"";
			if (useSessionKey)
				query["sessionKey"] = "yes";

			return Comm::AsyncGet(
				GetBaseUrl(),
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
			if (m_sessionKey == sm_noSessionKey)
			{
				Reset();
				return 0;
			}
			RestClient::Response r;
			QueryParameters_t query;
			if (!Comm::Get(GetBaseUrl(), "/rr_disconnect", r, query, m_sessionKey))
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

	const std::string& Duet::GetBaseUrl() const
	{
		if (!m_ipAddress.empty())
		{
			dbg("Using IP address %s", m_ipAddress.c_str());
			return m_ipAddress;
		}
		dbg("Using hostname %s", m_hostname.c_str());
		return m_hostname;
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
		StoragePreferences::putInt(ID_DUET_BAUD_RATE, baudRate.internal);
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
		// if (m_communicationType == CommunicationType::network)
		// 	Disconnect();

		// StoragePreferences::putString("ip_address", ipAddress);
		m_ipAddress = ipAddress;
	}

	void Duet::ClearIPAddress()
	{
		m_ipAddress.clear();
		dbg("IP address cleared \"%s\"", m_ipAddress.c_str());
	}

	void Duet::SetHostname(const std::string hostname)
	{
		dbg("Hostname = %s", hostname.c_str());
		StoragePreferences::putString(ID_DUET_HOSTNAME, hostname);
		m_hostname.clear();

		if (hostname.find("http://") != 0)
		{
			m_hostname += "http://";
		}

		m_hostname += hostname.c_str();
		ClearIPAddress();
		info("Set Duet hostname to %s", m_hostname.c_str());
		if (m_communicationType == CommunicationType::network)
			Connect();
	}

	void Duet::SetPassword(const std::string& password)
	{
		StoragePreferences::putString(ID_DUET_PASSWORD, password);
		m_password = password;
	}

	void Duet::SetSessionKey(const uint32_t key)
	{
		m_sessionKey = key;
		info("Set Duet session key = %u", m_sessionKey);
	}
} // namespace Comm
