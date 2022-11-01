#include "Core.h"
#include "Mathcell.h"
#include "Graphics.h"
#include "Container.h"
#include "ECS.h"
#include "Renderer2D.h"

#include "GameScene.h"

#include <SDL2/SDL.h>
#include <stdio.h>

SDL_Window* window;
Game game;

//Handle window events.
bool OnWindowEvent(SDL_Event* e, void* userData) 
{
	//Translate pointers.
	SDL_WindowEvent* we = &e->window;

	//Handle resize.
	if (we->event == SDL_WINDOWEVENT_RESIZED) 
	{
		//Get width and height.
		int w = we->data1, h = we->data2;

		//Return if minimized.
		if (h <= 0 || w <= 0) return true; 

		//Set viewport.
		glViewport(0, 0, w, h);
	}

	return false;
}

int main(int argc, char* argv[])
{
	/* SDL inicializálása és ablak megnyitása */
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	window = SDL_CreateWindow("SDL peldaprogram",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		660, 480,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (window == NULL)
	{
		SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
		exit(1);
	}

	// Request an OpenGL 4.5 context (should be core)
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	// Also request a depth buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	//Init gl
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) exit(-1);

	// Use v-sync
	SDL_GL_SetSwapInterval(1);

	GLEnableDebugOutput();

	//Load game scene.
	InitGame(&game, window);

	Renderer2D renderer;
	Renderer2D_Init(&renderer);

	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	glViewport(0, 0, w, h);

	Timer_t timer = MakeTimer();

	EventDispatcher_t ev;
	bool exit = false;
	while (!exit)
	{
		//Events.
		while (GetEvent(&ev))
		{
			//Dispatch events.
			DispatchEvent(&ev, SDL_WINDOWEVENT, OnWindowEvent, NULL);
			DispatchGameEvents(&game, &ev);

			exit |= !ev.handled && ev.e.type == SDL_QUIT; //Exit once there is an unhandled QUIT event.
		}
		ResetUserEventMemory(); //Reset user event allocator.

		//Timing.
		float timediff = GetElapsedSeconds(timer);
		timer = MakeTimer();

		UpdateGame(&game, timediff);
		RenderGame(&game, &renderer);

		SDL_GL_SwapWindow(window);

		SDL_PumpEvents();
	}

	CleanupGame(&game);

	Renderer2D_Destroy(&renderer);
	SDL_GL_DeleteContext(glcontext);
	SDL_Quit();

	Tree_ResetPool();
	ShutDownUserEvents();

	return 0;
}