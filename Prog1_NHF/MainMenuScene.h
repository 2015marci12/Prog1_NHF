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

typedef enum MenuOptions 
{
	STARTGAME,
	LEADERBOARD,
	SETTINGS,
	CREDITS,
	QUIT,
	MenuOptions_MAX,
} MenuOptions;

typedef struct MainMenu 
{
	MenuOptions selected;
	bool clicked;

	SDL_Window* window;
	BitmapFont* font;
	TextureAtlas BG;
} MainMenu;

MainMenu* InitMenu(MainMenu* menu, SDL_Window* window);
void CleanupMenu(MainMenu* menu);

bool MenuKeyDown(SDL_Event* e, const void* data);
bool MenuMouseDown(SDL_Event* e, const void* data);

void MenuDispatchEvents(EventDispatcher_t* ev, MainMenu* menu);

void RenderMenu(MainMenu* menu, Renderer2D* renderer);