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

//The currently selected option in the main menu.
typedef enum MenuOptions 
{
	STARTGAME,
	LEADERBOARD,
	SETTINGS,
	CREDITS,
	QUIT,
	MenuOptions_MAX,
} MenuOptions;

//The main menu scene.
typedef struct MainMenu 
{
	MenuOptions selected;
	bool clicked;

	SDL_Window* window;
	BitmapFont* font;
	TextureAtlas BG;
} MainMenu;

//Initialize the main menu scene.
MainMenu* InitMenu(MainMenu* menu, SDL_Window* window);
//Release the resources owned by the main menu scene.
void CleanupMenu(MainMenu* menu);

//Respond to keystrokes.
bool MenuKeyDown(SDL_Event* e, const void* data);
//Respond to clicks.
bool MenuMouseDown(SDL_Event* e, const void* data);

//Handle events.
void MenuDispatchEvents(EventDispatcher_t* ev, MainMenu* menu);

//Render the main menu.
void RenderMenu(MainMenu* menu, Renderer2D* renderer);
