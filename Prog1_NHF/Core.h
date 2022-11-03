#pragma once

#define NOMINMAX

//memory debugging.
#ifdef _DEBUG
#	include "debugmalloc.h"
#endif

#define SDL_MAIN_HANDLED

#include <stdlib.h>

#include "Log.h"
#include "Random.h"

#ifndef BIT
#	define BIT(x) (1ull << (x))
#endif 
