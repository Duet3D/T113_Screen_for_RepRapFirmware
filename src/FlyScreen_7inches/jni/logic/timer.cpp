/*
 * timer.cpp
 *
 *  Created on: 30 Jan 2024
 *      Author: Andy Everitt
 */


#include "timer.h"

static mainActivity* s_mainActivity = nullptr;

void initTimer(mainActivity* main)
{
    s_mainActivity = main;
}

void registerUserTimer(int id, int time)
{
    s_mainActivity->registerUserTimer(id, time);
}

void unregisterUserTimer(int id)
{
    s_mainActivity->unregisterUserTimer(id);
}

void resetUserTimer(int id, int time)
{
    s_mainActivity->resetUserTimer(id, time);
}
