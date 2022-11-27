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

//An enumeration of all texture assets.
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
	UPGRADE_TEX,
	HUD_TEX,
	BONUS_TEX,

	TEX_COUNT,
} TextureAssets;

//An enumeration of all animation assets.
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

//An enumeration of all particle systems.
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

//Teams.
typedef enum AlliegenceGroup
{
	NEUTRAL = 0,
	FRIENDLY = -1,
	ENEMY = -2,
} AlliegenceGroup;

//Collision masks.
#define COLLISIONMASK_WALL Layer_EVERYTHING
#define COLLISIONMASK_PLAYER (Layer_Walls | Layer_Bullets| Layer_Enemies | Layer_Missiles)
#define COLLISIONMASK_ENEMY (Layer_Walls | Layer_Bullets | Layer_Player | Layer_Missiles)
#define COLLISIONMASK_MISSILE (Layer_Walls | Layer_Player | Layer_Enemies | Layer_Bullets)
#define COLLISIONMASK_BULLET (Layer_Walls | Layer_Player | Layer_Enemies | Layer_Missiles)

//The parsed input data.
typedef struct InputState
{
	vec2 LookDir;
	vec2 MousePos;
	float Thrust;
	bool booster;
	bool firing;
	int selectedWeapon;
} InputState;

//A callback for the health component.
typedef void(*HealthIsZeroCallback_t)(entity_t, void*);

//The main game scene.
typedef struct Game 
{
	DiffConfig constants;
	uint64_t score;
	uint32_t Wave;
	uint32_t EnemyCount;
	bool GameOver;

	HealthIsZeroCallback_t GameOverCB;
	HealthIsZeroCallback_t EnemyDestroyedCB;
	HealthIsZeroCallback_t BonusEnemyDestroyedCB;

	Scene_t* scene;
	SDL_Window* window;

	BitmapFont* font;

	TextureAtlas Textures[TEX_COUNT];
	Animation Animations[ANIM_COUNT];
	ParticleSystem* Particles[PARTICLESYS_COUNT];
} Game;

//Initialize the game.
Game* InitGame(Game* game, SDL_Window* window);
//Release the resources owned by the game.
void CleanupGame(Game* game);

//Dispatch all events concerning the game.
void DispatchGameEvents(Game* game, EventDispatcher_t* dispatcher);
//Update the game world.
void UpdateGame(Game* game, float dt);
//Render the game to the screen.
void RenderGame(Game* game, Renderer2D* renderer);

//React to window resizes.
bool GameResizeEvent(SDL_Event* e, void* userData);
//React to collisions.
bool GameOnCollision(SDL_Event* e, void* userData);
//React to mouse button releases.
bool GameOnMouseRelease(SDL_Event* e, void* userData);

//Parse the current input.
void GameParseInput(SDL_Window* win, InputSnapshot_t* snapshot, InputState* input);
//Update the camera position.
void GameUpdateCamera(Game* game, InputState* input);

//Render the parallax background.
void GameRenderBackground(Game* game, Renderer2D* renderer);
//Render the HUD.
void GameRenderGui(Game* game, Renderer2D* renderer);

//The callback that fires when the player dies.
void GameOverCallBack(entity_t player, void* game);
//The callback that fires when a normal enemy dies.
void EnemyDestroyedCallBack(entity_t enemy, void* game);
//The callback that fires when an enemy that might spawn a bonus dies.
void BonusEnemyDestroyedCallBack(entity_t enemy, void* game);

//Spawn the next wave of enemies.
void SpawnNextWave(Game* game);
