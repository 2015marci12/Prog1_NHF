#pragma once

#include "Core.h"
#include "Mathcell.h"
#include "Graphics.h"
#include "Container.h"
#include "ECS.h"
#include "Renderer2D.h"
#include "Input.h"
#include "Timer.h"

#include "LeaderBoardScene.h"

#include <SDL2/SDL.h>
#include <time.h>

//The settings in binary form as they are saved to disk.
typedef struct GlobalSettings 
{
	float masterVolume;
	float musicVolume;
	float effectsVolume;

	bool FullScreen;
	uint32_t ResolutionVariation;
} GlobalSettings;

//Load the settings from disk or initialize the file.
GlobalSettings* GetGlobalSettings();
//Save the current settings to disk.
void SaveGlobalSettings();

//Get the resolution denoted by the saved resolution variation.
uvec2 GetResolutionVariation(uint32_t variation);
//Get the number of resolutions supported.
uint32_t GetResolutionVariationCount();

//The currently selected setting.
typedef enum CurrentSettings 
{
	VOLUME_MASTER,
	VOLUME_MUSIC,
	VOLUME_EFFECTS,
	RESETLEADERBOARD,
	FULLSCREEN,
	RESOLUTION,
	SETTINGS_MAX,
} CurrentSettings;

//The scene where one can change the game's settings.
typedef struct SettingsScene 
{
	CurrentSettings selected;
	bool GoBack;

	SDL_Window* window;
	BitmapFont* font;
	TextureAtlas BG;
	TextureAtlas HUD;
} SettingsScene;

//Initialize the settings scene.
SettingsScene* InitSettings(SettingsScene* s, SDL_Window* window);
//Release the resources held by the settings scene.
void CleanupSettings(SettingsScene* s);

//Respond to keystrokes.
bool SettingsKeyDown(SDL_Event* e, void* data);
//Respond to clicks.
bool SettingsMouseDown(SDL_Event* e, void* data);

//Handle events.
void DispatchSettingsEvents(EventDispatcher_t* ev, SettingsScene* s);

//Render the settings scene.
void RenderSettings(SettingsScene* s, Renderer2D* renderer);
