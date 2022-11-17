#include "Animation.h"

TextureAtlas TextureAtlas_create(GLTexture* tex, uvec2 unit)
{
	TextureAtlas ret =
	{
		tex,
		unit,
		vec2_Div(
			uvec2_to_vec2(unit),
			uvec2_to_vec2(
				new_uvec2_v3(tex->Size)
			)
		) };
	return ret;
}

SubTexture TextureAtlas_SubTexture(TextureAtlas* atlas, uvec2 tile, uvec2 size)
{
	SubTexture ret =
	{
		atlas->texture,
		new_Rect_ps(
			vec2_Mul(atlas->unitSizeCoords, uvec2_to_vec2(tile)),
			vec2_Mul(atlas->unitSizeCoords, uvec2_to_vec2(size))
		)
	};
	return ret;
}

SubTexture SubTexture_empty()
{
	SubTexture ret = { NULL, new_Rect(0, 0, 1, 1) };
	return ret;
}

SubTexture SubTexture_Flip(SubTexture tex, bool x, bool y)
{
	if (x) 
	{
		tex.texRect.x += tex.texRect.w;
		tex.texRect.w *= -1.f;
	}
	if (y)
	{
		tex.texRect.y += tex.texRect.h;
		tex.texRect.h *= -1.f;
	}
	return tex;
}

SubTexture Animation_GetAt(Animation* animation, float time, float* overtime)
{
	if (animation && animation->frameCount)
	{
		uint32_t frame = ((uint32_t)(time / animation->frameTime)) % animation->frameCount;
		if (overtime) *overtime = time - (animation->frameTime * animation->frameCount);
		return animation->frames[frame];
	}
	return SubTexture_empty();
}

float Animaton_GetDuration(Animation* anim)
{
	if (anim)
	{
		return anim->frameCount * anim->frameTime;
	}
	return 0.f;
}

typedef struct AnimIniLoadTemp 
{
	float frameTime;
	uint32_t frameCount;
	struct { uvec2 tile, size; } frames[MAX_ANIMATION_FRAMES];
} AnimIniLoadTemp;

static int iniHandler(const void* user, const char* section, const char* name,
	const char* value)
{
	AnimIniLoadTemp* data = (AnimIniLoadTemp*)user;
	
	if (strcmp(section, "anim") == 0) 
	{
		if (strcmp(name, "frameTime") == 0) { data->frameTime = (float)atof(value); }
		else if (strcmp(name, "frameCount") == 0) { data->frameCount = atoi(value); }
		else
		{
			ERROR("Failed animation INI parse: unknown name %s\n", name);
			return -1;
		}
	}
	else 
	{
		uint32_t frameNum = -1;
		if ((sscanf_s(section, "frame%d", &frameNum) > 0)
			&& (frameNum != -1)
			&& (frameNum < data->frameCount))
		{
			if (strcmp(name, "x") == 0) { data->frames[frameNum].tile.x = atoi(value); }
			else if (strcmp(name, "y") == 0) { data->frames[frameNum].tile.y = atoi(value); }
			else if (strcmp(name, "w") == 0) { data->frames[frameNum].size.x = atoi(value); }
			else if (strcmp(name, "h") == 0) { data->frames[frameNum].size.y = atoi(value); }
			else 
			{
				ERROR("Failed animation INI parse: unknown name %s\n", name);
				return -1;
			}
		}
		else
		{
			ERROR("Failed animation INI parse: unknown section name %s\n", section);
			return -1;
		};
	}
	return 0;
}

bool Animation_FromIni(const char* filename, Animation* anim, TextureAtlas* atlas)
{
	AnimIniLoadTemp temp;
	if(mIni_File(filename, iniHandler, &temp) < 0) return false;

	anim->frameCount = temp.frameCount;
	anim->frameTime = temp.frameTime;
	for(uint32_t i = 0; i < MAX_ANIMATION_FRAMES; i++) 
	{
		anim->frames[i] = (i < temp.frameCount && temp.frames[i].size.x != 0) ?
			TextureAtlas_SubTexture(atlas, temp.frames[i].tile, temp.frames[i].size) :
			SubTexture_empty();
	}
	return true;
}
