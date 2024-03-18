/*
 * format.h
 *
 *  Created on: 26 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_UTILS_H_
#define JNI_LOGIC_UTILS_H_

#include <string>

namespace utils
{
	template <typename T>
	T bound(T value, T min, T max)
	{
		return value < min ? min : (value > max ? max : value);
	}
	std::string format(const char* fmt, ...);
	std::string format(const char* fmt, va_list args);
	size_t removeCharFromString(std::string& nString, char c);
	void replaceSubstring(std::string& nString, const std::string& target, const std::string& replacement);
} // namespace utils

#endif /* JNI_LOGIC_UTILS_H_ */
