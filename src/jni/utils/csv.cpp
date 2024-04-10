/*
 * csv.cpp
 *
 *  Created on: 10 Apr 2024
 *      Author: andy
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "csv.h"
#include <sstream>

namespace utils
{
	CSV::CSV(const std::string& csvContents, bool hasHeaders) : m_hasHeaders(hasHeaders)
	{
		size_t col = 0;
		size_t row = 0;
		bool inQuote = false;

		for (size_t i = 0; i < csvContents.size(); i++)
		{
			const char c = csvContents[i];
			const char n = i < csvContents.size() - 1 ? csvContents[i + 1] : 0;

			// Make sure we have enough rows and columns
			if (hasHeaders && row == 0)
			{
				if (m_headers.size() <= col)
				{
					m_headers.push_back("");
				}
			}
			else
			{
				if (m_data.size() < row)
				{
					m_data.push_back(std::vector<std::string>());
				}
				if (m_data[row - 1].size() <= col)
				{
					m_data[row - 1].push_back("");
				}
			}

			// Deal with escaped quotes
			if (inQuote && c == '"' && n == '"')
			{
				if (hasHeaders && row == 0)
				{
					m_headers[col] += c;
				}
				else
				{
					m_data[row - 1][col] += c;
				}
				i++;
				continue;
			}

			// Deal with start and end of a field
			if (c == '"')
			{
				inQuote = !inQuote;
				continue;
			}

			// Check if we can move on to the next field
			if (c == ',' && !inQuote)
			{
				col++;
				continue;
			}

			// Check if we can move on to the next row
			if (c == '\n' && !inQuote)
			{
				m_data.push_back(std::vector<std::string>());
				col = 0;
				row++;
				continue;
			}

			if (c == '\r' && !inQuote)
			{
				continue;
			}

			// Add the character to the current field
			if (hasHeaders && row == 0)
			{
				m_headers[col] += c;
			}
			else
			{
				m_data[row - 1][col] += c;
			}
		}

		if (m_data.size() > 0 && m_data[m_data.size() - 1].size() == 0)
		{
			m_data.pop_back();
		}
	}

	const std::vector<std::string>& CSV::GetHeaders() const
	{
		return m_headers;
	}

	bool CSV::GetCell(const std::string& header, size_t row, std::string& val)
	{
		if (!m_hasHeaders)
		{
			warn("No headers in CSV");
			return false;
		}

		for (size_t i = 0; i < m_headers.size(); i++)
		{
			if (m_headers[i] == header)
			{
				return GetCell(i, row, val);
			}
		}

		warn("Header \"%s\" not found", header.c_str());
		return false;
	}

	bool CSV::GetCell(size_t col, size_t row, std::string& val)
	{
		if (row >= m_data.size())
		{
			warn("Row %u out of range, rows=%u", row, m_data.size());
			return false;
		}

		if (col >= m_data[row].size())
		{
			warn("Column %u out of range, row[%u].size()=%u", col, row, m_data[row].size());
			return false;
		}

		val = m_data[row][col];
		return true;
	}

	bool CSV::GetCell(const std::string& header, size_t row, size_t& val)
	{
		std::string str;
		if (!GetCell(header, row, str))
		{
			return false;
		}

		val = strtoul(str.c_str(), NULL, 10);
		return true;
	}

	bool CSV::GetCell(size_t col, size_t row, size_t& val)
	{
		std::string str;
		if (!GetCell(col, row, str))
		{
			return false;
		}

		val = strtoul(str.c_str(), NULL, 10);
		return true;
	}

	bool CSV::GetCell(const std::string& header, size_t row, double& val)
	{
		std::string str;
		if (!GetCell(header, row, str))
		{
			return false;
		}

		val = strtod(str.c_str(), NULL);
		return true;
	}

	bool CSV::GetCell(size_t col, size_t row, double& val)
	{
		std::string str;
		if (!GetCell(col, row, str))
		{
			return false;
		}

		val = strtod(str.c_str(), NULL);
		return true;
	}

} // namespace utils
