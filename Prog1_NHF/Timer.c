#include "Timer.h"

Timer_t MakeTimer()
{
	Timer_t ret = { SDL_GetTicks64() };
	return ret;
}

uint64_t GetElapsedMs(Timer_t timer)
{
	uint64_t now = SDL_GetTicks64();
	return now - timer.startTime;
}

float GetElapsedSeconds(Timer_t timer)
{
	return (float)GetElapsedMs(timer) / 1000.f;
}
