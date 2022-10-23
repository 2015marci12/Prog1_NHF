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

LinearAllocator_t* userEventAllocator = NULL;

int PushEvent(SDL_Window* window, uint32_t eventtype, int32_t code, void* eventdata, size_t datasize, void* userdata2)
{
	void* data = NULL;
	if (eventdata) //Allocate space for the user defined event structure data if it exists.
	{
		if (!userEventAllocator) userEventAllocator = LinearAllocator_Create(4096u);
		data = LinearAllocator_Allocate(userEventAllocator, datasize);
		memcpy(data, eventdata, datasize);
	}

	SDL_Event e;
	e.type = eventtype;
	e.user.code = code;
	e.user.data1 = data;
	e.user.data2 = userdata2;
	e.user.timestamp = SDL_GetTicks();
	e.user.windowID = window ? SDL_GetWindowID(window) : 0;

	return SDL_PushEvent(&e); //Event queue may be full, or code may be wrong. in this case, an error is passed to the user.
}

void ResetUserEventMemory()
{
	LinearAllocator_Reset(userEventAllocator);
}

void ShutDownUserEvents()
{
	LinearAllocator_Destroy(userEventAllocator);
	userEventAllocator = NULL;
}
