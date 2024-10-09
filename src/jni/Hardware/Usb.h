/*
 * Usb.h
 *
 *  Created on: 6 Feb 2024
 *      Author: andy
 */

#ifndef JNI_HARDWARE_USB_H_
#define JNI_HARDWARE_USB_H_

#include "Debug.h"
#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include <vector>

namespace USB
{
	typedef struct
	{
		unsigned char d_type;	 /* file type */
		char d_name[256];		 /* file name */
		off_t st_size;			 /* total size, in bytes */
		blksize_t st_blksize;	 /* blocksize for file system I/O */
		blkcnt_t st_blocks;		 /* number of 512B blocks allocated */
		struct timespec st_atim; /* time of last access */
		struct timespec st_mtim; /* time of last modification */
		struct timespec st_ctim; /* time of last status change */
	} FileInfo;

	std::vector<FileInfo> ListEntriesInDirectory(const std::string& directoryPath);
	bool ReadUsbFileContents(const std::string& filePath, std::string& contents);
	bool ReadFileContents(const std::string& filePath, std::string& contents);
} // namespace USB

#endif /* JNI_HARDWARE_USB_H_ */
