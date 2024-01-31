/*
 * timer.cpp
 *
 *  Created on: 30 Jan 2024
 *      Author: Andy Everitt
 */

#define DEBUG (1)

#include "timer.h"
#include "Debug.h"

static mainActivity* s_mainActivity = nullptr;

void initTimer(mainActivity* main)
{
    s_mainActivity = main;
}

void registerUserTimer(int id, int time)
{
	if (s_mainActivity == nullptr)
	{
		dbg("Cannot register user timer, mainActivity is null");
		return;
	}
	dbg("%d, %d", id, time);
	s_mainActivity->registerUserTimer(id, time);
}

void unregisterUserTimer(int id)
{
	if (s_mainActivity == nullptr)
	{
		dbg("Cannot unregister user timer, mainActivity is null");
		return;
	}
	dbg("%d", id);
	s_mainActivity->unregisterUserTimer(id);
}

void resetUserTimer(int id, int time)
{
	if (s_mainActivity == nullptr)
	{
		dbg("Cannot reset user timer, mainActivity is null");
		return;
	}
	dbg("%d, %d", id, time);
	s_mainActivity->resetUserTimer(id, time);
}
