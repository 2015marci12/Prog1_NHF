#include "Input.h"

InputSnapshot_t GetInput()
{
	InputSnapshot_t ret;
	ret.keyboardstate = (uint8_t*)SDL_GetKeyboardState(NULL);
	ret.mods = SDL_GetModState();
	ret.mouseButtons = SDL_GetMouseState(&ret.mouseX, &ret.mouseY);
	return ret;
}

bool IsKeyPressed(const InputSnapshot_t* state, uint32_t sdlk)
{
	return state && state->keyboardstate[sdlk];
}

bool IsMouseButtonPressed(const InputSnapshot_t* state, uint32_t button)
{
	return state && (state->mouseButtons & SDL_BUTTON(button));
}

ivec2 GetMousePos(const InputSnapshot_t* state)
{
	return state ? new_ivec2(state->mouseX, state->mouseY) : new_ivec2_v(0);
}

bool GetEvent(EventDispatcher_t* e)
{
	e->handled = false;
	return SDL_PollEvent(&e->e);
}

bool DispatchEvent(EventDispatcher_t* dispatcher, uint32_t type, EventFun_t handler, void* userdata)
{
	ASSERT(dispatcher, "Cannot dispatch without a dispatcher.");
	if (!dispatcher->handled && type == dispatcher->e.type) dispatcher->handled |= handler(&dispatcher->e, userdata);
	return dispatcher->handled;
}
