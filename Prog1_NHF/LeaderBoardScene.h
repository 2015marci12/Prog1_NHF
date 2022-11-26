#pragma once
#include "Core.h"
#include "Mathcell.h"
#include "Graphics.h"
#include "Container.h"
#include "ECS.h"
#include "Renderer2D.h"
#include "Input.h"
#include "Timer.h"

#include <SDL2/SDL.h>
#include <time.h>

typedef struct Score 
{
	char name[64];
	uint64_t Score;
	uint32_t Wave;
	uint64_t timePoint;
} Score;

typedef struct LeaderBoard 
{
	Score* scores;
	size_t scoreCount;
	size_t scoreCapacity;
	float scroll;

	SDL_Window* window;
	BitmapFont* font;
	TextureAtlas BG;

	bool GoBack;
} LeaderBoard;

LeaderBoard* InitLeaderBoard(LeaderBoard* l);
void CleanupLeaderBoard(LeaderBoard* l);

void SaveScoreToLeaderBoard(Score score);
void DeleteLeaderBoard();
