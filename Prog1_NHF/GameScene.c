#include "GameScene.h"
#include "GameComponents.h"

void GameParseInput(SDL_Window* win, InputSnapshot_t* snapshot, InputState* input)
{
	//Mouse.
	ivec2 screenSize;
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

	*transform = mat4_Translate(mat4x4_Identity(), new_vec3(x, -game->constants.arena_height / 2, 0.f));
	*sprite = Sprite_init();
	sprite->size = new_vec2(scale, scale * 2.f);
	sprite->subTex = TextureAtlas_SubTexture(&game->Textures[GROUND_TEX], new_uvec2(1, 1), new_uvec2(1, 2));
}

void SetupWalls(Game* game)
{
	float scale = 3.f;
	for (float x = -game->constants.arena_width / 2.f; x < game->constants.arena_width / 2 + scale; x += scale)
	{
		SpawnFloorCeilingTile(game, scale, x);
	}

	//FloorColloider.
	entity_t floor = Scene_CreateEntity(game->scene);
	mat4* transform = Scene_AddComponent(game->scene, floor, Component_TRANSFORM);
	Colloider* colloider = Scene_AddComponent(game->scene, floor, Component_COLLOIDER);
	PhysicsComponent* physics = Scene_AddComponent(game->scene, floor, Component_PHYSICS);

	*transform = mat4_Translate(mat4x4_Identity(), new_vec3(0.f, -game->constants.arena_height / 2, 0.f));

	colloider->body = new_Rect(-game->constants.arena_width / 2, -scale, game->constants.arena_width, scale * 2.f);
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
		ComponentInfo_t playerInfo = COMPONENT_DEF(Component_PLAYER, PlayerComponent);
		Scene_AddComponentType(game->scene, playerInfo);
		ComponentInfo_t planeInfo = COMPONENT_DEF(Component_PLANE, PlaneComponent);
		Scene_AddComponentType(game->scene, planeInfo);

		LoadConfig(&game->constants, "Config.ini");
		game->window = window;

		//Particle systems.
		ParticleSystemData boosterData;
		boosterData.animation = NULL;
		boosterData.size = new_vec2_v(0.1f);
		boosterData.z = -0.1f;
		game->Particles[PARTICLE_BOOSTER] = Particles_New(200u, boosterData);

		ParticleSystemData explosionData;
		explosionData.animation = &game->Animations[EXPLOSION_ANIM];
		explosionData.size = new_vec2_v(-1.f);
		explosionData.z = 1.f;
		game->Particles[PARTICLE_EXPLOSION] = Particles_New(400u, explosionData);

		//Load assets.
		game->Textures[PLAYER_TEX] = TextureAtlas_create(LoadTex2D("Resources\\jet.png"), new_uvec2(64, 32));
		game->Textures[WEAPON_TEX] = TextureAtlas_create(LoadTex2D("Resources\\weapons.png"), new_uvec2(16, 16));
		game->Textures[EXPLOSION_TEX] = TextureAtlas_create(LoadTex2D("Resources\\Explosion.png"), new_uvec2(96, 96));
		game->Textures[GROUND_TEX] = TextureAtlas_create(LoadTex2D("Resources\\GroundTiling.png"), new_uvec2(32, 32));

		Animation_FromIni("Resources\\BoosterAnim.ini", &game->Animations[BOOSTER_ANIM], &game->Textures[PLAYER_TEX]);
		Animation_FromIni("Resources\\CannonAnim.ini", &game->Animations[CANNON_ANIM], &game->Textures[PLAYER_TEX]);
		Animation_FromIni("Resources\\MissileAnim.ini", &game->Animations[MISSILE_ANIM], &game->Textures[WEAPON_TEX]);
		Animation_FromIni("Resources\\ExplosionAnim.ini", &game->Animations[EXPLOSION_ANIM], &game->Textures[EXPLOSION_TEX]);

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

		pcoll->body = new_Rect(-0.5f, -0.2f, 1.f, 0.4f);
		pcoll->categoryBits = Layer_Player;
		pcoll->maskBits = COLLISIONMASK_PLAYER;
		pcoll->groupIndex = FRIENDLY;

		pphys->inv_mass = CalcInvMass(game->constants.plane_mass);
		pphys->mass = game->constants.plane_mass;
		pphys->restitution = 1.f;

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
	}
	return game;
}

void CleanupGame(Game* game)
{
	if (!game) return;
	Scene_Delete(game->scene);

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
}

void UpdateGame(Game* game, float dt)
{
	//Input.
	InputSnapshot_t snapshot = GetInput();
	InputState inputstate;
	GameParseInput(game->window, &snapshot, &inputstate);

	//Update.
	MovePlanes(game, dt);
	UpdatePlayer(game, &inputstate, dt);
	UpdateMovement(game->scene, dt);
	UpdateLifetimes(game->scene);
	GameUpdateCamera(game, &inputstate);
	FireCollisionEvents(game->scene);

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
	RenderSprites(game->scene, renderer, cameraMVP);

	//Render particles.
	for (int i = 0; i < PARTICLESYS_COUNT; i++)
	{
		Particles_Draw(game->Particles[i], renderer);
	}

	DebugDrawColloiders(game->scene, renderer);

	//Render test background.
	int w, h;
	SDL_GetWindowSize(game->window, &w, &h);
	float aspect = (float)w / (float)h;
	vec2 camPos = new_vec2_v4(mat4x4_Mul_v(*cam_tr, new_vec4(0.f, 0.f, 0.f, 1.f)));
	for (float x = camPos.x - aspect * game->constants.viewport_scale; x < camPos.x + aspect * game->constants.viewport_scale; x++)
	{
		float x_ = floorf(x / 5.f) * 5.f;
		Renderer2D_DrawLine(renderer,
			new_vec3(x_, camPos.y + game->constants.viewport_scale, -1.f),
			new_vec3(x_, camPos.y - game->constants.viewport_scale, -1.f),
			new_vec4_v(1.f)
		);
	}

	for (float y = camPos.y - game->constants.viewport_scale; y < camPos.y + game->constants.viewport_scale; y++)
	{
		float y_ = floorf(y / 5.f) * 5.f;
		Renderer2D_DrawLine(renderer,
			new_vec3(camPos.x + aspect * game->constants.viewport_scale, y_, -1.f),
			new_vec3(camPos.x - aspect * game->constants.viewport_scale, y_, -1.f),
			new_vec4_v(1.f)
		);
	}

	Renderer2D_EndScene(renderer);
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

	//Physics.
	PhysicsResolveCollision(scene, ev);

	//TODO gamelogic.

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
	*transform = mat4_Translate(mat4x4_Identity(), Pos); //Set transfrom.
}