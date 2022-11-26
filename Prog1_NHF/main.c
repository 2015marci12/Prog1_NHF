#include "Core.h"
#include "Mathcell.h"
#include "Graphics.h"
#include "Container.h"
#include "ECS.h"
#include "Renderer2D.h"

#include "GameScene.h"
#include "MainMenuScene.h"
#include "LeaderBoardScene.h"
#include "ScoreSubmissionScene.h"

#include <SDL2/SDL.h>
#include <stdio.h>

SDL_Window* window;
Game game;
MainMenu menu;
LeaderBoard leaderboard;
ScoreSubmissionScene scoresubmit;
uint64_t latestScore;
uint32_t latestWave;

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

typedef enum CurrentScene 
{
	SCENE_NONE = 0,
	SCENE_GAME,
	SCENE_MAINMENU,
	SCENE_LEADERBOARD,
	SCENE_SETTINGS,
	SCENE_CREDITS,
	SCENE_SCORESUBMIT,
} CurrentScene;
CurrentScene currentScene = SCENE_NONE;
CurrentScene nextScene = SCENE_NONE;

void SwitchScenes(CurrentScene newScene) 
{
	//Destroy Previous.
	switch (currentScene)
	{
	case SCENE_GAME:
		CleanupGame(&game);
		break;
	case SCENE_MAINMENU:
		CleanupMenu(&menu);
		break;
	case SCENE_LEADERBOARD:
		CleanupLeaderBoard(&leaderboard);
		break;
	case SCENE_SETTINGS:
		break;
	case SCENE_CREDITS:
		break;
	case SCENE_SCORESUBMIT:
		CleanupScoreSubmission(&scoresubmit);
		break;
	default:
		break;
	}

	//Init Next.
	currentScene = newScene;
	switch (currentScene)
	{
	case SCENE_GAME:
		InitGame(&game, window);
		break;
	case SCENE_MAINMENU:
		InitMenu(&menu, window);
		break;
	case SCENE_LEADERBOARD:
		InitLeaderBoard(&leaderboard);
		break;
	case SCENE_SETTINGS:
		break;
	case SCENE_CREDITS:
		break;
	case SCENE_SCORESUBMIT:
		InitScoreSubmission(&scoresubmit, latestScore, latestWave, window);
		break;
	default:
		break;
	}
}

void Frame(bool* exit, Timer_t* timer, Renderer2D* renderer) 
{
	EventDispatcher_t ev;

	//Events.
	while (GetEvent(&ev))
	{
		//Dispatch events.
		DispatchEvent(&ev, SDL_WINDOWEVENT, OnWindowEvent, NULL);

		//Dispatch scene specific events.
		switch (currentScene)
		{
		case SCENE_GAME:
			DispatchGameEvents(&game, &ev);
			break;
		case SCENE_MAINMENU:
			MenuDispatchEvents(&ev, &menu);
			break;
		case SCENE_LEADERBOARD:
			break;
		case SCENE_SETTINGS:
			break;
		case SCENE_CREDITS:
			break;
		case SCENE_SCORESUBMIT:
			DispatchEventsScoreSubmission(&ev, &scoresubmit);
			break;
		default:
			break;
		}

		*exit |= !ev.handled && ev.e.type == SDL_QUIT; //Exit once there is an unhandled QUIT event.
	}
	ResetUserEventMemory(); //Reset user event allocator.

	//Timing.
	float timediff = GetElapsedSeconds(*timer);
	*timer = MakeTimer();

	//Update and render the scene.
	switch (currentScene)
	{
	case SCENE_GAME:
		UpdateGame(&game, timediff);
		RenderGame(&game, renderer);

		//Switch scenes.
		if (game.GameOver)
		{
			//Save score for the submitting frame.
			latestScore = game.score;
			latestWave = game.Wave;
			nextScene = SCENE_SCORESUBMIT;
		}
		break;
	case SCENE_MAINMENU:
		RenderMenu(&menu, renderer);

		if (menu.clicked) 
		{
			switch (menu.selected)
			{
			case STARTGAME:
				nextScene = SCENE_GAME;
				break;
			case LEADERBOARD:
				nextScene = SCENE_LEADERBOARD;
				break;
			case SETTINGS:
				nextScene = SCENE_SETTINGS;
				break;
			case CREDITS:
				nextScene = SCENE_CREDITS;
				break;
			case QUIT:
				*exit = true;
				break;
			default:
				break;
			}
		}
		break;
	case SCENE_LEADERBOARD:
		break;
	case SCENE_SETTINGS:
		break;
	case SCENE_CREDITS:
		break;
	case SCENE_SCORESUBMIT:
		RenderScoreSubmission(&scoresubmit, renderer);

		if (scoresubmit.next) 
		{
			SaveScoreToLeaderBoard(scoresubmit.score);
			nextScene = SCENE_MAINMENU;
		}
		break;
	default:
		break;
	}
	
	SDL_GL_SwapWindow(window);

	SDL_PumpEvents();
}

int main(int argc, char* argv[])
{
	//Init RNG.
	Rand_Init();

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
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );

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
	SwitchScenes(SCENE_MAINMENU);

	//Init renderer.
	Renderer2D renderer;
	Renderer2D_Init(&renderer);

	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	glViewport(0, 0, w, h);

	Timer_t timer = MakeTimer();

	bool exit = false;
	while (!exit)
	{
		Frame(&exit, &timer, &renderer);
		if (nextScene != SCENE_NONE) SwitchScenes(nextScene);
		nextScene = SCENE_NONE;
	}	

	//Free the current scene's resources.
	SwitchScenes(SCENE_NONE);

	Renderer2D_Destroy(&renderer);
	SDL_GL_DeleteContext(glcontext);
	SDL_Quit();

	Tree_ResetPool();
	ShutDownUserEvents();

	return 0;
}