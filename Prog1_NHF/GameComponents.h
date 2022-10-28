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

void MovePlanes(Game* game, float dt);

typedef struct PlayerComponent
{
	float fuel;
	Timer_t boosterParticleTimer;
	Timer_t shootingTimer;
} PlayerComponent;

void UpdatePlayer(Game* game, InputState* input, float dt);