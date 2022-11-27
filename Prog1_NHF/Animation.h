#pragma once
#include "Core.h"
#include "Log.h"
#include "Graphics.h"
#include "mIni.h"

/*
* A structure representing a texture atlas with multiple frames of the same size.
*/
typedef struct TextureAtlas 
{
	GLTexture* texture;
	uvec2 unitSizePx;
	vec2 unitSizeCoords;
} TextureAtlas;

/*
* A subsection of a larger texture.
*/
typedef struct SubTexture 
{
	GLTexture* texture;
	Rect texRect;
} SubTexture;

/*
* Creates a texture atlas from a parent texture given the size in pixels of a single subtexture.
*/
TextureAtlas TextureAtlas_create(GLTexture* tex, uvec2 unit);
/*
* Get one of the subtextures in a textureAtlas. tile specifies the starting coordinates in the grid,
* size is the extent of the subtexture divided by the frame dimensions.
*/
SubTexture TextureAtlas_SubTexture(TextureAtlas* atlas, uvec2 tile, uvec2 size);
/*
* Creates an empty subtexture. 
*/
SubTexture SubTexture_empty();
/*
* Flips a given subtexture along the given axis'.
*/
SubTexture SubTexture_Flip(SubTexture tex, bool x, bool y);

/*
* The max number of frames an animation can have.
*/
#define MAX_ANIMATION_FRAMES 32

/*
* A structure representing an animation.
*/
typedef struct Animation 
{
	float frameTime;
	uint32_t frameCount;
	SubTexture frames[MAX_ANIMATION_FRAMES];
} Animation;

/*
* Get the current frame of the animation given the time. This funcion will also calculate
* how long ago did the animation end, and will store that value in seconds to the given location in memory.
* If overtime is NULL, then that parameter is ignored.
*/
SubTexture Animation_GetAt(Animation* animation, float time, float* overtime);
/*
* Calculate the duration of an animation.
*/
float Animaton_GetDuration(Animation* anim);
/*
* Load an animation from an INI file on disk.
*/
bool Animation_FromIni(const char* filename, Animation* anim, TextureAtlas* atlas);
