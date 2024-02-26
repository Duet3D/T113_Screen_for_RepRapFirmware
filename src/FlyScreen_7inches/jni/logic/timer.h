/*
 * timer.h
 *
 *  Created on: 30 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_TIMER_H_
#define JNI_LOGIC_TIMER_H_

#include "activity/mainActivity.h"
#include "std_fixed/functional.h"

constexpr int TIMER_UPDATE_DATA = 0;  // Id of the timer used to update values from data received from serial
constexpr int TIMER_DELAYED_TASK = 1; // Id of the timer used to run a task after a delay
constexpr int TIMER_ASYNC_HTTP_REQUEST = 2; // Id of the timer used to run queued async HTTP requests
constexpr int TIMER_THUMBNAIL = 3;			// Id of the timer used to request thumbnail data

void initTimer(mainActivity* main);
void registerUserTimer(int id, int time);
void unregisterUserTimer(int id);
void resetUserTimer(int id, int time);

void registerDelayedCallback(const char* id, long long delay, function<bool()> callback);
void unregisterDelayedCallback(const char* id);
void runDelayedCallbacks();

#endif /* JNI_LOGIC_TIMER_H_ */
