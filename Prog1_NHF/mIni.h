#pragma once
#include "Core.h"

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#define M_INI_MAX_ID_LEN 256
#define M_INI_MAX_SECTION_LEN 256
#define M_INI_LINE 4096

typedef int(*mIni_Handler_t)(const void* userdata, const char* section, const char* id, const char* value);

int mIni_Parse(FILE* file, mIni_Handler_t handler, const void* userdata);
int mIni_File(const char* file, mIni_Handler_t handler, const void* userdata);
