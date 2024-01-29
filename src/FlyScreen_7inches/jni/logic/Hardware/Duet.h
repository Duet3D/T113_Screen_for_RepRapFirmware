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
	constexpr uint32_t defaultPrinterPollInterval = 1000;
	static uint32_t printerPollInterval = defaultPrinterPollInterval;
	static uint32_t printerResponseTimeout = 2000;

	constexpr size_t maxIpLength = 50;
	constexpr size_t maxHostnameLength = 64;

	class Duet
	{
	  public:
		enum class CommunicationType
		{
			uart,
			network,
			usb,
		};

		enum BaudRate : unsigned int
		{
			b1200 = 1200,
			b2400 = 2400,
			b4800 = 4800,
			b9600 = 9600,
			b19200 = 19200,
			b38400 = 38400,
			b57600 = 57600,
			b115200 = 115200,
			b230400 = 230400,
			b460800 = 460800,
			b921600 = 921600,
		};

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
		void ProcessReply();

		void RequestModel(const char* flags = "d99f");
		void RequestModel(const char* key, const char* flags);
		void RequestFileList(const char* dir, const size_t first = 0);
		void RequestFileInfo(const char* filename);
		void RequestThumbnail();

		// UART methods
		void SetBaudRate(BaudRate baudRate);
		const int GetBaudRate() const;

		// Network methods
		const int Connect(const char* password = nullptr);
		const int Disconnect();

		void SetIPAddress(const char* ipAddress) { m_ipAddress.copy(ipAddress); }
		const StringRef GetIPAddress() const;

		void SetHostname(const char* hostname);
		const StringRef GetHostname() const;

		// USB methods

	  private:
		const RestClient::Response* RequestReply();

		CommunicationType m_communicationType;
		String<maxIpLength> m_ipAddress;
		String<maxHostnameLength> m_hostname;

		uint32_t m_pollInterval;
		BaudRate m_baudRate;
		RestClient::Response m_reply;
		int m_sessionKey;
	};

	extern Duet duet;
} // namespace Comm

#endif /* JNI_LOGIC_HARDWARE_DUET_H_ */
