#pragma once
#include "Core.h"
#include "Mathcell.h"

#include <SDL2/SDL.h>

#include <stdbool.h>

typedef struct InputSnapshot_t 
{
	uint8_t* keyboardstate;
	SDL_Keymod mods;
	uint32_t mouseButtons;
	int mouseX, mouseY;
} InputSnapshot_t;

InputSnapshot_t GetInput();

bool IsKeyPressed(const InputSnapshot_t* state, uint32_t sdlk);
bool IsMouseButtonPressed(const InputSnapshot_t* state, uint32_t button);
ivec2 GetMousePos(const InputSnapshot_t* state);

typedef bool(*EventFun_t)(SDL_Event*, void*);

typedef struct EventDispatcher_t
{
	SDL_Event e;
	bool handled;
} EventDispatcher_t;

EventDispatcher_t GetEvent();
bool DispatchEvent(EventDispatcher_t* dispatcher, uint32_t type, EventFun_t handler, void* userdata);
