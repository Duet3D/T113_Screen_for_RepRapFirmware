/*
 * Debug.h
 *
 *  Created on: Nov 19, 2023
 *      Author: Loïc
 */

#ifndef JNI_LOGIC_DEBUG_HPP_
#define JNI_LOGIC_DEBUG_HPP_

#if DEBUG
# include "utils/Log.h"
#define dbg(fmt, args...)		do { LOGD("%s(%d): " fmt , __FUNCTION__, __LINE__, ##args); } while(0)

#else
# define dbg(fmt, args...)		do {} while(0)

#endif

#endif /* JNI_LOGIC_DEBUG_HPP_ */
