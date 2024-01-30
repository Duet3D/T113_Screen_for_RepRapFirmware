/*
 * timer.h
 *
 *  Created on: 30 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_TIMER_H_
#define JNI_LOGIC_TIMER_H_

#include "activity/mainActivity.h"

constexpr int TIMER_UPDATE_DATA = 0;  // Id of the timer used to update values from data received from serial

void initTimer(mainActivity* main);
void registerUserTimer(int id, int time);
void unregisterUserTimer(int id);
void resetUserTimer(int id, int time);


#endif /* JNI_LOGIC_TIMER_H_ */
