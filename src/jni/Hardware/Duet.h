/*
 * Duet.h
 *
 *  Created on: 26 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_HARDWARE_DUET_H_
#define JNI_HARDWARE_DUET_H_

#include "utils.h"
#include "curl/curl.h"
#include "restclient-cpp/restclient.h"

#include "Comm/Network.h"
#include "Configuration.h"
#include "Duet3D/General/String.h"
#include "Duet3D/General/StringRef.h"
#include "manager/ConfigManager.h"
#include "termio.h"

namespace Comm
{
	constexpr const char* const duetCommunicationTypeNames[] = {"UART", "Network", "USB"};

	typedef struct
	{
		unsigned int rate;
		unsigned int internal;
	} baudrate_t;

	constexpr baudrate_t baudRates[] = {{1200, B1200},
										{2400, B2400},
										{4800, B4800},
										{9600, B9600},
										{19200, B19200},
										{38400, B38400},
										{57600, B57600},
										{115200, B115200},
										{230400, B230400},
										{460800, B460800},
										{921600, B921600}};

	class Duet
	{
	  public:
		enum class CommunicationType
		{
			none = -1,
			uart,
			network,
			usb,
			COUNT
		};

		typedef int32_t error_code;

		Duet();
		void Init();
		void Reset();
		void Reconnect();

		void SetCommunicationType(CommunicationType type);
		const CommunicationType GetCommunicationType() const { return m_communicationType; }
		void SetPollInterval(uint32_t interval);
		void ScalePollInterval(float scale);
		const uint32_t GetPollInterval() const { return m_pollInterval; }

		template <typename... Args>
		void SendGcodef(const char* fmt, Args... args)
		{
			SendGcode(utils::format(fmt, args...).c_str());
		}
		void SendGcode(const char* gcode);
		void RequestReply(RestClient::Response& r);
		void ProcessReply(const RestClient::Response& r);

		bool UploadFile(const char* filename, const std::string& contents);

		void RequestModel(const char* flags = "d99f");
		void RequestModel(const char* key, const char* flags);
		void RequestFileList(const char* dir, const size_t first = 0);
		void RequestFileInfo(const char* filename);
		void RequestThumbnail(const char* filename, uint32_t offset);

		// UART methods
		void SetBaudRate(const unsigned int baudRateCode);
		void SetBaudRate(const baudrate_t& baudRate);
		const baudrate_t& GetBaudRate() const { return m_baudRate; }

		// Network methods
		const bool Connect(bool useSessionKey = true);
		const error_code Disconnect();

		void SetIPAddress(const std::string& ipAddress);
		const std::string GetIPAddress() const { return m_ipAddress; }

		void SetHostname(const std::string& hostname);
		const std::string GetHostname() const { return m_hostname; }

		void SetPassword(const std::string& password);
		const std::string GetPassword() const { return m_password; }

		void SetSessionKey(const uint32_t sessionKey);

		// USB methods

	  private:
		bool AsyncGet(const char* subUrl,
					  QueryParameters_t& queryParameters,
					  function<bool(RestClient::Response&)> callback,
					  bool queue = false);
		bool Get(const char* subUrl, RestClient::Response& r, QueryParameters_t& queryParameters);
		bool Post(const char* subUrl,
				  RestClient::Response& r,
				  QueryParameters_t& queryParameters,
				  const std::string& data);

		CommunicationType m_communicationType;
		std::string m_ipAddress;
		std::string m_hostname;
		std::string m_password;
		int32_t m_sessionTimeout;
		long long m_lastRequestTime;
		uint32_t m_sessionKey;

		uint32_t m_pollInterval;
		baudrate_t m_baudRate;

		static constexpr uint32_t noSessionKey = 0;
	};

	extern Duet duet;
} // namespace Comm

#endif /* JNI_HARDWARE_DUET_H_ */
