#include "Animation.h"

TextureAtlas TextureAtlas_create(GLTexture* tex, uvec2 unit)
{
	TextureAtlas ret =
	{
		tex,
		unit,
		vec2_Div(
			uvec2_to_vec2(
				new_uvec2_v3(tex->Size)
			),
			uvec2_to_vec2(unit)
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