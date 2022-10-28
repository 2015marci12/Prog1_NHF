#include "GameComponents.h"

void MovePlanes(Game* game, float dt)
{
	for (View_t view = View_Create(game->scene, 3, Component_TRANSFORM, Component_PLANE, Component_MOVEMENT);
		!View_End(&view); View_Next(&view))
	{
		mat4* transform = View_GetComponent(&view, 0);
		PlaneComponent* plane = View_GetComponent(&view, 1);
		MovementComponent* mc = View_GetComponent(&view, 2);

		vec2 forward = new_vec2_v4(mat4x4_Mul_v(*transform, new_vec4(1.f, 0.f, 0.f, 1.f)));
		vec3 pos = new_vec3_v4(mat4x4_Mul_v(*transform, new_vec4(0.f, 0.f, 0.f, 1.f)));
		vec2 direction = vec2_Normalize(vec2_Sub(forward, new_vec2_v3(pos)));
		vec2 velocitynormal = vec2_Normalize(mc->velocity);
		float vel = vec2_Len(mc->velocity);

		float dirAngle = vec2_Angle(direction);
		float velAngle = vec2_Angle(velocitynormal);

		//Angle differernce.
		float anglediff = dirAngle - velAngle;
		anglediff += (anglediff > PI) ? -(PI * 2.f) : (anglediff < -PI) ? (PI * 2.f) : 0;
		float aoa = max(-1.f, min(1.f, (anglediff))); //aoa restricted between [-1, 1]

		vec2 aeroforce = vec2_Mul_s(velocitynormal, -1.f * (0.5f + fabsf(aoa)) * plane->dragcoeff * (vel * vel)); //drag.
		vec2 thrustforce = vec2_Mul_s(direction, plane->thrust); //thrust.
		vec2 gravity = vec2_Mul_s(new_vec2(0.f, -1.f), game->constants.g * plane->mass); //gravity.

		//Lift.
		vec2 lift = vec2_Rot(new_vec2(0.f, vel * aoa * plane->liftcoeff), velAngle);
		//Adjust for straight flying.
		lift = vec2_Add(lift, new_vec2(0.f, fabsf(mc->velocity.x) / 5.f * plane->liftcoeff));

		//Sum forces and divide by mass.
		vec2 accel = vec2_Div_s(vec2_Add(vec2_Add(vec2_Add(aeroforce, thrustforce), gravity), lift), plane->mass);

		//Apply to movement.
		mc->acceleration = accel;
	}
}

bool SpawnExplosion(Scene_t* scene, entity_t e, const void* d)
{
	Game* game = d;
	mat4* transform = Scene_Get(game->scene, e, Component_TRANSFORM);
	vec2 Pos = new_vec2_v4(mat4x4_Mul_v(*transform, new_vec4(0.f, 0.f, 0.f, 1.f)));
	
	Particle p = MakeParticle(Pos, 0.f, new_vec4_v(1.f), Animaton_GetDuration(&game->Animations[EXPLOSION_ANIM]));
	Particles_Emit(game->Particles[PARTICLE_EXPLOSION], p);
	return false;
}

void UpdatePlayer(Game* game, InputState* input, float dt)
{
	View_t view = View_Create(game->scene, 5, Component_TRANSFORM, Component_PLAYER, Component_MOVEMENT, Component_SPRITE, Component_PLANE);

	mat4* transform = View_GetComponent(&view, 0);
	PlayerComponent* pc = View_GetComponent(&view, 1);
	MovementComponent* mc = View_GetComponent(&view, 2);
	PlaneComponent* pm = View_GetComponent(&view, 4);
	Sprite* sprite = View_GetComponent(&view, 3);

	vec3 Pos = new_vec3_v4(mat4x4_Mul_v(*transform, new_vec4(0.f, 0.f, 0.f, 1.f)));

	//Look at mouse.
	float angle = vec2_Angle(input->LookDir);
	*transform = mat4_Rotate(mat4_Translate(mat4x4_Identity(), Pos), angle, new_vec3(0.f, 0.f, 1.f));

	if (mc->velocity.x < 0) *transform = mat4_Scale(*transform, new_vec3(1.f, -1.f, 1.f)); //Invert sprite if the velocity is facing the other way.

	//scale thrust.
	float thrust = game->constants.thrust_idle;
	thrust *= input->Thrust;
	if (input->booster)
	{
		thrust = game->constants.thrust_booster;

		//Booster animation.
		Timer_t timer = { 0 };
		sprite->overlays[0] = Animation_GetAt(&game->Animations[BOOSTER_ANIM], GetElapsedSeconds(timer), NULL);

		//Booster particles.
		vec4 colors[] =
		{
			new_vec4(1.f, 0.7f, 0.f, 1.f),
			new_vec4(0.9f, 0.7f, 0.f, 1.f),
			new_vec4(0.9f, 0.7f, 0.2f, 1.f),
			new_vec4(0.5f, 0.5f, 0.5f, 1.f),
			new_vec4(0.7f, 0.7f, 0.7f, 1.f),
			new_vec4(0.9f, 0.9f, 0.9f, 1.f),
		};
		vec4 col = colors[(int)(((float)rand() / (float)(RAND_MAX)) * 6.f)];
		float lifetime = 0.2f + ((float)rand() / (float)(RAND_MAX)) * 0.2f;

		vec2 ppos = new_vec2_v4(mat4x4_Mul_v(*transform, new_vec4(-0.8f, 0.f, 0.f, 1.f)));

		Particle p = MakeParticle(ppos, 0.f, col, lifetime);
		p.Velocity = vec2_Add(vec2_Mul_s(mc->velocity, -0.5f), vec2_s_Mul(10.f, new_vec2(((float)rand() / (float)(RAND_MAX)) - 0.5f, ((float)rand() / (float)(RAND_MAX)) - 0.5f)));

		if (GetElapsedSeconds(pc->boosterParticleTimer) > game->constants.booster_particle_time)
		{
			pc->boosterParticleTimer = MakeTimer();
			Particles_Emit(game->Particles[PARTICLE_BOOSTER], p);
		}

		//TODO particle emitter and proper particle systems per particle type.
	}
	else
	{
		sprite->overlays[0] = SubTexture_empty();
	}
	pm->thrust = thrust;

	//shooting.
	if (input->firing)
	{
		//cannon animation.
		Timer_t timer = { 0 };
		sprite->overlays[1] = Animation_GetAt(&game->Animations[CANNON_ANIM], GetElapsedSeconds(timer), NULL);

		//Spawn bullet
		if (GetElapsedSeconds(pc->shootingTimer) > game->constants.cannon_shooting_time)
		{
			pc->shootingTimer = MakeTimer();
			entity_t bullet = Scene_CreateEntity(game->scene);

			vec2 forward = new_vec2_v4(mat4x4_Mul_v(*transform, new_vec4(1.f, 0.f, 0.f, 1.f)));
			vec3 pos = new_vec3_v4(mat4x4_Mul_v(*transform, new_vec4(0.f, 0.f, 0.f, 1.f)));
			vec2 direction = vec2_Normalize(vec2_Sub(forward, new_vec2_v3(pos)));
			mat4* bullettransform = Scene_AddComponent(game->scene, bullet, Component_TRANSFORM);
			*bullettransform = mat4_Translate(*transform, new_vec3(0.6f, 0.f, 0.f));

			Sprite* bulletsprite = Scene_AddComponent(game->scene, bullet, Component_SPRITE);
			*bulletsprite = Sprite_init();
			bulletsprite->size = new_vec2(0.3f, 0.3f);
			bulletsprite->subTex = TextureAtlas_SubTexture(&game->Textures[WEAPON_TEX], new_uvec2(0, 1), new_uvec2(1, 1));
			bulletsprite->tintColor = new_vec4_v(1.f);

			MovementComponent* bmc = Scene_AddComponent(game->scene, bullet, Component_MOVEMENT);
			bmc->velocity = vec2_Add(mc->velocity, vec2_Mul_s(direction, game->constants.bullet_velocity));
			bmc->acceleration = new_vec2_v(0.f);

			LifetimeComponent* lt = Scene_AddComponent(game->scene, bullet, Component_LIFETIME);
			lt->timer = MakeTimer();
			lt->lifetime = game->constants.bullet_lifeTime;
			lt->userdata = NULL;
			lt->callback = NULL; //TODO go poof when done.
			lt->userdata = game;
			lt->callback = SpawnExplosion;

			Colloider* bColl = Scene_AddComponent(game->scene, bullet, Component_COLLOIDER);
			bColl->body = new_Rect(-0.15f, -0.15f, 0.3f, 0.3f);
			bColl->categoryBits = Layer_Bullets;
			bColl->maskBits = COLLISIONMASK_BULLET;
			bColl->groupIndex = FRIENDLY;
		}
	}
	else
	{
		sprite->overlays[1] = SubTexture_empty();
	}
}
