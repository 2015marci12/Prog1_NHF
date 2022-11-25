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
	bool releasedAfterFiring;
	uint32_t MissileAmmo;
	uint32_t BombAmmo;
} PlayerComponent;

void RegisterPlayer(Scene_t* scene);
void UpdatePlayer(Game* game, InputState* input, float dt);


typedef void(*HealthIsZeroCallback_t)(entity_t, Game*);

typedef struct HealthComponent
{
	float health;
	float max_health;
	float invincibility_time;
	Timer_t lastHit;
	Timer_t lastParticle;
	uint64_t score;
	HealthIsZeroCallback_t cb;
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

typedef struct MissileGuidanceComponent
{
	entity_t target;
	Timer_t lifeTime;
	Timer_t particleTimer;
} MissileGuidanceComponent;

void RegisterMissiles(Scene_t* scene);
void UpdateMissiles(Game* game, float dt);

typedef struct MissileLauncherAI 
{
	Timer_t realoadTimer;
} MissileLauncherAI;

void RegisterMissileLaunchers(Scene_t* scene);
void UpdateMissileLaunchers(Game* game, float dt);

typedef struct FighterAI 
{
	Timer_t reloadTimer;
	int burstCounter;
} FighterAI;

typedef enum PowerupType 
{
	HEALTH,
	MISSILES,
	BOMBS,
	SCORE,
	POWERUP_MAX,
} PowerupType;

typedef struct PowerupComponent 
{
	PowerupType type;
	vec2 SpawnPos;
	Timer_t timer;
} PowerupComponent;

void RegisterProjectile(Scene_t* scene);
void ResolveCollisionProjectiles(Game* game, entity_t a, entity_t b);

void SpawnTurret(Game* game, vec2 Pos, bool flip, bool MissileTurret);
void SpawnTank(Game* game, vec2 Pos, bool flip, bool MissileTruck);
void SpawnMissile(Game* game, vec2 Pos, vec2 Dir, int32_t alliegence, float damage, entity_t target);

void RegisterGameComponents(Scene_t* scene);