#pragma once
#include "Core.h"

#include "GameScene.h"

typedef struct PlaneComponent
{
	float liftcoeff;
	float dragcoeff;
	float thrust;
	float mass;
} PlaneComponent;

void RegisterPlane(Scene_t* scene);
void MovePlanes(Game* game, float dt);

typedef struct PlayerComponent
{
	float fuel;
	float max_fuel;
	Timer_t boosterParticleTimer;
	Timer_t shootingTimer;
	int selected_weapon;
} PlayerComponent;

void RegisterPlayer(Scene_t* scene);
void UpdatePlayer(Game* game, InputState* input, float dt);

typedef struct HealthComponent
{
	float health;
	float max_health;
	float invincibility_time;
	Timer_t lastHit;
	Timer_t lastParticle;
	uint64_t score;
} HealthComponent;

void RegisterHealth(Scene_t* scene);
void UpdateHealth(Game* game, float dt);

typedef enum ProjectileType 
{
	BULLET,
	MISSILE,
	BOMB,
} ProjectileType;

typedef struct ProjectileComponent
{
	float damage;
	ProjectileType type;

} ProjectileComponent;

typedef struct TankHullAI
{
	bool IsMissleTruck;
} TankHullAI;

void RegisterTankAIs(Scene_t* scene);
void UpdateTankAIs(Game* game, float dt, entity_t player);

typedef struct GunTurretAI 
{
	Timer_t reloadTimer;
} GunTurretAI;

void RegisterGunAIs(Scene_t* scene);
void UpdateGunTurretAIs(Game* game, float dt, entity_t player);

typedef struct MissileLauncherAI 
{
	Timer_t realoadTimer;
} MissileLauncherAI;

typedef enum FighterState 
{
	Fighter_IDLE,
	Fighter_ATTACKING,
	Fighter_FLEEING,
} FighterState;

typedef struct FighterAI 
{
	FighterState state;
	Timer_t reloadTimer;
	int burstCounter;
} FighterAI;

void RegisterProjectile(Scene_t* scene);
void ResolveCollisionProjectiles(Game* game, entity_t a, entity_t b);

void SpawnGunTurret(Game* game, vec2 Pos, bool flip);
void SpawnTank(Game* game, vec2 Pos, bool flip, bool MissileTruck);

void RegisterGameComponents(Scene_t* scene);