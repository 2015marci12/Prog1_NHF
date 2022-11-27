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

/*
* The scene that credits every resource used in the game to its author.
*/
typedef struct CreditsScene 
{
	bool GoBack;

	SDL_Window* window;
	BitmapFont* font;
	TextureAtlas BG;
	TextureAtlas HUD;
} CreditsScene;

//Initialize and load the scene.
CreditsScene* InitCredits(CreditsScene* c, SDL_Window* window);
//Release the resources held by the scene.
void CleanupCredits(CreditsScene* c);

//Respond to mouse clicks.
bool CreditsMouseDown(SDL_Event* e, void* data);
//Respond to keystrokes.
bool CreditsKeyDown(SDL_Event* e, void* data);

//Handle incoming events.
void DispatchCreditsEvents(EventDispatcher_t* ev, CreditsScene* c);

//Render the scene.
void RenderCredits(CreditsScene* c, Renderer2D* renderer);
