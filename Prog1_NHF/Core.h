#pragma once

#define NOMINMAX

//memory debugging.
#ifdef _DEBUG
#	include "debugmalloc.h"
#else
#define NO_LOG
#endif

#define SDL_MAIN_HANDLED
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>

#include "Log.h"
#include "Random.h"

#ifndef BIT
#	define BIT(x) (1ull << (x))
#endif 
