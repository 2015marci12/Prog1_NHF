#pragma once
#include "Core.h"
#include "Mathcell.h"
#include "Graphics.h"
#include "Container.h"
#include "ECS.h"
#include "Renderer2D.h"
#include "Input.h"
#include "Timer.h"
#include "ParticleSystem.h"
#include "Components.h"
#include "DiffConfig.h"
#include "BitmapFont.h"

#include <SDL2/SDL.h>

typedef enum TextureAssets
{
	PLAYER_TEX,
	WEAPON_TEX,
	EXPLOSION_TEX,
	BG1_TEX,
	GROUND_TEX,
	SMOKE_TEX,
	FONT_TEX,
	ENEMIES_TEX,
	HUD_TEX,

	TEX_COUNT,
} TextureAssets;

typedef enum AnimationAssets 
{
	BOOSTER_ANIM,
	CANNON_ANIM,
	EXPLOSION_ANIM,
	MISSILE_ANIM,
	LIGHT_SMOKE_ANIM,
	HEAVY_SMOKE_ANIM,
	LIGHT_FIRE_ANIM,
	HEAVY_FIRE_ANIM,
	RADAR_ANIM,

	ANIM_COUNT,
} AnimationAssets;

typedef enum ParticleSystems
{
	PARTICLE_EXPLOSION,
	PARTICLE_BOOSTER,
	LIGHT_SMOKE_PARTICLES,
	HEAVY_SMOKE_PARTICLES,
	LIGHT_FIRE_PARTICLES,
	HEAVY_FIRE_PARTICLES,

	PARTICLESYS_COUNT,
} ParticleSystems;

//Collision layers.
typedef enum CollisionLayers
{
	Layer_Walls = 0x1,
	Layer_Player = 0x2,
	Layer_Enemies = 0x4,
	Layer_Bullets = 0x8,
	Layer_Missiles = 0x10,
	Layer_EVERYTHING = 0xFFFFFFFFFFFFFFFFull,
} CollisionLayer;

typedef enum AlliegenceGroup
{
	NEUTRAL = 0,
	FRIENDLY = -1,
	ENEMY = -2,
} AlliegenceGroup;

#define COLLISIONMASK_WALL Layer_EVERYTHING
#define COLLISIONMASK_PLAYER (Layer_Walls | Layer_Bullets| Layer_Enemies | Layer_Missiles)
#define COLLISIONMASK_ENEMY (Layer_Walls | Layer_Bullets | Layer_Player | Layer_Missiles)
#define COLLISIONMASK_MISSILE (Layer_Walls | Layer_Player | Layer_Enemies | Layer_Bullets)
#define COLLISIONMASK_BULLET (Layer_Walls | Layer_Player | Layer_Enemies | Layer_Missiles)

typedef struct InputState
{
	vec2 LookDir;
	float Thrust;
	bool booster;
	bool firing;
	int selectedWeapon;
} InputState;

typedef struct Game 
{
	DiffConfig constants;
	uint64_t score;
	uint32_t Wave;
	Scene_t* scene;
	SDL_Window* window;

	BitmapFont* font;

	TextureAtlas Textures[TEX_COUNT];
	Animation Animations[ANIM_COUNT];
	ParticleSystem* Particles[PARTICLESYS_COUNT];
} Game;

Game* InitGame(Game* game, SDL_Window* window);
void CleanupGame(Game* game);

void DispatchGameEvents(Game* game, EventDispatcher_t* dispatcher);
void UpdateGame(Game* game, float dt);
void RenderGame(Game* game, Renderer2D* renderer);

bool GameResizeEvent(SDL_Event* e, void* userData);
bool GameOnCollision(SDL_Event* e, void* userData);
void GameOnMouseRelease(SDL_Event* e, void* userData);

void GameParseInput(SDL_Window* win, InputSnapshot_t* snapshot, InputState* input);
void GameUpdateCamera(Game* game, InputState* input);

void GameRenderBackground(Game* game, Renderer2D* renderer);
void GameRenderGui(Game* game, Renderer2D* renderer);
