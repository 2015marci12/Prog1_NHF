#pragma once

//TODO using SDL_Mixer
//TODO credits

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

typedef struct GlobalSettings 
{
	float masterVolume;
	float musicVolume;
	float effectsVolume;

	bool FullScreen;
	uint32_t ResolutionVariation;
} GlobalSettings;

GlobalSettings* GetGlobalSettings();
void SaveGlobalSettings();

uvec2 GetResolutionVariation(uint32_t variation);
uint32_t GetResolutionVariationCount();

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

typedef struct SettingsScene 
{
	CurrentSettings selected;
	bool GoBack;

	SDL_Window* window;
	BitmapFont* font;
	TextureAtlas BG;
	TextureAtlas HUD;
} SettingsScene;

SettingsScene* InitSettings(SettingsScene* s, SDL_Window* window);
void CleanupSettings(SettingsScene* s);

bool SettingsKeyDown(SDL_Event* e, void* data);
bool SettingsMouseDown(SDL_Event* e, void* data);

void DispatchSettingsEvents(EventDispatcher_t* ev, SettingsScene* s);

void RenderSettings(SettingsScene* s, Renderer2D* renderer);
