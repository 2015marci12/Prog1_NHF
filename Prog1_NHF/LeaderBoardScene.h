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
	TextureAtlas HUD;

	bool GoBack;
} LeaderBoard;

LeaderBoard* InitLeaderBoard(LeaderBoard* l, SDL_Window* window);
void CleanupLeaderBoard(LeaderBoard* l);

bool LeaderBoardScroll(SDL_Event* e, void* data);
bool LeaderBoardKeyDown(SDL_Event* e, void* data);
bool LeaderBoardMouseDown(SDL_Event* e, void* data);

void DispatchLeaderBoardEvents(EventDispatcher_t* ev, LeaderBoard* l);

void SaveScoreToLeaderBoard(Score score);
void DeleteLeaderBoard();

void RenderLeaderBoard(LeaderBoard* l, Renderer2D* renderer);
