#include "GameComponents.h"

void RegisterPlane(Scene_t* scene)
{
	ComponentInfo_t planeInfo = COMPONENT_DEF(Component_PLANE, PlaneComponent);
	Scene_AddComponentType(scene, planeInfo);
}

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

bool SpawnSmoke(Scene_t* scene, entity_t e, const void* d)
{
	Game* game = d;
	mat4* transform = Scene_Get(game->scene, e, Component_TRANSFORM);
	vec2 Pos = new_vec2_v4(mat4x4_Mul_v(*transform, new_vec4(0.f, 0.f, 0.f, 1.f)));

	float rot = RandF_1_Range(0.f, 2.f * PI);
	Particle p = MakeParticle_s(Pos, rot, new_vec4_v(1.f), new_vec2_v(5.f), Animaton_GetDuration(&game->Animations[LIGHT_SMOKE_ANIM]));
	p.EndSize = new_vec2_v(1.f);
	Particles_Emit(game->Particles[LIGHT_SMOKE_PARTICLES], p);
	return false;
}

void RegisterPlayer(Scene_t* scene)
{
	ComponentInfo_t playerInfo = COMPONENT_DEF(Component_PLAYER, PlayerComponent);
	Scene_AddComponentType(scene, playerInfo);
}

void SpawnBullet(Game* game, mat4* transform, MovementComponent* mc, int32_t group, float vel, float damage)
{
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
	bmc->velocity = vec2_Mul_s(direction, vel);
	bmc->acceleration = new_vec2_v(0.f);

	LifetimeComponent* lt = Scene_AddComponent(game->scene, bullet, Component_LIFETIME);
	lt->timer = MakeTimer();
	lt->lifetime = game->constants.bullet_lifeTime;
	lt->userdata = game;
	lt->callback = SpawnSmoke;

	Colloider* bColl = Scene_AddComponent(game->scene, bullet, Component_COLLOIDER);
	bColl->body = new_Rect(-0.15f, -0.15f, 0.3f, 0.3f);
	bColl->categoryBits = Layer_Bullets;
	bColl->maskBits = COLLISIONMASK_BULLET;
	bColl->groupIndex = group;

	ProjectileComponent* bProj = Scene_AddComponent(game->scene, bullet, Component_PROJECTILE);
	bProj->damage = damage;
	bProj->type = BULLET;
}

void SpawnBomb(Game* game, mat4* transform, MovementComponent* mc)
{
	entity_t bomb = Scene_CreateEntity(game->scene);

	vec2 forward = new_vec2_v4(mat4x4_Mul_v(*transform, new_vec4(1.f, 0.f, 0.f, 1.f)));
	vec3 pos = new_vec3_v4(mat4x4_Mul_v(*transform, new_vec4(0.f, 0.f, 0.f, 1.f)));
	vec2 direction = vec2_Normalize(vec2_Sub(forward, new_vec2_v3(pos)));
	mat4* bombtransform = Scene_AddComponent(game->scene, bomb, Component_TRANSFORM);
	*bombtransform = mat4_Translate(*transform, new_vec3(0.f, -0.1f, 0.f));

	Sprite* bombsprite = Scene_AddComponent(game->scene, bomb, Component_SPRITE);
	*bombsprite = Sprite_init();
	bombsprite->size = new_vec2(0.5f, 0.5f);
	bombsprite->subTex = TextureAtlas_SubTexture(&game->Textures[WEAPON_TEX], new_uvec2(1, 0), new_uvec2(1, 1));
	bombsprite->tintColor = new_vec4_v(1.f);

	MovementComponent* bmc = Scene_AddComponent(game->scene, bomb, Component_MOVEMENT);
	bmc->velocity = mc->velocity;
	bmc->acceleration = new_vec2(0.f, -2.f * game->constants.g);

	LifetimeComponent* lt = Scene_AddComponent(game->scene, bomb, Component_LIFETIME);
	lt->timer = MakeTimer();
	lt->lifetime = 30.f;
	lt->userdata = game;
	lt->callback = SpawnSmoke;

	Colloider* bColl = Scene_AddComponent(game->scene, bomb, Component_COLLOIDER);
	bColl->body = new_Rect(-0.15f, -0.15f, 0.3f, 0.3f);
	bColl->categoryBits = Layer_Missiles;
	bColl->maskBits = COLLISIONMASK_BULLET;
	bColl->groupIndex = FRIENDLY;

	ProjectileComponent* bProj = Scene_AddComponent(game->scene, bomb, Component_PROJECTILE);
	bProj->damage = game->constants.bullet_damage; //TODO
	bProj->type = BOMB;
}

void UpdatePlayer(Game* game, InputState* input, float dt)
{
	View_t view = View_Create(game->scene, 5, Component_TRANSFORM, Component_PLAYER, Component_MOVEMENT, Component_SPRITE, Component_PLANE);

	mat4* transform = View_GetComponent(&view, 0);
	mat4* cam_transform;
	GetCamera(game->scene, &cam_transform, NULL, NULL);
	PlayerComponent* pc = View_GetComponent(&view, 1);
	MovementComponent* mc = View_GetComponent(&view, 2);
	PlaneComponent* pm = View_GetComponent(&view, 4);
	Sprite* sprite = View_GetComponent(&view, 3);

	vec3 Pos = new_vec3_v4(mat4x4_Mul_v(*transform, new_vec4(0.f, 0.f, 0.f, 1.f)));
	vec3 CamPos = new_vec3_v4(mat4x4_Mul_v(*cam_transform, new_vec4(0.f, 0.f, 0.f, 1.f)));
	int w, h;
	SDL_GetWindowSize(game->window, &w, &h);
	float aspect = (float)w / (float)h;
	vec2 correction = vec2_Div(new_vec2_v3(vec3_Sub(CamPos, Pos)),
		new_vec2(aspect * game->constants.viewport_scale, game->constants.viewport_scale));
	vec2 trueLookVec = vec2_Add(vec2_Mul_s(input->LookDir, input->Thrust), correction);

	//Look at mouse.
	float angle = vec2_Angle(trueLookVec);
	*transform = mat4_Rotate(mat4_Translate(mat4x4_Identity(), Pos), angle, new_vec3(0.f, 0.f, 1.f));

	if (mc->velocity.x < 0) *transform = mat4_Scale(*transform, new_vec3(1.f, -1.f, 1.f)); //Invert sprite if the velocity is facing the other way.

	if (input->selectedWeapon != -1) pc->selected_weapon = input->selectedWeapon;

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
		vec4 col = colors[RandI32_Range(0, 6)];
		float lifetime = RandF_1_Range(0.2f, 0.4f);

		vec2 ppos = new_vec2_v4(mat4x4_Mul_v(*transform, new_vec4(-0.8f, 0.f, 0.f, 1.f)));

		Particle p = MakeParticle(ppos, 0.f, col, lifetime);
		p.Velocity = vec2_Add(vec2_Mul_s(mc->velocity, -0.5f), vec2_s_Mul(vec2_Len(mc->velocity) * 0.3f, new_vec2(RandF_1_Range(-0.5f, 0.5f), RandF_1_Range(-0.5f, 0.5f))));
		p.EndColor.a = 0.f;

		if (GetElapsedSeconds(pc->boosterParticleTimer) > game->constants.booster_particle_time)
		{
			pc->boosterParticleTimer = MakeTimer();
			Particles_Emit(game->Particles[PARTICLE_BOOSTER], p);
		}
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

		switch (pc->selected_weapon)
		{
		case 0:
			if (GetElapsedSeconds(pc->shootingTimer) > game->constants.cannon_shooting_time)
			{
				pc->shootingTimer = MakeTimer();
				SpawnBullet(game, transform, mc, FRIENDLY, game->constants.bullet_velocity, game->constants.bullet_damage);
			}
			break;
		case 1: break;
		case 2:
			if (GetElapsedSeconds(pc->shootingTimer) > game->constants.cannon_shooting_time) //TODO
			{
				pc->shootingTimer = MakeTimer();
				SpawnBomb(game, transform, mc);
			}
			break;
		default:
			break;
		}
	}
	else
	{
		sprite->overlays[1] = SubTexture_empty();
	}
}

void RegisterHealth(Scene_t* scene)
{
	ComponentInfo_t cinf = COMPONENT_DEF(Component_HEALTH, HealthComponent);
	Scene_AddComponentType(scene, cinf);
}

void UpdateHealth(Game* game, float dt)
{
	for (View_t v = View_Create(game->scene, 1, Component_HEALTH);
		!View_End(&v);)
	{
		HealthComponent* health = View_GetComponent(&v, 0);
		if (health->health <= 0.f)
		{
			mat4* transform = Scene_Get(game->scene, View_GetCurrent(&v), Component_TRANSFORM);
			if (transform)
			{
				//Death animation. TODO maybe change to something with a bit more OOMPH. Also sound effect.
				vec2 Pos = new_vec2_v4(mat4x4_Mul_v(*transform, new_vec4(0.f, 0.f, 0.f, 1.f)));

				Particle p = MakeParticle_s(Pos, 0.f, new_vec4_v(1.f), new_vec2_v(5.f), Animaton_GetDuration(&game->Animations[EXPLOSION_ANIM]));
				Particles_Emit(game->Particles[PARTICLE_EXPLOSION], p);
			}
			game->score += health->score;
			KillChildren(game->scene, View_GetCurrent(&v));
			View_DestroyCurrent_FindNext(&v);
		}
		else
		{
			mat4* transform = Scene_Get(game->scene, View_GetCurrent(&v), Component_TRANSFORM);
			if (transform && (GetElapsedSeconds(health->lastParticle) > 0.05f))
			{
				health->lastParticle = MakeTimer();

				//Damage animation.
				vec2 Pos = new_vec2_v4(mat4x4_Mul_v(*transform, new_vec4(0.f, 0.f, 0.f, 1.f)));
				int damageLevel = (int)floorf(health->health / health->max_health * 5.f);
				float rot = RandF_1_Range(0.f, 2.f * PI);
				float v = RandF_1();
				vec2 velrand = vec2_Rot(new_vec2(0.f, 0.5f), rot);
				rot *= PI * 2.f;
				Particle p = MakeParticle_s(Pos, rot, new_vec4_v(1.f), new_vec2_v(5.f), 1.f);
				p.Velocity = vec2_Add(new_vec2(0.f, 2.f), velrand);
				p.EndSize = new_vec2_v(1.f);
				switch (damageLevel)
				{
				case 0:
					p.LifeTime = Animaton_GetDuration(&game->Animations[HEAVY_FIRE_ANIM]);
					Particles_Emit(game->Particles[HEAVY_FIRE_PARTICLES], p);
					break;
				case 1:
					p.LifeTime = Animaton_GetDuration(&game->Animations[LIGHT_FIRE_ANIM]);
					Particles_Emit(game->Particles[LIGHT_FIRE_PARTICLES], p);
					break;
				case 2:
					p.LifeTime = Animaton_GetDuration(&game->Animations[HEAVY_SMOKE_ANIM]);
					Particles_Emit(game->Particles[HEAVY_SMOKE_PARTICLES], p);
					break;
				case 3:
					p.LifeTime = Animaton_GetDuration(&game->Animations[LIGHT_SMOKE_ANIM]);
					Particles_Emit(game->Particles[LIGHT_SMOKE_PARTICLES], p);
					break;
				default:
					break;
				}
			}

			//Invincibility animation.
			Sprite* sprite = Scene_Get(game->scene, View_GetCurrent(&v), Component_SPRITE);
			if (sprite && (GetElapsedSeconds(health->lastHit) < health->invincibility_time)) 
			{
				float alpha = health->invincibility_time / GetElapsedSeconds(health->lastHit) + 1.f;
				sprite->tintColor = new_vec4_v(alpha);
				sprite->tintColor.a = 1.f;
			}
			else if(sprite)
			{
				sprite->tintColor = new_vec4_v(1.f);
			}
			View_Next(&v);
		}
	}
}

void RegisterTankAIs(Scene_t* scene)
{
	ComponentInfo_t cinf = COMPONENT_DEF(Component_TankAI, TankHullAI);
	Scene_AddComponentType(scene, cinf);
}

void UpdateTankAIs(Game* game, float dt, entity_t player)
{
	mat4 PlayerTransform = *(mat4*)Scene_Get(game->scene, player, Component_TRANSFORM);

	vec2 PlayerPos = new_vec2_v4(mat4x4_Mul_v(PlayerTransform, new_vec4(0.f, 0.f, 0.f, 1.f)));
	for (View_t v = View_Create(game->scene, 4, Component_TRANSFORM, Component_MOVEMENT, Component_SPRITE, Component_TankAI);
		!View_End(&v); View_Next(&v))
	{
		mat4* Transform = View_GetComponent(&v, 0);
		MovementComponent* Movement = View_GetComponent(&v, 1);
		Sprite* Sprite = View_GetComponent(&v, 2);
		TankHullAI* AI = View_GetComponent(&v, 3);

		vec2 Pos = new_vec2_v4(mat4x4_Mul_v(*Transform, new_vec4(0.f, 0.f, 0.f, 1.f)));
		vec2 Diff = vec2_Sub(PlayerPos, Pos);

		//Movement towards player.
		if (vec2_Len(Diff) > 20.f) 
		{
			Movement->velocity = new_vec2_v(0.f);
		}
		else 
		{
			Movement->velocity = new_vec2(clamp(-1.f, 1.f, Diff.x) * 2.f, 0.f);
		}

		//Animation
		if (AI->IsMissleTruck) 
		{
			Sprite->subTex = TextureAtlas_SubTexture(&game->Textures[ENEMIES_TEX], new_uvec2(0, 5), new_uvec2(1, 1));
		}
		else 
		{
			entity_t turretChild = FirstChild(game->scene, View_GetCurrent(&v));
			mat4* TurretTransform = Scene_Get(game->scene, turretChild, Component_TRANSFORM);

			vec2 Pos2 = new_vec2_v4(mat4x4_Mul_v(*TurretTransform, new_vec4(0.f, 0.f, 0.f, 1.f)));
			vec2 Dir = new_vec2_v4(mat4x4_Mul_v(*TurretTransform, new_vec4(1.f, 0.f, 0.f, 1.f)));
			Dir = vec2_Sub(Dir, Pos2);


			SubTexture directions[2] =
			{
				TextureAtlas_SubTexture(&game->Textures[ENEMIES_TEX], new_uvec2(0, 7), new_uvec2(1, 1)),
				TextureAtlas_SubTexture(&game->Textures[ENEMIES_TEX], new_uvec2(1, 7), new_uvec2(1, 1))
			};

			Sprite->subTex = directions[((Dir.x <= 0.f) ^ (Diff.x <= 0.f)) ? 1 : 0];
			
		}

		Sprite->subTex = SubTexture_Flip(Sprite->subTex, (Diff.x <= 0.f), false);	
	}
}

void RegisterGunAIs(Scene_t* scene)
{
	ComponentInfo_t cinf = COMPONENT_DEF(Component_GunTurretAI, GunTurretAI);
	Scene_AddComponentType(scene, cinf);
}

void UpdateGunTurretAIs(Game* game, float dt, entity_t player)
{
	mat4 PlayerTransform = *(mat4*)Scene_Get(game->scene, player, Component_TRANSFORM);
	MovementComponent PlayerMovement = *(MovementComponent*)Scene_Get(game->scene, player, Component_MOVEMENT);

	vec2 PlayerPos = new_vec2_v4(mat4x4_Mul_v(PlayerTransform, new_vec4(0.f, 0.f, 0.f, 1.f)));

	for (View_t v = View_Create(game->scene, 3, Component_TRANSFORM, Component_SPRITE, Component_GunTurretAI);
		!View_End(&v); View_Next(&v))
	{
		mat4* Transform = View_GetComponent(&v, 0);
		Sprite* Sprite = View_GetComponent(&v, 1);
		GunTurretAI* AI = View_GetComponent(&v, 2);

		mat4 WorldTransform = CalcWorldTransform(game->scene, View_GetCurrent(&v));
		vec2 Pos = new_vec2_v4(mat4x4_Mul_v(WorldTransform, new_vec4(0.f, 0.f, 0.f, 1.f)));
		vec2 Dir = new_vec2_v4(mat4x4_Mul_v(WorldTransform, new_vec4(1.f, 0.f, 0.f, 1.f)));
		Dir = vec2_Sub(Dir, Pos);
		
		vec2 Diff = vec2_Sub(PlayerPos, Pos);

		if (vec2_Len(Diff) > (game->constants.bullet_velocity * game->constants.bullet_lifeTime * 0.6f)) continue;

		//Calculate firing solution.
		float a = vec2_Dot(PlayerMovement.velocity, PlayerMovement.velocity) - (game->constants.bullet_velocity * game->constants.bullet_velocity);
		float b = vec2_Dot(PlayerMovement.velocity, Diff) * 2.f;
		float c = vec2_Dot(Diff, Diff);

		//Quadratic formula.
		float p = -b / (2.f * a); 
		float q = sqrtf((b * b) - 4 * a * c) / (2.f * a);
		float t1 = p - q;
		float t2 = p + q;
		float t;

		//Pick the better solution (sorter fligt time)
		if (t1 > t2 && t2 > 0) t = t2;
		else t = t1;

		//Randomize shot because otherwise the thing is too accurate :)
		float randomMisAim = RandF_1_Range(-0.5f, 0.5f);
		//scale randomness with distance.
		randomMisAim *= clamp(0.2f, 3.f, randomMisAim * vec2_Len(Diff) / 7.f);

		vec2 AimSpot = vec2_Add(PlayerPos, vec2_Mul_s(PlayerMovement.velocity, t + randomMisAim));

		vec2 AimPath = vec2_Sub(AimSpot, Pos);

		//Turn towards target spot.
		float AngleDiff = vec2_Angle(AimPath) - vec2_Angle(Dir) + randomMisAim;

		//Fire when ready.
		if (GetElapsedSeconds(AI->reloadTimer) > game->constants.cannon_shooting_time * 4.f) 
		{
			*Transform = mat4_Rotate(*Transform, AngleDiff, new_vec3(0.f, 0.f, 1.f));
			AI->reloadTimer = MakeTimer();
			MovementComponent dummyMc;
			dummyMc.acceleration = new_vec2_v(0.f);
			dummyMc.velocity = new_vec2_v(0.f);

			SpawnBullet(game, &WorldTransform, &dummyMc, ENEMY, game->constants.bullet_velocity, game->constants.bullet_damage_enemy);
		}
	}
}

void RegisterProjectile(Scene_t* scene)
{
	ComponentInfo_t cinf = COMPONENT_DEF(Component_PROJECTILE, ProjectileComponent);
	Scene_AddComponentType(scene, cinf);
}

void RegisterMissiles(Scene_t* scene)
{
	ComponentInfo_t cinf = COMPONENT_DEF(Component_MissileGuidance, MissileGuidanceComponent);
	Scene_AddComponentType(scene, cinf);
}

void UpdateMissiles(Game* game, float dt)
{
	for (View_t v = View_Create(game->scene, 3, Component_TRANSFORM, Component_MOVEMENT, Component_MissileGuidance);
		!View_End(&v);)
	{
		mat4* Transform = View_GetComponent(&v, 0);
		MovementComponent* Movement = View_GetComponent(&v, 1);
		MissileGuidanceComponent* Missile = View_GetComponent(&v, 2);

		//TODO constant for missile lifetime.
		if (GetElapsedSeconds(Missile->lifeTime) > game->constants.bullet_lifeTime * 5.f)
		{
			SpawnSmoke(game->scene, View_GetCurrent(&v), game);
			View_DestroyCurrent_FindNext(&v);
			continue;
		}

		if (GetElapsedSeconds(Missile->particleTimer) > 0.5f)
		{
			SpawnSmoke(game->scene, View_GetCurrent(&v), game);
			Missile->particleTimer = MakeTimer();
		}

		mat4 target_Transform = CalcWorldTransform(game->scene, Missile->target);
		vec2 ToTarget = new_vec2_v4(vec4_Sub(
			mat4x4_Mul_v(target_Transform, new_vec4(0.f, 0.f, 0.f, 1.f)),
			mat4x4_Mul_v(*Transform, new_vec4(0.f, 0.f, 0.f, 1.f))
		));

		vec2 Pos = new_vec2_v4(mat4x4_Mul_v(*Transform, new_vec4(0.f, 0.f, 0.f, 1.f)));
		vec2 Dir = new_vec2_v4(mat4x4_Mul_v(*Transform, new_vec4(1.f, 0.f, 0.f, 1.f)));
		Dir = vec2_Sub(Dir, Pos);

		float ToTargetRot = clamp(
			-game->constants.missile_turnrate,
			game->constants.missile_turnrate,
			vec2_Angle(ToTarget) - vec2_Angle(Dir)
		);

		*Transform = mat4_Rotate(*Transform, ToTargetRot, new_vec3(0.f, 0.f, 1.f));
		Movement->velocity = vec2_Mul_s(new_vec2_v4(
			mat4x4_Mul_v(*Transform, new_vec4(1.f, 0.f, 0.f, 1.f))), game->constants.missile_velocity);

		View_Next(&v);
	}
}

void ProjectileHit(HealthComponent* health, ProjectileComponent* proj)
{
	if (GetElapsedSeconds(health->lastHit) > health->invincibility_time)
	{
		health->health -= proj->damage;
		health->lastHit = MakeTimer();
	}
}

void SpawnProjectileParticle(Game* game, mat4* transform, ProjectileType type)
{
	if (type == BULLET)
	{
		if (transform)
		{
			vec2 Pos = new_vec2_v4(mat4x4_Mul_v(*transform, new_vec4(0.f, 0.f, 0.f, 1.f)));
			float rot = (float)rand() / (float)RAND_MAX;
			Particle p = MakeParticle_s(Pos, rot, new_vec4_v(1.f), new_vec2_v(5.f), Animaton_GetDuration(&game->Animations[LIGHT_SMOKE_ANIM]));
			p.EndSize = new_vec2_v(1.f);
			Particles_Emit(game->Particles[LIGHT_SMOKE_PARTICLES], p);
		}
	}
	else
	{
		if (transform)
		{
			vec2 Pos = new_vec2_v4(mat4x4_Mul_v(*transform, new_vec4(0.f, 0.f, 0.f, 1.f)));
			float rot = (float)rand() / (float)RAND_MAX;
			Particle p = MakeParticle_s(Pos, rot, new_vec4_v(1.f), new_vec2_v(2.f), Animaton_GetDuration(&game->Animations[EXPLOSION_ANIM]));
			Particles_Emit(game->Particles[PARTICLE_EXPLOSION], p);
		}
	}
}

void ResolveCollisionProjectiles(Game* game, entity_t a, entity_t b)
{
	ProjectileComponent* projA = Scene_Get(game->scene, a, Component_PROJECTILE);
	ProjectileComponent* projB = Scene_Get(game->scene, b, Component_PROJECTILE);
	HealthComponent* healthA = Scene_Get(game->scene, a, Component_HEALTH);
	HealthComponent* healthB = Scene_Get(game->scene, b, Component_HEALTH);

	if (projA)
	{
		if (healthB) ProjectileHit(healthB, projA);
		//Partile on projectile death.
		mat4* transform = Scene_Get(game->scene, a, Component_TRANSFORM);
		SpawnProjectileParticle(game, transform, projA->type);
		KillChildren(game->scene, a);
		Scene_DeleteEntity(game->scene, a);
	}
	else if (projB)
	{
		if (healthA) ProjectileHit(healthA, projB);
		//Particle on projectile death.
		mat4* transform = Scene_Get(game->scene, b, Component_TRANSFORM);
		SpawnProjectileParticle(game, transform, projB->type);
		KillChildren(game->scene, b);
		Scene_DeleteEntity(game->scene, b);
	}
}

void SpawnGunTurret(Game* game, vec2 Pos, bool flip)
{
	entity_t building = Scene_CreateEntity(game->scene);

	mat4* transform = Scene_AddComponent(game->scene, building, Component_TRANSFORM);
	*transform = mat4_Translate(mat4x4_Identity(), new_vec3_v2(Pos, 0.f));
	if (flip) *transform = mat4_Rotate(*transform, PI, new_vec3(0.f, 0.f, 1.f));

	Sprite* sprite = Scene_AddComponent(game->scene, building, Component_SPRITE);
	*sprite = Sprite_init();
	sprite->size = new_vec2(2.f, 1.f);
	sprite->subTex = TextureAtlas_SubTexture(&game->Textures[ENEMIES_TEX], new_uvec2(2, 1), new_uvec2_v(1));
	sprite->tintColor = new_vec4_v(1.f);

	Colloider* coll = Scene_AddComponent(game->scene, building, Component_COLLOIDER);
	coll->body = new_Rect_ps(new_vec2(-0.5f, -0.5f), new_vec2(1.f, 1.f));
	coll->categoryBits = Layer_Enemies;
	coll->maskBits = COLLISIONMASK_ENEMY;
	coll->groupIndex = ENEMY;

	HealthComponent* health = Scene_AddComponent(game->scene, building, Component_HEALTH);
	health->health = game->constants.structure_health;
	health->invincibility_time = 0.1f;
	health->max_health = game->constants.structure_health;
	health->score = game->constants.structure_score;
	health->lastHit = MakeTimer();
	health->lastParticle = MakeTimer();


	entity_t turret = Scene_CreateEntity(game->scene);

	AddChild(game->scene, building, turret);

	transform = Scene_AddComponent(game->scene, turret, Component_TRANSFORM);
	*transform = mat4_Translate(mat4x4_Identity(), new_vec3(0.f, 0.3f, 2.f));

	sprite = Scene_AddComponent(game->scene, turret, Component_SPRITE);
	*sprite = Sprite_init();
	sprite->subTex = TextureAtlas_SubTexture(&game->Textures[ENEMIES_TEX], new_uvec2(2, 7), new_uvec2(1, 1));
	sprite->size = new_vec2(2.f, 1.f);

	GunTurretAI* AI = Scene_AddComponent(game->scene, turret, Component_GunTurretAI);
	AI->reloadTimer = MakeTimer();
}

void SpawnTank(Game* game, vec2 Pos, bool flip, bool MissileTruck)
{
	entity_t tank = Scene_CreateEntity(game->scene);

	mat4* transform = Scene_AddComponent(game->scene, tank, Component_TRANSFORM);
	*transform = mat4_Translate(mat4x4_Identity(), new_vec3_v2(Pos, 0.f));
	if (flip) *transform = mat4_Rotate(*transform, PI, new_vec3(0.f, 0.f, 1.f));

	Sprite* sprite = Scene_AddComponent(game->scene, tank, Component_SPRITE);
	*sprite = Sprite_init();
	sprite->size = new_vec2(2.f, 1.f);
	sprite->subTex = SubTexture_empty(); //The AI will choose the texture.
	sprite->tintColor = new_vec4_v(1.f);

	Colloider* coll = Scene_AddComponent(game->scene, tank, Component_COLLOIDER);
	coll->body = new_Rect_ps(new_vec2(-0.5f, -0.5f), new_vec2(1.f, 1.f));
	coll->categoryBits = Layer_Enemies;
	coll->maskBits = COLLISIONMASK_ENEMY;
	coll->groupIndex = ENEMY;

	HealthComponent* health = Scene_AddComponent(game->scene, tank, Component_HEALTH);
	health->health = game->constants.vehicle_health;
	health->invincibility_time = 0.1f;
	health->max_health = game->constants.vehicle_health;
	health->score = game->constants.vehicle_score;
	health->lastHit = MakeTimer();
	health->lastParticle = MakeTimer();

	MovementComponent* movement = Scene_AddComponent(game->scene, tank, Component_MOVEMENT);
	movement->acceleration = new_vec2_v(0.f);
	movement->velocity = new_vec2_v(0.f);

	TankHullAI* tankAI = Scene_AddComponent(game->scene, tank, Component_TankAI);
	tankAI->IsMissleTruck = MissileTruck;

	if (!MissileTruck) 
	{
		entity_t turret = Scene_CreateEntity(game->scene);

		AddChild(game->scene, tank, turret);

		transform = Scene_AddComponent(game->scene, turret, Component_TRANSFORM);
		*transform = mat4_Translate(mat4x4_Identity(), new_vec3(0.f, 0.2f, 0.f));

		sprite = Scene_AddComponent(game->scene, turret, Component_SPRITE);
		*sprite = Sprite_init();
		sprite->subTex = TextureAtlas_SubTexture(&game->Textures[ENEMIES_TEX], new_uvec2(2, 7), new_uvec2(1, 1));
		sprite->size = new_vec2(2.f, 1.f);

		GunTurretAI* AI = Scene_AddComponent(game->scene, turret, Component_GunTurretAI);
		AI->reloadTimer = MakeTimer();
	}
	else 
	{
		//TODO add missile launcher.
	}

}

void SpawnMissile(Game* game, vec2 Pos, vec2 Dir, int32_t alliegence, float damage) 
{
	//TODO
}

void RegisterGameComponents(Scene_t* scene)
{
	RegisterPlane(scene);
	RegisterPlayer(scene);
	RegisterHealth(scene);
	RegisterProjectile(scene);
	RegisterGunAIs(scene);
	RegisterTankAIs(scene);
}