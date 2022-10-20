#include "ParticleSystem.h"

bool defaultParticleMovementFun(Particle* p, float dt)
{
	vec2 movement = vec2_Mul_s(p->velocity, dt);
	p->transform = mat4x4x4_Mul(p->transform, mat4_Translate(mat4x4_Identity(), new_vec3_v2(movement, 0.f)));
	p->velocity = vec2_Add(p->velocity, vec2_Mul_s(p->acceleration, dt));
	float remainingTime = p->lifespan - GetElapsedSeconds(p->spawnTimer);
	p->col.a = remainingTime / p->lifespan;
	return remainingTime > 0.f;
}

ParticleSystem* Particles_Init(ParticleSystem* inst)
{
	if (inst)
	{
		memset(inst->particles, 0, sizeof(Particle) * MAX_PARTICLES);
		inst->count = 0;
	}
	return inst;
}

ParticleSystem* Particles_New()
{
	return Particles_Init(malloc(sizeof(ParticleSystem)));
}

void Particles_Delete(ParticleSystem* inst)
{
	if (inst) free(inst);
}

void Particles_Update(ParticleSystem* inst, float dt) 
{
	if (inst)
	{
		for (int i = 0; i < inst->count; i++)
		{
			Particle* p = &inst->particles[i];
			if (!(p->updateFun)(p, dt))
			{
				*p = inst->particles[--inst->count];				
				i--;
			}
		}
	}
}

void Particles_Draw(ParticleSystem* inst, Renderer2D* renderer, mat4 camera)
{
	if (inst)
	{
		Renderer2D_BeginScene(renderer, camera);
		for (int i = 0; i < inst->count; i++)
		{
			Particle* p = &inst->particles[i];
			Renderer2D_DrawSprite(renderer, p->transform, new_vec2_v(1.f), p->col, p->tex);
		}
		Renderer2D_EndScene(renderer);
	}
}

void Particles_Emit(ParticleSystem* inst, Particle p)
{
	if (inst)
	{
		if (inst->count < MAX_PARTICLES)
		{
			inst->particles[inst->count++] = p;
		}
	}
}