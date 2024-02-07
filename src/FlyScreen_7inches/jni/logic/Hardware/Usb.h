/*
 * Usb.h
 *
 *  Created on: 6 Feb 2024
 *      Author: andy
 */

#ifndef JNI_LOGIC_HARDWARE_USB_H_
#define JNI_LOGIC_HARDWARE_USB_H_

#include "Debug.h"
#include <dirent.h>
#include <string>
#include <vector>

namespace USB
{
	std::vector<dirent> ListEntriesInDirectory(const std::string& directoryPath);
	bool ReadFileContents(const std::string& filePath, std::string& contents);
} // namespace USB

#endif /* JNI_LOGIC_HARDWARE_USB_H_ */
