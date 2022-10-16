#pragma once

#define NOMINMAX

#ifdef _DEBUG
//memory debugging.
#include "debugmalloc.h"
#endif

#define SDL_MAIN_HANDLED

#include <stdlib.h>

#include "Log.h"