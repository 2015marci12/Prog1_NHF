#pragma once
#include "Core.h"
#include "Log.h"

#include <SDL2/SDL.h>

//A simple timestamp.
typedef struct Timer_t 
{
	uint64_t startTime;
} Timer_t;

//Create a timer with the current time.
Timer_t MakeTimer();
//Create a timer with an offset in miliseconds from the current time.
Timer_t MakeTimer_Offset(int64_t ticksFromNow);
//Create a timer with an offset in seconds from the current time.
Timer_t MakeTimer_OffsetSec(float secondsFromNow);
//Get the number of milisecods elapsed since the timestamp in the timer.
int64_t GetElapsedMs(Timer_t timer);
//Get the number of seconds elapsed since the timestamp in the timer.
float GetElapsedSeconds(Timer_t timer);
