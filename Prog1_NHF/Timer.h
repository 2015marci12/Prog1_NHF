#pragma once
#include "Core.h"
#include "Log.h"

#include <SDL2/SDL.h>

typedef struct Timer_t 
{
	uint64_t startTime;
} Timer_t;


Timer_t MakeTimer();
Timer_t MakeTimer_Offset(int64_t ticksFromNow);
Timer_t MakeTimer_OffsetSec(float secondsFromNow);
int64_t GetElapsedMs(Timer_t timer);
float GetElapsedSeconds(Timer_t timer);
