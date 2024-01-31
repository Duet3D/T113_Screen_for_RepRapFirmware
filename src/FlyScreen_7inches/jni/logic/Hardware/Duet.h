/*
 * Duet.h
 *
 *  Created on: 26 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_HARDWARE_DUET_H_
#define JNI_LOGIC_HARDWARE_DUET_H_

#include "../utils.h"
#include "curl/curl.h"
#include "restclient-cpp/restclient.h"

#include "Duet3D/General/String.h"
#include "Duet3D/General/StringRef.h"
#include "manager/ConfigManager.h"

namespace Comm
{
	constexpr uint32_t defaultPrinterPollInterval = 500;
	constexpr uint32_t printerResponseTimeout = 2000;

	constexpr size_t maxIpLength = 50;
	constexpr size_t maxHostnameLength = 64;

    constexpr const char* const duetCommunicationTypeNames[] = {
        "UART", "Network", "USB"};

	constexpr int baudRates[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};

	class Duet
	{
	  public:
		enum class CommunicationType
		{
			uart,
			network,
			usb,
			COUNT
		};

		typedef int32_t error_code;

		Duet();
		void Reset();

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

		void RequestModel(const char* flags = "d99f");
		void RequestModel(const char* key, const char* flags);
		void RequestFileList(const char* dir, const size_t first = 0);
		void RequestFileInfo(const char* filename);
		void RequestThumbnail();

		// UART methods
		void SetBaudRate(int baudRate);
		const int GetBaudRate() const { return m_baudRate; }

		// Network methods
		const error_code Connect();
		const error_code Disconnect();

		void SetIPAddress(const std::string& ipAddress);
		const std::string GetIPAddress() const;

		void SetHostname(const std::string& hostname);
		const std::string GetHostname() const;

		void SetPassword(const std::string& password);
		const std::string GetPassword() const;

		void SetSessionKey(const int32_t sessionKey);

		// USB methods

	  private:
		CommunicationType m_communicationType;
		std::string m_ipAddress;
		std::string m_hostname;
		std::string m_password;

		uint32_t m_pollInterval;
		int m_baudRate;
		int32_t m_sessionKey;
	};

	extern Duet duet;
} // namespace Comm

#endif /* JNI_LOGIC_HARDWARE_DUET_H_ */
