#pragma once
#include "Core.h"
#include "Animation.h"
#include "Renderer2D.h"
#include "Timer.h"

typedef struct Particle 
{
	vec2 Position;
	vec2 Velocity;
	vec2 Acceleration;

	float rotation;
	float rotational_vel;

	Timer_t SpawnTime;
	float LifeTime;

	vec4 CurrentColor;
	vec4 StartColor;
	vec4 EndColor;

	vec2 StartSize;
	vec2 CurrentSize;
	vec2 EndSize;
} Particle;

typedef struct ParticleSystemData 
{
	Animation* animation;
	float z;
	vec2 size;
} ParticleSystemData;

typedef struct ParticleSystem 
{
	ParticleSystemData data;

	size_t count;
	size_t max_Particles;
	Particle particles[];
} ParticleSystem;

ParticleSystem* Particles_Init(ParticleSystem* inst, size_t max_Particles, ParticleSystemData data);
ParticleSystem* Particles_New(size_t max_Particles, ParticleSystemData data);
void Particles_Delete(ParticleSystem* inst);
void Particles_Update(ParticleSystem* inst, float dt);
void Particles_Draw(ParticleSystem* inst, Renderer2D* renderer);
Particle MakeParticle(vec2 Pos, float rot, vec4 col, float lifetime);
Particle MakeParticle_s(vec2 Pos, float rot, vec4 col, vec2 size, float lifetime);
void Particles_Emit(ParticleSystem* inst, Particle p);
