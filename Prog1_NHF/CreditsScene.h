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

typedef struct CreditsScene 
{
	bool GoBack;

	SDL_Window* window;
	BitmapFont* font;
	TextureAtlas BG;
	TextureAtlas HUD;
} CreditsScene;

CreditsScene* InitCredits(CreditsScene* c, SDL_Window* window);
void CleanupCredits(CreditsScene* c);

bool CreditsMouseDown(SDL_Event* e, void* data);
bool CreditsKeyDown(SDL_Event* e, void* data);

void DispatchCreditsEvents(EventDispatcher_t* ev, CreditsScene* c);

void RenderCredits(CreditsScene* c, Renderer2D* renderer);
