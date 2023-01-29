#ifndef _UTILS_LOG_H_
#define _UTILS_LOG_H_

#include <stdio.h>
#include <stdlib.h>

#ifdef USE_ANDROID_LOG
#include <android/log.h>

#define LOG_TAG "zkgui"
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#else
#define LOGE(fmt,...)	fprintf(stderr, fmt, ##__VA_ARGS__)
#define LOGD(fmt,...) 	fprintf(stderr, fmt, ##__VA_ARGS__)
#endif

#endif /* _UTILS_LOG_H_ */
