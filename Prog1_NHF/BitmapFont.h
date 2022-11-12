#pragma once
#include "Core.h"
#include "Log.h"
#include "Animation.h"

typedef struct BitmapFont 
{
	bool invertY;
	vec2 CellFactors;
	float advanceFactor;
	uint32_t rowPitch;
	char baseCharOffset;
	char charWidths[256];
	GLTexture* FontTexture;
} BitmapFont;

BitmapFont* LoadBitmapFont(const char* filepath, bool invertY);
void DeleteBitmapFont(BitmapFont* font);

SubTexture FontGetChar(BitmapFont* font, float fontSize, char ch, vec2* size, float* advance);