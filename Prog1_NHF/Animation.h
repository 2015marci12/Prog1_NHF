#pragma once
#include "Core.h"
#include "Log.h"
#include "Graphics.h"

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
