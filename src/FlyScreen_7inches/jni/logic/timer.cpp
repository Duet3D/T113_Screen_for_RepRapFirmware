/*
 * timer.cpp
 *
 *  Created on: 30 Jan 2024
 *      Author: Andy Everitt
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG

#include "timer.h"
#include "Debug.h"
#include "utils/TimeHelper.h"
#include <system/Thread.h>
#include <vector>

static mainActivity* s_mainActivity = nullptr;

void initTimer(mainActivity* main)
{
    s_mainActivity = main;
}

void registerUserTimer(int id, int time)
{
	if (s_mainActivity == nullptr)
	{
		error("Cannot register user timer, mainActivity is null");
		return;
	}
	info("%d, %d", id, time);
	s_mainActivity->registerUserTimer(id, time);
}

void unregisterUserTimer(int id)
{
	if (s_mainActivity == nullptr)
	{
		error("Cannot unregister user timer, mainActivity is null");
		return;
	}
	info("%d", id);
	s_mainActivity->unregisterUserTimer(id);
}

void resetUserTimer(int id, int time)
{
	if (s_mainActivity == nullptr)
	{
		error("Cannot reset user timer, mainActivity is null");
		return;
	}
	info("%d, %d", id, time);
	s_mainActivity->resetUserTimer(id, time);
}

struct DelayedCallback
{
	DelayedCallback(const char* id, long long delay, function<bool()> callback)
		: id(id), delay(delay), callback(callback)
	{
		lastRun = TimeHelper::getCurrentTime();
	}

	const char* id;
	long long delay;
	long long lastRun;
	function<bool()> callback;
};

static std::vector<DelayedCallback> s_delayedCallbacks;

void registerDelayedCallback(const char* id, long long delay, function<bool()> callback)
{
	unregisterDelayedCallback(id);
	s_delayedCallbacks.push_back(DelayedCallback(id, delay, callback));
	info("Registered delayed callback %s", id);
}

void unregisterDelayedCallback(const char* id)
{
	size_t index = 0;
	for (auto& cb : s_delayedCallbacks)
	{
		if (strcmp(cb.id, id) == 0)
		{
			s_delayedCallbacks.erase(s_delayedCallbacks.begin() + index);
			info("Unregistered delayed callback %s", id);
			return;
		}
		index++;
	}
}

void runDelayedCallbacks()
{
	auto cb = s_delayedCallbacks.begin();
	while (cb != s_delayedCallbacks.end())
	{
		if (TimeHelper::getCurrentTime() - cb->lastRun >= cb->delay)
		{
			if (cb->callback())
			{
				cb->lastRun = TimeHelper::getCurrentTime();
				info("Delayed callback %s scheduled for %u", cb->id, cb->lastRun + cb->delay);
			}
			else
			{
				info("Unregistering delayed callback %s", cb->id);
				cb = s_delayedCallbacks.erase(cb);
				continue;
			}
		}
		++cb;
	}
}
