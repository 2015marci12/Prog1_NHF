#include "GameScene.h"
#include "GameComponents.h"

void GameParseInput(SDL_Window* win, InputSnapshot_t* snapshot, InputState* input)
{
	//Mouse.
	ivec2 screenSize = { 0 };
	SDL_GetWindowSize(win, &screenSize.x, &screenSize.y);
	ivec2 mousePos = GetMousePos(snapshot);
	ivec2 screenCenter = ivec2_Div_s(screenSize, 2);
	vec2 mouseDiff = ivec2_to_vec2(ivec2_Sub(mousePos, screenCenter));
	mouseDiff.y *= -1.f;
	input->LookDir = vec2_Normalize(mouseDiff);
	input->Thrust = max(0.f, min(vec2_Len(mouseDiff) / (float)ivec2_Min(screenCenter), 1.f));

	//Buttons.
	input->booster = IsKeyPressed(snapshot, SDL_SCANCODE_SPACE);
	input->firing = IsMouseButtonPressed(snapshot, SDL_BUTTON_LEFT);

	//Selection.
	if (IsKeyPressed(snapshot, SDL_SCANCODE_1)) input->selectedWeapon = 0;
	else if (IsKeyPressed(snapshot, SDL_SCANCODE_2)) input->selectedWeapon = 1;
	else if (IsKeyPressed(snapshot, SDL_SCANCODE_3)) input->selectedWeapon = 2;
	else input->selectedWeapon = -1;
}

void SpawnFloorCeilingTile(Game* game, float scale, float x)
{
	//Floor
	entity_t floor = Scene_CreateEntity(game->scene);
	mat4* transform = Scene_AddComponent(game->scene, floor, Component_TRANSFORM);
	Sprite* sprite = Scene_AddComponent(game->scene, floor, Component_SPRITE);

	*transform = mat4_Translate(mat4x4_Identity(), new_vec3(x, -game->constants.arena_height / 2 - scale * 1.f, 0.f));
	*sprite = Sprite_init();
	sprite->size = new_vec2(scale, scale * 2.f);
	sprite->subTex = TextureAtlas_SubTexture(&game->Textures[GROUND_TEX], new_uvec2(1, 1), new_uvec2(1, 2));

	//Ceiling.
	entity_t ceiling = Scene_CreateEntity(game->scene);
	transform = Scene_AddComponent(game->scene, ceiling, Component_TRANSFORM);
	sprite = Scene_AddComponent(game->scene, ceiling, Component_SPRITE);

	*transform = mat4_Translate(mat4x4_Identity(), new_vec3(x, game->constants.arena_height / 2 + scale * 1.5f, 0.f));
	*sprite = Sprite_init();
	sprite->size = new_vec2(scale, scale * 3.f);
	sprite->subTex = TextureAtlas_SubTexture(&game->Textures[GROUND_TEX], new_uvec2(0, 0), new_uvec2(1, 3));
}

void SetupWalls(Game* game)
{
	//Ground and ceiling sprites.
	float scale = 3.f;
	for (float x = -game->constants.arena_width / 2.f - scale * 10.f; x < game->constants.arena_width / 2 + scale * 10.f; x += scale)
	{
		SpawnFloorCeilingTile(game, scale, x);
	}

	//Floor Colloider
	entity_t floor = Scene_CreateEntity(game->scene);
	mat4* transform = Scene_AddComponent(game->scene, floor, Component_TRANSFORM);
	Colloider* colloider = Scene_AddComponent(game->scene, floor, Component_COLLOIDER);
	PhysicsComponent* physics = Scene_AddComponent(game->scene, floor, Component_PHYSICS);
	WallTagComponent* wallTag = Scene_AddComponent(game->scene, floor, Component_WallTag);

	*transform = mat4_Translate(mat4x4_Identity(), new_vec3(0.f, -game->constants.arena_height / 2 - scale, 0.f));

	colloider->body = new_Rect(-game->constants.arena_width / 2, -scale, game->constants.arena_width, scale * 2.f);
	colloider->maskBits = COLLISIONMASK_WALL;
	colloider->categoryBits = Layer_Walls;
	colloider->groupIndex = 0;

	physics->inv_mass = 0.f;
	physics->mass = 0.f;
	physics->restitution = 1.f;

	//Ceiling Colloider
	entity_t ceiling = Scene_CreateEntity(game->scene);
	transform = Scene_AddComponent(game->scene, ceiling, Component_TRANSFORM);
	colloider = Scene_AddComponent(game->scene, ceiling, Component_COLLOIDER);
	physics = Scene_AddComponent(game->scene, ceiling, Component_PHYSICS);
	wallTag = Scene_AddComponent(game->scene, ceiling, Component_WallTag);

	*transform = mat4_Translate(mat4x4_Identity(), new_vec3(0.f, game->constants.arena_height / 2 + scale * 1.f, 0.f));

	colloider->body = new_Rect(-game->constants.arena_width / 2, -scale, game->constants.arena_width, scale * 2.f);
	colloider->maskBits = COLLISIONMASK_WALL;
	colloider->categoryBits = Layer_Walls;
	colloider->groupIndex = 0;

	physics->inv_mass = 0.f;
	physics->mass = 0.f;
	physics->restitution = 1.f;

	//Left wall.
	entity_t leftwall = Scene_CreateEntity(game->scene);
	transform = Scene_AddComponent(game->scene, leftwall, Component_TRANSFORM);
	colloider = Scene_AddComponent(game->scene, leftwall, Component_COLLOIDER);
	physics = Scene_AddComponent(game->scene, leftwall, Component_PHYSICS);
	wallTag = Scene_AddComponent(game->scene, leftwall, Component_WallTag);

	*transform = mat4_Translate(mat4x4_Identity(), new_vec3(-game->constants.arena_width / 2, 0.f, 0.f));

	colloider->body = new_Rect(-scale, -game->constants.arena_height / 2, scale, game->constants.arena_height);
	colloider->maskBits = COLLISIONMASK_WALL;
	colloider->categoryBits = Layer_Walls;
	colloider->groupIndex = 0;

	physics->inv_mass = 0.f;
	physics->mass = 0.f;
	physics->restitution = 1.f;

	//Right wall.
	entity_t rightwall = Scene_CreateEntity(game->scene);
	transform = Scene_AddComponent(game->scene, rightwall, Component_TRANSFORM);
	colloider = Scene_AddComponent(game->scene, rightwall, Component_COLLOIDER);
	physics = Scene_AddComponent(game->scene, rightwall, Component_PHYSICS);
	wallTag = Scene_AddComponent(game->scene, rightwall, Component_WallTag);

	*transform = mat4_Translate(mat4x4_Identity(), new_vec3(game->constants.arena_width / 2, 0.f, 0.f));

	colloider->body = new_Rect(scale, -game->constants.arena_height / 2, -scale, game->constants.arena_height);
	colloider->maskBits = COLLISIONMASK_WALL;
	colloider->categoryBits = Layer_Walls;
	colloider->groupIndex = 0;

	physics->inv_mass = 0.f;
	physics->mass = 0.f;
	physics->restitution = 1.f;
}

Game* InitGame(Game* game, SDL_Window* window)
{
	if (game)
	{
		memset(game, 0, sizeof(Game));

		game->scene = Scene_New();
		RegisterStandardComponents(game->scene);
		RegisterGameComponents(game->scene);

		LoadConfig(&game->constants, "Config.ini");
		game->window = window;

		game->EnemyDestroyedCB = EnemyDestroyedCallBack;
		game->GameOverCB = GameOverCallBack;
		game->BonusEnemyDestroyedCB = BonusEnemyDestroyedCallBack;

		//Load assets.
		game->Textures[PLAYER_TEX] = TextureAtlas_create(LoadTex2D("Resources\\jet.png"), new_uvec2(64, 32));
		game->Textures[WEAPON_TEX] = TextureAtlas_create(LoadTex2D("Resources\\weapons.png"), new_uvec2(16, 16));
		game->Textures[EXPLOSION_TEX] = TextureAtlas_create(LoadTex2D("Resources\\Explosion.png"), new_uvec2(96, 96));
		game->Textures[GROUND_TEX] = TextureAtlas_create(LoadTex2D("Resources\\GroundTiling.png"), new_uvec2(32, 32));
		game->Textures[SMOKE_TEX] = TextureAtlas_create(LoadTex2D("Resources\\Smoke_Fire.png"), new_uvec2(16, 16));
		game->Textures[BG1_TEX] = TextureAtlas_create(LoadTex2D("Resources\\BG.png"), new_uvec2(550, 367));
		game->Textures[ENEMIES_TEX] = TextureAtlas_create(LoadTex2D("Resources\\Enemies.png"), new_uvec2(32, 16));
		game->Textures[UPGRADE_TEX] = TextureAtlas_create(LoadTex2D("Resources\\UpgradedEnemies.png"), new_uvec2(32, 16));
		game->Textures[HUD_TEX] = TextureAtlas_create(LoadTex2D("Resources\\HudIcons.png"), new_uvec2(32, 32));
		game->Textures[BONUS_TEX] = TextureAtlas_create(LoadTex2D("Resources\\Bonuses.png"), new_uvec2(32, 32));

		//Load font.
		game->font = LoadBitmapFont("Resources\\@Malgun Gothic.bff", true);

		Animation_FromIni("Resources\\BoosterAnim.ini", &game->Animations[BOOSTER_ANIM], &game->Textures[PLAYER_TEX]);
		Animation_FromIni("Resources\\CannonAnim.ini", &game->Animations[CANNON_ANIM], &game->Textures[PLAYER_TEX]);
		Animation_FromIni("Resources\\MissileAnim.ini", &game->Animations[MISSILE_ANIM], &game->Textures[WEAPON_TEX]);
		Animation_FromIni("Resources\\ExplosionAnim.ini", &game->Animations[EXPLOSION_ANIM], &game->Textures[EXPLOSION_TEX]);
		Animation_FromIni("Resources\\RadarAnim.ini", &game->Animations[RADAR_ANIM], &game->Textures[ENEMIES_TEX]);

		Animation_FromIni("Resources\\LightSmokeAnim.ini", &game->Animations[LIGHT_SMOKE_ANIM], &game->Textures[SMOKE_TEX]);
		Animation_FromIni("Resources\\HeavySmokeAnim.ini", &game->Animations[HEAVY_SMOKE_ANIM], &game->Textures[SMOKE_TEX]);
		Animation_FromIni("Resources\\LightFireAnim.ini", &game->Animations[LIGHT_FIRE_ANIM], &game->Textures[SMOKE_TEX]);
		Animation_FromIni("Resources\\HeavyFireAnim.ini", &game->Animations[HEAVY_FIRE_ANIM], &game->Textures[SMOKE_TEX]);

		//Particle systems.
		ParticleSystemData boosterData = { 0 };
		boosterData.animation = NULL;
		boosterData.size = new_vec2_v(0.1f);
		boosterData.z = -0.1f;
		game->Particles[PARTICLE_BOOSTER] = Particles_New(200u, boosterData);

		ParticleSystemData explosionData = { 0 };
		explosionData.animation = &game->Animations[EXPLOSION_ANIM];
		explosionData.size = new_vec2_v(1.f);
		explosionData.z = 1.f;
		game->Particles[PARTICLE_EXPLOSION] = Particles_New(400u, explosionData);

		ParticleSystemData smokeAndFireData = { 0 };
		smokeAndFireData.animation = &game->Animations[LIGHT_SMOKE_ANIM];
		smokeAndFireData.size = new_vec2_v(0.1f);
		smokeAndFireData.z = 1.f;
		game->Particles[LIGHT_SMOKE_PARTICLES] = Particles_New(400u, smokeAndFireData);
		smokeAndFireData.animation = &game->Animations[HEAVY_SMOKE_ANIM];
		game->Particles[HEAVY_SMOKE_PARTICLES] = Particles_New(400u, smokeAndFireData);
		smokeAndFireData.animation = &game->Animations[LIGHT_FIRE_ANIM];
		game->Particles[LIGHT_FIRE_PARTICLES] = Particles_New(400u, smokeAndFireData);
		smokeAndFireData.animation = &game->Animations[HEAVY_FIRE_ANIM];
		game->Particles[HEAVY_FIRE_PARTICLES] = Particles_New(400u, smokeAndFireData);

		//Spawn player and create camera.

		//Player
		entity_t e = Scene_CreateEntity(game->scene);
		mat4* tr = Scene_AddComponent(game->scene, e, Component_TRANSFORM);
		Sprite* s = Scene_AddComponent(game->scene, e, Component_SPRITE);
		PlayerComponent* pc = Scene_AddComponent(game->scene, e, Component_PLAYER);
		PlaneComponent* pm = Scene_AddComponent(game->scene, e, Component_PLANE);
		MovementComponent* mc = Scene_AddComponent(game->scene, e, Component_MOVEMENT);
		Colloider* pcoll = Scene_AddComponent(game->scene, e, Component_COLLOIDER);
		PhysicsComponent* pphys = Scene_AddComponent(game->scene, e, Component_PHYSICS);
		HealthComponent* phealth = Scene_AddComponent(game->scene, e, Component_HEALTH);

		*tr = mat4_Translate(mat4x4_Identity(), new_vec3(0.f, -game->constants.arena_height * 0.3f, 0.f));

		s->subTex = TextureAtlas_SubTexture(&game->Textures[PLAYER_TEX], new_uvec2(0, 3), new_uvec2(1, 1));;
		s->tintColor = new_vec4_v(1.f);
		s->size = new_vec2(2.f, 1.0f);
		for (int i = 0; i < 5; i++) s->overlays[i] = SubTexture_empty();

		PlaneComponent temp = {
			game->constants.lift_coeff,
			game->constants.drag_coeff,
			game->constants.thrust_booster,
			game->constants.plane_mass
		};
		*pm = temp;

		mc->acceleration = new_vec2_v(0.f);
		mc->velocity = new_vec2_v(0.f);

		pc->shootingTimer = MakeTimer();
		pc->boosterParticleTimer = MakeTimer();
		pc->selected_weapon = 0;
		pc->BombAmmo = 10;
		pc->MissileAmmo = 5;
		pc->max_fuel = game->constants.booster_fuel;
		pc->fuel = game->constants.booster_fuel;

		pcoll->body = new_Rect(-0.5f, -0.2f, 1.f, 0.4f);
		pcoll->categoryBits = Layer_Player;
		pcoll->maskBits = COLLISIONMASK_PLAYER;
		pcoll->groupIndex = FRIENDLY;

		pphys->inv_mass = CalcInvMass(game->constants.plane_mass);
		pphys->mass = game->constants.plane_mass;
		pphys->restitution = 1.f;

		phealth->health = game->constants.player_health;
		phealth->max_health = game->constants.player_health;
		phealth->invincibility_time = 0.2f;
		phealth->lastParticle = MakeTimer();
		phealth->lastHit = MakeTimer();
		phealth->score = 0;
		phealth->cb = game->GameOverCB;

		//Camera
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		glViewport(0, 0, w, h);
		float aspect = (float)w / (float)h;

		entity_t camera = Scene_CreateEntity(game->scene);
		mat4* cam_tr = Scene_AddComponent(game->scene, camera, Component_TRANSFORM);
		Camera* cam = Scene_AddComponent(game->scene, camera, Component_CAMERA);
		*cam_tr = mat4x4_Identity();
		*cam = mat4_Ortho(
			-aspect * game->constants.viewport_scale,
			aspect * game->constants.viewport_scale,
			game->constants.viewport_scale,
			-game->constants.viewport_scale,
			-1000, 1000);

		//Walls
		SetupWalls(game);

		SpawnTank(game, new_vec2(1.f, -game->constants.arena_height * 0.5f + 0.5f), false, true, true);
		SpawnTurret(game, new_vec2(-10.f, -game->constants.arena_height * 0.5f + 0.5f), false, false, false);
		SpawnTurret(game, new_vec2(5.f, game->constants.arena_height * 0.5f - 0.5f), true, true, true);
		SpawnRadar(game, new_vec2(-5.f, game->constants.arena_height * 0.5f - 0.5f), true, false);
	}
	return game;
}

void CleanupGame(Game* game)
{
	if (!game) return;
	Scene_Delete(game->scene);

	DeleteBitmapFont(game->font);

	for (int i = 0; i < TEX_COUNT; i++)
	{
		if (game->Textures[i].texture) GLTexture_Destroy(game->Textures[i].texture);
	}

	for (int i = 0; i < PARTICLESYS_COUNT; i++)
	{
		if (game->Particles[i]) Particles_Delete(game->Particles[i]);
	}
}

void DispatchGameEvents(Game* game, EventDispatcher_t* dispatcher)
{
	DispatchEvent(dispatcher, SDL_WINDOWEVENT, GameResizeEvent, game);
	DispatchEvent(dispatcher, CollisionEventType(), GameOnCollision, game);
	DispatchEvent(dispatcher, SDL_MOUSEBUTTONUP, GameOnMouseRelease, game);
}

void UpdateGame(Game* game, float dt)
{
	//Input.
	InputSnapshot_t snapshot = GetInput();
	InputState inputstate;
	GameParseInput(game->window, &snapshot, &inputstate);

	//Update.
	View_t v = View_Create(game->scene, 1, Component_PLAYER);//TODO game over
	if (!View_End(&v))
	{
		UpdateTankAIs(game, dt, View_GetCurrent(&v));
		UpdateGunTurretAIs(game, dt, View_GetCurrent(&v));
		MovePlanes(game, dt);
		UpdateMovement(game->scene, dt);

		UpdateMissileLaunchers(game, dt);
		UpdateMissiles(game, dt);
		UpdateLifetimes(game->scene);
		UpdateBonusTowers(game);
		GameUpdateCamera(game, &inputstate);
		UpdatePlayer(game, &inputstate, dt);
		FireCollisionEvents(game->scene);
		UpdateHealth(game, dt);
	}

	//Update particles.
	for (int i = 0; i < PARTICLESYS_COUNT; i++)
	{
		Particles_Update(game->Particles[i], dt);
	}
}

void RenderGame(Game* game, Renderer2D* renderer)
{
	mat4 cameraMVP;
	mat4* cam_tr;
	GetCamera(game->scene, &cam_tr, NULL, &cameraMVP);

	Renderer2D_BeginScene(renderer, cameraMVP);

	Renderer2D_Clear(renderer, new_vec4_v(0.f)); //Clear screen.

	//Render scene.
	RenderSprites(game->scene, renderer);
	GameRenderBackground(game, renderer);

	//Render particles.
	for (int i = 0; i < PARTICLESYS_COUNT; i++)
	{
		Particles_Draw(game->Particles[i], renderer);
	}

	DebugDrawColloiders(game->scene, renderer);

	Renderer2D_EndScene(renderer);

	//Render GUI scene.
	GameRenderGui(game, renderer);
}

bool GameResizeEvent(SDL_Event* e, void* userData)
{
	//Translate pointers.
	SDL_WindowEvent* we = &e->window;
	Game* game = userData;

	//Get camera.
	mat4* projection;
	GetCamera(game->scene, NULL, &projection, NULL);

	//Handle resize.
	if (we->event == SDL_WINDOWEVENT_RESIZED)
	{
		//Get width and height.
		int w = we->data1, h = we->data2;

		//Return if minimized.
		if (h <= 0 || w <= 0) return true;

		//Set camera.
		float aspect = (float)w / (float)h;
		*projection = mat4_Ortho(
			-aspect * game->constants.viewport_scale,
			aspect * game->constants.viewport_scale,
			game->constants.viewport_scale,
			-game->constants.viewport_scale,
			-1000, 1000);
	}

	return false;
}

bool GameOnCollision(SDL_Event* e, void* userData)
{
	//Translate pointers.
	CollisionEvent* ev = e->user.data1;
	Scene_t* scene = e->user.data2;
	Game* game = userData;

	//Physics.
	PhysicsResolveCollision(scene, ev);

	//Game logic.
	ResolveCollisionProjectiles(game, ev->a, ev->b);
	ResolveCollisionWall(game, ev->a, ev->b);
	ResolveCollisionPowerup(game, ev->a, ev->b);

	return false;
}

bool GameOnMouseRelease(SDL_Event* e, void* userData)
{
	//Translate pointers.
	SDL_MouseButtonEvent* ev = e;
	Game* game = userData;

	if (ev->button == SDL_BUTTON_LEFT) 
	{
		//Set release trigger for the player.
		View_t Player = View_Create(game->scene, 1, Component_PLAYER);
		if (View_End(&Player)) return false;
		PlayerComponent* pc = View_GetComponent(&Player, 0);
		pc->releasedAfterFiring = true;
	}

	return false;
}

void GameUpdateCamera(Game* game, InputState* input)
{
	//Camera.
	mat4* transform;
	entity_t camera = GetCamera(game->scene, &transform, NULL, NULL);

	//Get player.
	View_t v = View_Create(game->scene, 2, Component_TRANSFORM, Component_PLAYER);
	mat4* playertransform = View_GetComponent(&v, 0);

	//Set camera position.
	vec3 Pos = new_vec3_v4(mat4x4_Mul_v(*playertransform, new_vec4(0.f, 0.f, 0.f, 1.f))); //Get the position of the player.
	Pos = vec3_Add(Pos, new_vec3_v2(vec2_Mul_s(input->LookDir, input->Thrust * game->constants.peek_scale), 0.f)); //Shift it towards the cursor.

	//Bound camera between floor and ceiling.
	int w, h;
	SDL_GetWindowSize(game->window, &w, &h);
	float aspect = (float)w / (float)h;
	float clampDist_h = (game->constants.arena_height - 2.f * game->constants.viewport_scale) / 2 + 3.f;
	float clampDist_w = (game->constants.arena_width - 2.f * aspect * game->constants.viewport_scale) / 2;
	Pos.y = clamp(-clampDist_h, clampDist_h, Pos.y);
	Pos.x = clamp(-clampDist_w, clampDist_w, Pos.x);

	*transform = mat4_Translate(mat4x4_Identity(), Pos); //Set transfrom.
}

void GameRenderBackground(Game* game, Renderer2D* renderer)
{
	mat4* transform;
	GetCamera(game->scene, &transform, NULL, NULL);
	vec3 Pos = new_vec3_v4(mat4x4_Mul_v(*transform, new_vec4(0.f, 0.f, 0.f, 1.f)));

	const float BG1parallax = -0.05f;

	vec3 BG1Offset = vec3_Add(Pos, vec3_Mul_s(Pos, BG1parallax));
	BG1Offset.z = -10.f;
	BG1Offset.y -= game->constants.arena_height * 0.1f;
	const float BG1Aspect = 367.f / 550.f;
	const float BG1Width = 2.2f * (game->constants.viewport_scale + (game->constants.arena_width * fabsf(BG1parallax)));

	Renderer2D_DrawRotatedQuad_s(renderer, BG1Offset, new_vec2(BG1Width, BG1Width * BG1Aspect), 0.f, new_vec4_v(0.9f),
		TextureAtlas_SubTexture(&game->Textures[BG1_TEX], new_uvec2(0, 0), new_uvec2(1, 1)));
}

void GetEnemyDirections(Game* game, vec2 PlayerPos, bool* left, bool* right) 
{
	if (!left || !right) return;
	for (View_t v = View_Create(game->scene, 2, Component_TRANSFORM, Component_EnemyTag);
		!View_End(&v); View_Next(&v)) 
	{
		mat4* transform = View_GetComponent(&v, 0);
		vec2 EnemyPos;
		Transform_Decompose_2D(*transform, &EnemyPos, NULL, NULL);

		*left |= EnemyPos.x < PlayerPos.x;
		*right |= EnemyPos.x > PlayerPos.x;
		
		if (*left && *right) return;
	}
}

void GameRenderGui(Game* game, Renderer2D* renderer)
{
	View_t Player = View_Create(game->scene, 3, Component_PLAYER, Component_HEALTH, Component_TRANSFORM);
	if (View_End(&Player)) return; //Game over, no player found.

	PlayerComponent* pc = View_GetComponent(&Player, 0);
	HealthComponent* health = View_GetComponent(&Player, 1);
	mat4* transform = View_GetComponent(&Player, 2);
	vec2 PlayerPos;
	Transform_Decompose_2D(*transform, &PlayerPos, NULL, NULL);

	int w, h;
	SDL_GetWindowSize(game->window, &w, &h);

	mat4 view = mat4_Ortho(0, w, 0, h, 30, -30);
	Renderer2D_BeginScene(renderer, view);
	Renderer2D_ClearDepth(renderer);

	char buff[256]; //Buffer to format the displayed text with.

	const float margin = 50.f;
	const float fontSize = 50.f;

	//HealthBar.
	const float barLength = 300.f;
	vec4 green = new_vec4(0.f, 1.f, 0.f, 1.f);
	vec4 red = new_vec4(1.f, 0.f, 0.f, 1.f);
	float healthFraction = (health->health / health->max_health);
	vec4 healthColor = vec4_Add(vec4_Mul_s(green, healthFraction), vec4_Mul_s(red, 1.f - healthFraction));

	Renderer2D_DrawFilledRect(renderer, new_Rect(margin, margin, barLength, 50.f), 1.f, new_vec4(0.5f, 0.5f, 0.5f, 1.f));
	Renderer2D_DrawFilledRect(renderer, new_Rect(margin, margin, barLength * healthFraction, 50.f), 0.f, healthColor);
	Renderer2D_DrawFilledRect_t(renderer, new_Rect(margin - 65.f, margin + 100.f, 150.f, -150.f), -1.f, new_vec4_v(1.f),
		TextureAtlas_SubTexture(&game->Textures[HUD_TEX], new_uvec2(0, 2), new_uvec2(1, 1)));
	Renderer2D_DrawText(renderer, new_vec3(barLength + fontSize * 0.5f + margin, margin + 25.f, 0.f), game->font, fontSize, new_vec4_v(1.f), "Health", true);

	//FuelBar.
	vec4 FuelBarColor = new_vec4(1.f, 0.7f, 0.f, 1.f);
	float fuelFraction = pc->fuel / pc->max_fuel;
	const float spaceBetweenBars = 80.f;

	Renderer2D_DrawFilledRect(renderer, new_Rect(margin, margin + spaceBetweenBars, barLength, 50.f), 1.f, new_vec4(0.5f, 0.5f, 0.5f, 1.f));
	Renderer2D_DrawFilledRect(renderer, new_Rect(margin, margin + spaceBetweenBars, barLength * fuelFraction, 50.f), 0.f, FuelBarColor);
	Renderer2D_DrawFilledRect_t(renderer, new_Rect(margin - 50.f, margin + 75.f + spaceBetweenBars, 100.f, -100.f), -1.f, new_vec4_v(1.f),
		TextureAtlas_SubTexture(&game->Textures[HUD_TEX], new_uvec2(1, 2), new_uvec2(1, 1)));
	Renderer2D_DrawText(renderer, new_vec3(barLength + fontSize * 0.5f + margin, margin + 25.f + spaceBetweenBars, 0.f), game->font, fontSize, new_vec4_v(1.f), "Fuel", true);
	
	//Score counter.
	snprintf(buff, 256, "Score: %llu", game->score);
	float x = (float)w - Renderer2D_CalcTextSize(renderer, game->font, fontSize, new_vec4_v(1.f), buff).x - margin;
	Renderer2D_DrawText(renderer, new_vec3(x, margin, 0.f), game->font, fontSize, new_vec4_v(1.f), buff, true);

	//Wave counter.
	snprintf(buff, 256, "Wave: %u", game->Wave);
	x = (float)w - Renderer2D_CalcTextSize(renderer, game->font, fontSize, new_vec4_v(1.f), buff).x - margin;
	Renderer2D_DrawText(renderer, new_vec3(x, margin + 50, 0.f), game->font, fontSize, new_vec4_v(1.f), buff, true);

	//Enemy counter.
	snprintf(buff, 256, "Enemies: %u", game->EnemyCount);
	x = (float)w - Renderer2D_CalcTextSize(renderer, game->font, fontSize, new_vec4_v(1.f), buff).x - margin;
	Renderer2D_DrawText(renderer, new_vec3(x, margin + 100, 0.f), game->font, fontSize, new_vec4_v(1.f), buff, true);

	//Enemy direction indicators.
	bool left = false, right = false;
	GetEnemyDirections(game, PlayerPos, &left, &right);

	x = w - margin - 200;
	if (left) 
		Renderer2D_DrawFilledRect_t(renderer, new_Rect(x, margin + 150, 100, 100), 0.f, new_vec4_v(1.f),
			TextureAtlas_SubTexture(&game->Textures[HUD_TEX], new_uvec2(1, 3), new_uvec2(1, 1)));
	x += 100;
	if (right) 
		Renderer2D_DrawFilledRect_t(renderer, new_Rect(x, margin + 150, 100, 100), 0.f, new_vec4_v(1.f),
			TextureAtlas_SubTexture(&game->Textures[HUD_TEX], new_uvec2(0, 3), new_uvec2(1, 1)));

	//Weapon selection.
	
	//Cannon.
	const float CannonStartY = spaceBetweenBars + margin + 80.f;
	if (pc->selected_weapon == 0)
	{
		//Selection.
		Renderer2D_DrawFilledRect(renderer, new_Rect(margin, CannonStartY, 270, 50), 1.f, new_vec4(0.5f, 0.5f, 0.6f, 0.2f));
	}
	Renderer2D_DrawFilledRect_t(renderer, new_Rect(margin, CannonStartY + 50, 50, -50), 0.f, new_vec4_v(1.f), 
		TextureAtlas_SubTexture(&game->Textures[HUD_TEX], new_uvec2(0, 0), new_uvec2(1, 1)));
	snprintf(buff, 256, "Cannon");
	Renderer2D_DrawText(renderer, new_vec3(margin + 70.f, CannonStartY + fontSize * 0.5f, 0.f), game->font, fontSize, new_vec4_v(1.f), buff, true);

	//Missiles.
	const float MissilesStartY = CannonStartY + 50.f;
	if (pc->selected_weapon == 1)
	{
		//Selection.
		Renderer2D_DrawFilledRect(renderer, new_Rect(margin, MissilesStartY, 270, 50), 1.f, new_vec4(0.5f, 0.5f, 0.6f, 0.2f));
	}
	Renderer2D_DrawFilledRect_t(renderer, new_Rect(margin, MissilesStartY + 50, 50, -50), 0.f, new_vec4_v(1.f),
		TextureAtlas_SubTexture(&game->Textures[HUD_TEX], new_uvec2(1, 1), new_uvec2(1, 1)));
	snprintf(buff, 256, "Missiles: %u", pc->MissileAmmo);
	Renderer2D_DrawText(renderer, new_vec3(margin + 70.f, MissilesStartY + fontSize * 0.5f, 0.f), game->font, fontSize, new_vec4_v(1.f), buff, true);
	
	//Bombs.
	const float BombStartY = MissilesStartY + 50.f;
	if (pc->selected_weapon == 2)
	{
		//Selection.
		Renderer2D_DrawFilledRect(renderer, new_Rect(margin, BombStartY, 270, 50), 1.f, new_vec4(0.5f, 0.5f, 0.6f, 0.2f));
	}
	Renderer2D_DrawFilledRect_t(renderer, new_Rect(margin, BombStartY + 50, 50, -50), 0.f, new_vec4_v(1.f),
		TextureAtlas_SubTexture(&game->Textures[HUD_TEX], new_uvec2(0, 1), new_uvec2(1, 1)));
	snprintf(buff, 256, "Bombs: %u", pc->BombAmmo);
	Renderer2D_DrawText(renderer, new_vec3(margin + 70.f, BombStartY + fontSize * 0.5f, 0.f), game->font, fontSize, new_vec4_v(1.f), buff, true);

	Renderer2D_EndScene(renderer, view);
}

void GameOverCallBack(entity_t player, void* game)
{
	Game* g = game;
	g->GameOver = true;
}

void EnemyDestroyedCallBack(entity_t enemy, void* game)
{
	Game* g = game;
	g->EnemyCount--;
	//TODO spawn next wave.
}

void BonusEnemyDestroyedCallBack(entity_t enemy, void* game)
{
	Game* g = game;
	EnemyDestroyedCallBack(enemy, game);
	mat4* Transform = Scene_Get(g->scene, enemy, Component_TRANSFORM);

	bool spawn = RandB_Chance(g->constants.powerup_chance);
	PowerupType type = RandUI32_Range(0, POWERUP_MAX - 1);
	if (spawn) SpawnPowerup(g, *Transform, type);
}
