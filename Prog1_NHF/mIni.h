#pragma once
#include "Core.h"

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

/*
* Simple ini file parser.
* supports:
*	-	[sections].
*	-	comments that begin with either ; or #.
*	-	inline comments.
*	-	multiline values.
*/

#define M_INI_MAX_ID_LEN 256
#define M_INI_MAX_SECTION_LEN 256
#define M_INI_LINE 4096

//Handler function for each segment of data.
typedef int(*mIni_Handler_t)(const void* userdata, const char* section, const char* id, const char* value);

/*
* Get and parse an INI file from a FILE* stream.
*/
int mIni_Parse(FILE* file, mIni_Handler_t handler, const void* userdata);
/*
* Get and parse an INI file from a file on disk.
*/
int mIni_File(const char* file, mIni_Handler_t handler, const void* userdata);
