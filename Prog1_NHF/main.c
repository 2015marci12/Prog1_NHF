#include "Core.h"
#include "Mathcell.h"
#include "Graphics.h"
#include "Container.h"
#include "ECS.h"
#include "Renderer2D.h"

#include "SettingsScene.h"
#include "GameScene.h"
#include "MainMenuScene.h"
#include "LeaderBoardScene.h"
#include "ScoreSubmissionScene.h"
#include "CreditsScene.h"

#include <SDL2/SDL.h>
#include <stdio.h>

typedef struct ProgramData 
{
	SDL_Window* window;
	Game game;
	MainMenu menu;
	LeaderBoard leaderboard;
	ScoreSubmissionScene scoresubmit;
	SettingsScene settings;
	CreditsScene credits;
	uint64_t latestScore;
	uint32_t latestWave;
} ProgramData;


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

void SwitchScenes(ProgramData* data, CurrentScene newScene)
{
	//Destroy Previous.
	switch (currentScene)
	{
	case SCENE_GAME:
		CleanupGame(&data->game);
		break;
	case SCENE_MAINMENU:
		CleanupMenu(&data->menu);
		break;
	case SCENE_LEADERBOARD:
		CleanupLeaderBoard(&data->leaderboard);
		break;
	case SCENE_SETTINGS:
		CleanupSettings(&data->settings);
		break;
	case SCENE_CREDITS:
		CleanupCredits(&data->credits);
		break;
	case SCENE_SCORESUBMIT:
		CleanupScoreSubmission(&data->scoresubmit);
		break;
	default:
		break;
	}

	//Init Next.
	currentScene = newScene;
	switch (currentScene)
	{
	case SCENE_GAME:
		InitGame(&data->game, data->window);
		break;
	case SCENE_MAINMENU:
		InitMenu(&data->menu, data->window);
		break;
	case SCENE_LEADERBOARD:
		InitLeaderBoard(&data->leaderboard, data->window);
		break;
	case SCENE_SETTINGS:
		InitSettings(&data->settings, data->window);
		break;
	case SCENE_CREDITS:
		InitCredits(&data->credits, data->window);
		break;
	case SCENE_SCORESUBMIT:
		InitScoreSubmission(&data->scoresubmit, data->latestScore, data->latestWave, data->window);
		break;
	default:
		break;
	}
}

void Frame(ProgramData* data, bool* exit, Timer_t* timer, Renderer2D* renderer) 
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
			DispatchGameEvents(&data->game, &ev);
			break;
		case SCENE_MAINMENU:
			MenuDispatchEvents(&ev, &data->menu);
			break;
		case SCENE_LEADERBOARD:
			DispatchLeaderBoardEvents(&ev, &data->leaderboard);
			break;
		case SCENE_SETTINGS:
			DispatchSettingsEvents(&ev, &data->settings);
			break;
		case SCENE_CREDITS:
			DispatchCreditsEvents(&ev, &data->credits);
			break;
		case SCENE_SCORESUBMIT:
			DispatchEventsScoreSubmission(&ev, &data->scoresubmit);
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
		UpdateGame(&data->game, timediff);
		RenderGame(&data->game, renderer);

		//Switch scenes.
		if (data->game.GameOver)
		{
			//Save score for the submitting frame.
			data->latestScore = data->game.score;
			data->latestWave = data->game.Wave;
			nextScene = SCENE_SCORESUBMIT;
		}
		break;
	case SCENE_MAINMENU:
		RenderMenu(&data->menu, renderer);

		if (data->menu.clicked)
		{
			switch (data->menu.selected)
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
		RenderLeaderBoard(&data->leaderboard, renderer);

		if (data->leaderboard.GoBack) nextScene = SCENE_MAINMENU;

		break;
	case SCENE_SETTINGS:
		RenderSettings(&data->settings, renderer);

		if(data->settings.GoBack) nextScene = SCENE_MAINMENU;
		break;
	case SCENE_CREDITS:
		RenderCredits(&data->credits, renderer);

		if (data->credits.GoBack) nextScene = SCENE_MAINMENU;
		break;
	case SCENE_SCORESUBMIT:
		RenderScoreSubmission(&data->scoresubmit, renderer);

		if (data->scoresubmit.next)
		{
			SaveScoreToLeaderBoard(data->scoresubmit.score);
			nextScene = SCENE_MAINMENU;
		}
		break;
	default:
		break;
	}
	
	SDL_GL_SwapWindow(data->window);

	SDL_PumpEvents();
}

int main(int argc, char* argv[])
{
	//Init RNG.
	Rand_Init();

	ProgramData data;

	/* SDL inicializálása és ablak megnyitása */
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	GlobalSettings* s = GetGlobalSettings();
	uvec2 Size = GetResolutionVariation(s->ResolutionVariation);

	data.window = SDL_CreateWindow("SDL peldaprogram",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		Size.x, Size.y,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | 
		((s->FullScreen) ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));

	if (data.window == NULL)
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
	SDL_GLContext glcontext = SDL_GL_CreateContext(data.window);
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) exit(-1);

	// Use v-sync
	SDL_GL_SetSwapInterval(1);

	GLEnableDebugOutput();

	//Load game scene.
	SwitchScenes(&data, SCENE_MAINMENU);

	//Init renderer.
	Renderer2D renderer;
	Renderer2D_Init(&renderer);

	int w, h;
	SDL_GetWindowSize(data.window, &w, &h);
	glViewport(0, 0, w, h);

	Timer_t timer = MakeTimer();

	bool exit = false;
	while (!exit)
	{
		Frame(&data, &exit, &timer, &renderer);
		if (nextScene != SCENE_NONE) SwitchScenes(&data, nextScene);
		nextScene = SCENE_NONE;
	}	

	//Free the current scene's resources.
	SwitchScenes(&data, SCENE_NONE);

	Renderer2D_Destroy(&renderer);
	SDL_GL_DeleteContext(glcontext);
	SDL_Quit();

	Tree_ResetPool();
	ShutDownUserEvents();

	return 0;
}