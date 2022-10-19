#pragma once
#include "Core.h"
#include "Animation.h"
#include "Renderer2D.h"
#include "Timer.h"

struct Particle;

typedef bool(*ParticleFun_t)(struct Particle*);

typedef struct Particle 
{
	mat4 transform;
	SubTexture tex;
	vec4 col;
	Timer_t spawnTimer;
	ParticleFun_t updateFun;
} Particle;

//TODO particle system.