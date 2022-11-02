#include "ParticleSystem.h"

ParticleSystem* Particles_Init(ParticleSystem* inst, size_t max_Particles, ParticleSystemData data)
{
	if (inst)
	{
		inst->data = data;
		inst->count = 0;
		inst->max_Particles = max_Particles;
	}
	return inst;
}

ParticleSystem* Particles_New(size_t max_Particles, ParticleSystemData data)
{
	return Particles_Init(malloc(sizeof(ParticleSystem) + max_Particles * sizeof(Particle)), max_Particles, data);
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
			float t = GetElapsedSeconds(p->SpawnTime) / p->LifeTime;

			//Euler integration.
			p->Position = vec2_Add(p->Position, vec2_Mul_s(p->Velocity, dt));
			p->Velocity = vec2_Add(p->Velocity, vec2_Mul_s(p->Acceleration, dt));
			p->rotation += p->rotational_vel * dt;

			//Color linear interpolation.
			p->CurrentColor = vec4_Add(p->StartColor, vec4_Mul_s(vec4_Sub(p->EndColor, p->StartColor), t));

			//Size linear interpolation.
			p->CurrentSize = vec2_Add(p->StartSize, vec2_Mul_s(vec2_Sub(p->EndSize, p->StartSize), t));

			//Delete if lifetime is over.
			if (t > 1.f)
			{
				*p = inst->particles[--inst->count];
				i--;
			}
		}
	}
}

void Particles_Draw(ParticleSystem* inst, Renderer2D* renderer)
{
	if (inst)
	{
		for (int i = 0; i < inst->count; i++)
		{
			Particle* p = &inst->particles[i];
			float time = GetElapsedSeconds(p->SpawnTime);
			if (time > p->LifeTime) return;

			SubTexture tex = SubTexture_empty();
			if (inst->data.animation) 
			{
				tex = Animation_GetAt(inst->data.animation, time, NULL);
			}

			Renderer2D_DrawRotatedQuad_s(renderer,
				new_vec3_v2(p->Position, inst->data.z),
				vec2_Mul(inst->data.size, p->CurrentSize),
				p->rotation,
				p->CurrentColor,
				tex);
		}
	}
}

Particle MakeParticle(vec2 Pos, float rot, vec4 col, float lifetime)
{
	Particle p = {0};
	p.Position = Pos;
	p.Velocity = new_vec2_v(0.f);
	p.Acceleration = new_vec2_v(0.f);

	p.rotation = rot;
	p.rotational_vel = 0.f;

	p.LifeTime = lifetime;
	p.SpawnTime = MakeTimer();

	p.StartColor = col;
	p.EndColor = col;
	p.CurrentColor = col;

	p.StartSize = new_vec2_v(1.f);
	p.EndSize = new_vec2_v(1.f);
	p.CurrentSize = new_vec2_v(1.f);
	return p;
}

Particle MakeParticle_s(vec2 Pos, float rot, vec4 col, vec2 size, float lifetime)
{
	Particle p = { 0 };
	p.Position = Pos;
	p.Velocity = new_vec2_v(0.f);
	p.Acceleration = new_vec2_v(0.f);

	p.rotation = rot;
	p.rotational_vel = 0.f;

	p.LifeTime = lifetime;
	p.SpawnTime = MakeTimer();

	p.StartColor = col;
	p.EndColor = col;
	p.CurrentColor = col;

	p.StartSize = size;
	p.EndSize = size;
	p.CurrentSize = size;
	return p;
}

void Particles_Emit(ParticleSystem* inst, Particle p)
{
	if (inst)
	{
		if (inst->count < inst->max_Particles)
		{
			inst->particles[inst->count++] = p;
		}
	}
}