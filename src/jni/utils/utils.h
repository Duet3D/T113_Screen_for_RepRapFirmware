/*
 * format.h
 *
 *  Created on: 26 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_UTILS_H_
#define JNI_LOGIC_UTILS_H_

#include <cstdarg>
#include <string>

namespace utils
{
	template <typename T>
	T bound(T value, T min, T max)
	{
		return value < min ? min : (value > max ? max : value);
	}
	std::string exec(const char* cmd);
	std::string format(const char* fmt, ...);
	std::string format(const char* fmt, va_list args);
	size_t removeCharFromString(std::string& nString, char c);
	void replaceSubstring(std::string& nString, const std::string& target, const std::string& replacement);
	std::vector<std::string> splitString(const std::string& str, const std::string& delimiter);
	size_t findInstance(const std::string& str, const std::string& target, size_t instance);
} // namespace utils

#endif /* JNI_LOGIC_UTILS_H_ */
