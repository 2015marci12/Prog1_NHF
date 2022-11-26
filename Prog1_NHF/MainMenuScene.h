#pragma once
#include "Core.h"
#include "Mathcell.h"
#include "Graphics.h"
#include "Container.h"
#include "ECS.h"
#include "Renderer2D.h"
#include "Input.h"
#include "Timer.h"

typedef enum MenuOptions 
{
	STARTGAME,
	LEADERBOARD,
	SETTINGS,
	CREDITS,
	QUIT,
} MenuOptions;

typedef struct MainMenu 
{
	MenuOptions selected;
	bool clicked;

	SDL_Window* window;
	BitmapFont* font;
} MainMenu;

static void InitMenu() {};