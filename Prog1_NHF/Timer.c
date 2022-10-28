#include "Timer.h"

Timer_t MakeTimer()
{
	Timer_t ret = { SDL_GetTicks64() };
	return ret;
}

Timer_t MakeTimer_Offset(int64_t ticksFromNow)
{
	Timer_t ret = { (uint64_t)((int64_t)SDL_GetTicks64() + ticksFromNow) };
	return ret;
}

Timer_t MakeTimer_OffsetSec(float secondsFromNow)
{
	Timer_t ret = { (uint64_t)((int64_t)SDL_GetTicks64() + (int64_t)(secondsFromNow * 1000ll)) };
	return ret;
}

int64_t GetElapsedMs(Timer_t timer)
{
	uint64_t now = SDL_GetTicks64();
	return (int64_t)now - (int64_t)timer.startTime;
}

float GetElapsedSeconds(Timer_t timer)
{
	return (float)GetElapsedMs(timer) / 1000.f;
}
