/*
 * csv.h
 *
 *  Created on: 10 Apr 2024
 *      Author: andy
 */

#ifndef JNI_UTILS_CSV_H_
#define JNI_UTILS_CSV_H_

#include <string>
#include <vector>

namespace utils
{
	class CSV
	{
	  public:
		CSV(const std::string& csvContents, bool hasHeaders = true);
		~CSV() {}

		const std::vector<std::string>& GetHeaders() const;

		size_t GetRowCount() const;
		size_t GetColumnCount() const;

		bool GetCell(const std::string& header, size_t row, std::string& val) const;
		bool GetCell(size_t col, size_t row, std::string& val) const;

		bool GetCell(const std::string& header, size_t row, size_t& val) const;
		bool GetCell(size_t col, size_t row, size_t& val) const;

		bool GetCell(const std::string& header, size_t row, double& val) const;
		bool GetCell(size_t col, size_t row, double& val) const;

	  private:
		std::vector<std::string> m_headers;
		std::vector<std::vector<std::string>> m_data;
		bool m_hasHeaders;
	};
} // namespace utils

#endif /* JNI_UTILS_CSV_H_ */
