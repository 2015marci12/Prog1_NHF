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
} PlayerComponent;

void RegisterPlayer(Scene_t* scene);
void UpdatePlayer(Game* game, InputState* input, float dt);

typedef struct HealthComponent
{
	float health;
	float max_health;
	float invincibility_time;
	Timer_t lastParticle;
} HealthComponent;

void RegisterHealth(Scene_t* scene);
void UpdateHealth(Game* game, float dt);

typedef struct ProjectileComponent
{
	float damage;
} ProjectileComponent;

void RegisterProjectile(Scene_t* scene);

void ResolveCollisionProjectiles(Game* game, entity_t a, entity_t b);

void RegisterGameComponents(Scene_t* scene);