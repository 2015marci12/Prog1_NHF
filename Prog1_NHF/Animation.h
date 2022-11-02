#pragma once
#include "Core.h"
#include "Log.h"
#include "Graphics.h"
#include "mIni.h"

typedef struct TextureAtlas 
{
	GLTexture* texture;
	uvec2 unitSizePx;
	vec2 unitSizeCoords;
} TextureAtlas;

typedef struct SubTexture 
{
	GLTexture* texture;
	Rect texRect;
} SubTexture;

TextureAtlas TextureAtlas_create(GLTexture* tex, uvec2 unit);
SubTexture TextureAtlas_SubTexture(TextureAtlas* atlas, uvec2 tile, uvec2 size);
SubTexture SubTexture_empty();

#define MAX_ANIMATION_FRAMES 32

typedef struct Animation 
{
	float frameTime;
	uint32_t frameCount;
	SubTexture frames[MAX_ANIMATION_FRAMES];
} Animation;

SubTexture Animation_GetAt(Animation* animation, float time, float* overtime);
float Animaton_GetDuration(Animation* anim);
bool Animation_FromIni(const char* filename, Animation* anim, TextureAtlas* atlas);
