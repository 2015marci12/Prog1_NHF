#pragma once
#include "Core.h"
#include "Animation.h"
#include "Renderer2D.h"
#include "Timer.h"

struct Particle;

typedef bool(*ParticleFun_t)(struct Particle*, float dt);


typedef struct Particle 
{
	mat4 transform;
	SubTexture tex;
	vec4 col;
	Timer_t spawnTimer;
	vec2 velocity;
	vec2 acceleration;
	float lifespan;
	ParticleFun_t updateFun;
} Particle;

bool defaultParticleMovementFun(Particle* p, float dt);

#define MAX_PARTICLES 4096

typedef struct ParticleSystem 
{
	Particle particles[MAX_PARTICLES];
	size_t count;
} ParticleSystem;

ParticleSystem* Particles_Init(ParticleSystem* inst);
ParticleSystem* Particles_New();
void Particles_Delete(ParticleSystem* inst);
void Particles_Update(ParticleSystem* inst, float dt);
void Particles_Draw(ParticleSystem* inst, Renderer2D* renderer, mat4 camera);
void Particles_Emit(ParticleSystem* inst, Particle p);
