/*
 * format.cpp
 *
 *  Created on: 26 Jan 2024
 *      Author: Andy Everitt
 */

#include "utils.h"
#include <cstdarg>

namespace utils
{
	std::string format(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		std::string tmp = format(fmt, args);
		va_end(args);
		return tmp;
	}

	std::string format(const char* fmt, va_list args)
	{
		std::string tmp;
		int num = vsnprintf(0, 0, fmt, args);
		if (num >= (int)tmp.capacity())
			tmp.reserve(num + sizeof(char));
		tmp.resize(num);
		vsnprintf((char*)tmp.data(), tmp.capacity(), fmt, args);
		return tmp;
	}

	size_t removeCharFromString(std::string& nString, char c)
	{
		std::string::size_type pos;
		while (1)
		{
			pos = nString.find(c);
			if (pos != std::string::npos)
			{
				nString.erase(pos, 1);
			}
			else
			{
				break;
			}
		}
		return (size_t)nString.size();
	}

	void replaceSubstring(std::string& nString, const std::string& target, const std::string& replacement)
	{
		size_t position = nString.find(target); // Find the first occurrence of the target substring

		while (position != std::string::npos)
		{
			nString.replace(
				position, target.length(), replacement); // Replace the target substring with the replacement string
			position = nString.find(target, position + replacement.length()); // Find the next occurrence, if any
		}
	}
} // namespace utils
