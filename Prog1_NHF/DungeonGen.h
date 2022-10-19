#pragma once
#include "Core.h"
#include "Log.h"
#include "Graphics.h"
#include "Animation.h"
#include "Random.h"

typedef enum TileType
{
	Tile_WALL = 0,
	Tile_FLOOR = 1,
	Tile_CORRIDOR = 2,
} TileType;

typedef struct Tile 
{
	TileType type;
	uint32_t style;
	SubTexture TileTex;
	SubTexture TileDecal;
} Tile;

typedef struct Level 
{
	Tile* tiles;
	uvec2 extent;
	float scale;
} Level;

Level Generate(uint64_t seed) 
{
	MT64_Gen_t rand;
	init_genrand64(&rand, seed);

	//Allocate the level
	const uint32_t levelsize = 256u;
	Level level = { (Tile*)malloc(levelsize * levelsize * sizeof(Tile)), new_uvec2_v(levelsize), 5.f };

	//Fill with walls.
	for (int i = 0; i < levelsize * levelsize; i++) 
	{
		Tile* t = &level.tiles[i];
		t->style = 0;
		t->type = Tile_WALL;
		t->TileTex = SubTexture_empty();
		t->TileDecal = SubTexture_empty();
	}

	//Try creating rooms n times
	const uint32_t n = 32;
	for(int i = 0; i < n; i++) 
	{
		const uint32_t upperlimit = levelsize / 16;
		uvec2 roomsize = new_uvec2(
			(uint32_t)((double)upperlimit * Rand_double_2(&rand)),
			(uint32_t)((double)upperlimit * Rand_double_2(&rand))
			);

		uvec2 center = dvec2_to_uvec2(
			dvec2_Mul(
				new_dvec2(Rand_double_2(&rand), Rand_double_2(&rand)),
				uvec2_to_dvec2(uvec2_Sub(level.extent, roomsize))
			));

		//TODO
	}
}
