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

//The scene prompting the player for a name to save their score with.
typedef struct ScoreSubmissionScene 
{
	Score score; //The score to save.
	bool next;

	BitmapFont* font;
	TextureAtlas BG;
	SDL_Window* window;
} ScoreSubmissionScene;

//Initialize the score submission scene.
ScoreSubmissionScene* InitScoreSubmission(ScoreSubmissionScene* s, uint64_t score, uint32_t Wave, SDL_Window* window);
//Release the resources held by the score submission scene.
void CleanupScoreSubmission(ScoreSubmissionScene* s);

//Respond to keystrokes.
bool ScoreSubmissionKeyDown(SDL_Event* e, void* data);
//Respond to text input.
bool ScoreSubmissionTextInput(SDL_Event* e, void* data);

//Handle events.
void DispatchEventsScoreSubmission(EventDispatcher_t* ev, ScoreSubmissionScene* s);

//Render the scene.
void RenderScoreSubmission(ScoreSubmissionScene* s, Renderer2D* renderer);
