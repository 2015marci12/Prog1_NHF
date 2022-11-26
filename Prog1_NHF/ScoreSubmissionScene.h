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

#include "LeaderBoardScene.h"

typedef struct ScoreSubmissionScene 
{
	Score score;
	bool next;

	BitmapFont* font;
	TextureAtlas BG;
	SDL_Window* window;
} ScoreSubmissionScene;

ScoreSubmissionScene* InitScoreSubmission(ScoreSubmissionScene* s, uint64_t score, uint32_t Wave, SDL_Window* window);
void CleanupScoreSubmission(ScoreSubmissionScene* s);

bool ScoreSubmissionKeyDown(SDL_Event* e, void* data);
bool ScoreSubmissionTextInput(SDL_Event* e, void* data);

void DispatchEventsScoreSubmission(EventDispatcher_t* ev, ScoreSubmissionScene* s);

void RenderScoreSubmission(ScoreSubmissionScene* s, Renderer2D* renderer);
