#pragma once
#include "Core.h"
#include "Log.h"
#include "Mathcell.h"
#include "LinearAllocator.h"

#include <SDL2/SDL.h>

#include <stdbool.h>

//A structure that saves the state of input devices in a given moment.
typedef struct InputSnapshot_t 
{
	uint8_t* keyboardstate;
	SDL_Keymod mods;
	uint32_t mouseButtons;
	int mouseX, mouseY;
} InputSnapshot_t;

//A utility function that creates an input snapshot.
InputSnapshot_t GetInput();

//Check if a key is pressed.
bool IsKeyPressed(const InputSnapshot_t* state, uint32_t sdlk);
//Check if a mouse button is pressed.
bool IsMouseButtonPressed(const InputSnapshot_t* state, uint32_t button);
//Get the position of the mouse.
ivec2 GetMousePos(const InputSnapshot_t* state);

//The event handler function signature.
typedef bool(*EventFun_t)(SDL_Event*, void*);

typedef struct EventDispatcher_t
{
	SDL_Event e; //The actual sdl event.
	bool handled; //Whether the event has been handled. stops propagation once handled.
} EventDispatcher_t;

bool GetEvent(EventDispatcher_t* e);
bool DispatchEvent(EventDispatcher_t* dispatcher, uint32_t type, EventFun_t handler, void* userdata);
/*
* Push an event into SDL's event queue.
* window: the window associated with the event. May be NULL.
* eventtype: the value obtained via SDL_RegisterEvents. identifies the type of the event.
* code: user defined value, may contain additional event type information.
* eventdata: the data to be copied into the user event memory. Use for stack allocated event structures. May be NULL. 
*	A pointer to the copied data will be written to the data1 field of the pushed event.
* datasize: the size of the data to copy. Ignored if eventdata is NULL.
* userdata2: a pointer to external data. contents will not be copied. can be acessed via the data2 field of the pushed event. May be NULL.
*/
int PushEvent(SDL_Window* window, uint32_t eventtype, int32_t code, void* eventdata, size_t datasize, void* userdata2);
/*
* Reset the memory used by pushed user events. call after the event handling loop each
* frame but before any functions that may push user events.
*/
void ResetUserEventMemory();
/*
* Free the memory reserved for user events. call at the end of the program.
*/
void ShutDownUserEvents();
