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

typedef struct SettingsScene 
{
	char dummy;
} SettingsScene;
