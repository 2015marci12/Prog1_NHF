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

//The result of a game with the player name.
typedef struct Score 
{
	char name[64];
	uint64_t Score;
	uint32_t Wave;
	uint64_t timePoint;
} Score;

//A scene that displays all locally stored scores.
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

//Initialize the leaderboard scene and load the leaderboard from disk.
LeaderBoard* InitLeaderBoard(LeaderBoard* l, SDL_Window* window);
//Release the resources owned by the leaderboard.
void CleanupLeaderBoard(LeaderBoard* l);

//Respond to scroll input.
bool LeaderBoardScroll(SDL_Event* e, void* data);
//Respond to keystrokes.
bool LeaderBoardKeyDown(SDL_Event* e, void* data);
//Respond to clicks.
bool LeaderBoardMouseDown(SDL_Event* e, void* data);

//Handle events.
void DispatchLeaderBoardEvents(EventDispatcher_t* ev, LeaderBoard* l);

//Save a score to the leaderboard file on disk.
void SaveScoreToLeaderBoard(Score score);
//Reset the leaderboard file on disk.
void DeleteLeaderBoard();

//Render the leaderboard scene.
void RenderLeaderBoard(LeaderBoard* l, Renderer2D* renderer);
