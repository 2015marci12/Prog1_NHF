#pragma once

//Logging and debug break fun.
#include "Core.h"

#include <stdio.h> 

#ifdef _MSC_VER
#	define DEBUG_BREAK() __debugbreak()
#elif __has_builtin(__builtin_debugtrap)
#	define DEBUG_BREAK() __builtin_debugtrap()
#else
#	include <assert.h>
#	define DEBUG_BREAK() assert(false)
#endif


#define VA_ARGS(...) , ##__VA_ARGS__

#define TRACE_STR(str) str
#define	INFO_STR(str) "\x1B[32m" str "\033[0m"
#define	WARN_STR(str) "\x1B[93m" str "\033[0m"
#define	ERROR_STR(str) "\x1B[91m" str "\033[0m"
#define	FATAL_STR(str) "\x1B[37,101m" str "\033[0m"

#ifndef NO_LOG
#	define TRACE(format, ...) printf(TRACE_STR(format) VA_ARGS(__VA_ARGS__))
#	define INFO(format, ...) printf(INFO_STR(format) VA_ARGS(__VA_ARGS__))
#	define WARN(format, ...) printf(WARN_STR(format) VA_ARGS(__VA_ARGS__))
#	define ERROR(format, ...) fprintf(stderr, ERROR_STR(format) VA_ARGS(__VA_ARGS__))
#	define FATAL(format, ...) fprintf(stderr, FATAL_STR(format) VA_ARGS(__VA_ARGS__))
#	define ASSERT(condition, ...) if(!condition) { FATAL(__VA_ARGS__); DEBUG_BREAK(); }
#else
#	define TRACE(format, ...)
#	define INFO(format, ...) 
#	define WARN(format, ...) 
#	define ERROR(format, ...)
#	define FATAL(format, ...)
#	define ASSERT(condition, ...)
#endif