#include "Core.h"
#include "Mathcell.h"
#include "Graphics.h"
#include "Container.h"
#include "ECS.h"
#include "Renderer2D.h"
#include "Input.h"
#include "Timer.h"
#include "ParticleSystem.h"
#include "Components.h"

#include <SDL2/SDL.h>
#include <stdio.h>

//Collision layers.
typedef enum CollisionLayer
{
	Layer_Walls,
	Layer_Player,
	Layer_Enemies,
	Layer_Bullets,
	Layer_Missiles,
} CollisionLayer;

#define PLAYER_BULLET_MASK (BIT(Layer_Walls) | BIT(Layer_Enemies) | BIT(Layer_Missiles))
#define PLAYER_MISSILE_MASK (BIT(Layer_Walls) | BIT(Layer_Enemies) | BIT(Layer_Bullets))
#define ENEMY_BULLET_MASK (BIT(Layer_Walls) | BIT(Layer_Player) | BIT(Layer_Missiles))
//#define PLAYER_MISSILE_MASK (BIT(Layer_Walls) | BIT(Layer_Player) | BIT(Layer_Bullets))

ParticleSystem* particles;

typedef struct PlayerComponent
{
	float fuel;
	Timer_t boosterParticleTimer;
	Timer_t shootingTimer;
} PlayerComponent;

typedef struct PlaneComponent
{
	float liftcoeff;
	float dragcoeff;
	float thrust;
	float mass;
} PlaneComponent;

typedef struct InputState
{
	vec2 LookDir;
	float Thrust;
	bool booster;
	bool firing;
} InputState;

void ParseInput(SDL_Window* win, InputSnapshot_t* snapshot, InputState* input)
{
	//TODO: gamepad controls.

	//Mouse
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
}

const float viewport_scale = 10.f;
const float peek_scale = 2.f;

const float g = 3.f;
const float thrust_idle = 13.f;
const float thrust_booster = 25.f;
const float lift_coeff = 1.1f;
const float drag_coeff = 0.1f;
const float plane_mass = 1.f;

const float booster_fuelconsumption = 0.1f;
const float booster_particle_time = 0.01f;

const float cannon_shooting_time = 0.08f;
const float bullet_velocity = 20.f;
const float bullet_lifeTime = 1.f;
const float bullet_damage = 1.f;

SDL_Window* window;

GLTexture* tex;
TextureAtlas atlas;
SubTexture planeTex;

Animation boosterAnim;
Animation cannonAnim;

GLTexture* weaponsTex;
TextureAtlas weaponsAtlas;
SubTexture bulletSubTex;

Animation missileAnim;

GLTexture* explosionTex;
TextureAtlas explosionAtlas;
Animation explosionAnim;

//Handle window events.
bool OnWindowEvent(SDL_Event* e, void* userData) 
{
	//Translate pointers.
	SDL_WindowEvent* we = &e->window;
	Scene_t* scene = userData;

	//Get camera.
	mat4* projection;
	GetCamera(scene, NULL, &projection, NULL);

	//Handle resize.
	if (we->event = SDL_WINDOWEVENT_RESIZED) 
	{
		//Get width and height.
		int w = we->data1, h = we->data2;

		//Return if minimized.
		if (h <= 0 || w <= 0) return true; 

		//Set viewport.
		glViewport(0, 0, w, h);

		//Set camera.
		float aspect = (float)w / (float)h;
		*projection = mat4_Ortho(-aspect * viewport_scale, aspect * viewport_scale, viewport_scale, -viewport_scale, -1000, 1000);

		return true;
	}

	return false;
}

bool explosionUpdateFun(Particle* p, float dt) 
{
	float seconds = GetElapsedSeconds(p->spawnTimer);
	float overtime;
	p->tex = Animation_GetAt(&explosionAnim, seconds, &overtime);
	return overtime < 0.f;
}

void Explosion(mat4 transform, vec2 size) 
{
	Particle p;
	p.col = new_vec4_v(1.f);
	p.transform = mat4_Scale(transform, new_vec3_v2(size, 1.f));
	p.updateFun = explosionUpdateFun;
	p.tex = Animation_GetAt(&explosionAnim, 0, NULL);
	p.spawnTimer = MakeTimer();
	Particles_Emit(particles, p);
}

bool SpawnExplosion(Scene_t* scene, entity_t e, const void* d) 
{
	mat4* transform = Scene_Get(scene, e, Component_TRANSFORM);
	Explosion(*transform, new_vec2_v(2.f));
	return false;
}

void UpdatePlayer(Scene_t* scene, InputState* input, float dt)
{
	View_t view = View_Create(scene, 5, Component_TRANSFORM, Component_PLAYER, Component_MOVEMENT, Component_SPRITE, Component_PLANE);

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
	float thrust = thrust_idle;
	thrust *= input->Thrust;
	if (input->booster)
	{
		thrust = thrust_booster;

		//Booster animation.
		Timer_t timer = { 0 };
		sprite->overlays[0] = Animation_GetAt(&boosterAnim, GetElapsedSeconds(timer), NULL);

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

		Particle p;
		p.transform = mat4_Scale(mat4_Translate(*transform, new_vec3(-0.8f, 0.f, 0.f)), new_vec3(0.1f, 0.1f, 1.f));
		p.updateFun = defaultParticleMovementFun;
		p.col = col;
		p.tex = SubTexture_empty();
		p.spawnTimer = MakeTimer();
		p.acceleration = new_vec2_v(0.f);
		p.velocity = vec2_Add(vec2_Mul_s(mc->velocity, -0.5f), vec2_s_Mul(30.f, new_vec2(((float)rand() / (float)(RAND_MAX)) - 0.5f, ((float)rand() / (float)(RAND_MAX)) - 0.5f)));
		p.lifespan = 0.2f + ((float)rand() / (float)(RAND_MAX)) * 0.2f;

		if (GetElapsedSeconds(pc->boosterParticleTimer) > booster_particle_time)
		{
			pc->boosterParticleTimer = MakeTimer();
			Particles_Emit(particles, p);
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
		sprite->overlays[1] = Animation_GetAt(&cannonAnim, GetElapsedSeconds(timer), NULL);

		//Spawn bullet
		if (GetElapsedSeconds(pc->shootingTimer) > cannon_shooting_time)
		{
			pc->shootingTimer = MakeTimer();
			entity_t bullet = Scene_CreateEntity(scene);
			mat4* bullettransform = Scene_AddComponent(scene, bullet, Component_TRANSFORM);
			Sprite* bulletsprite = Scene_AddComponent(scene, bullet, Component_SPRITE);
			MovementComponent* bmc = Scene_AddComponent(scene, bullet, Component_MOVEMENT);		
			LifetimeComponent* lt = Scene_AddComponent(scene, bullet, Component_LIFETIME);	

			//Refresh pointers, otherwise we may read garbage data.
			mat4* transform = View_GetComponent(&view, 0);
			PlayerComponent* pc = View_GetComponent(&view, 1);
			MovementComponent* mc = View_GetComponent(&view, 2);
			PlaneComponent* pm = View_GetComponent(&view, 4);
			Sprite* sprite = View_GetComponent(&view, 3);

			//Init the components.
			vec2 forward = new_vec2_v4(mat4x4_Mul_v(*transform, new_vec4(1.f, 0.f, 0.f, 1.f)));
			vec3 pos = new_vec3_v4(mat4x4_Mul_v(*transform, new_vec4(0.f, 0.f, 0.f, 1.f)));
			vec2 direction = vec2_Normalize(vec2_Sub(forward, new_vec2_v3(pos)));
			*bullettransform = mat4_Translate(*transform, new_vec3(0.6f, 0.f, 0.f));

			*bulletsprite = Sprite_init();
			bulletsprite->size = new_vec2(0.3f, 0.3f);
			bulletsprite->subTex = bulletSubTex;
			bulletsprite->tintColor = new_vec4_v(1.f);


			bmc->velocity = vec2_Add(mc->velocity, vec2_Mul_s(direction, bullet_velocity));
			bmc->acceleration = new_vec2_v(0.f);

			lt->timer = MakeTimer();
			lt->lifetime = bullet_lifeTime;
			lt->userdata = NULL;
			lt->callback = NULL; //TODO go poof when done.
			lt->callback = SpawnExplosion;
		}
	}
	else
	{
		sprite->overlays[1] = SubTexture_empty();
	}
}

void UpdateCamera(Scene_t* scene, InputState* input) 
{
	//Camera.
	mat4* transform;
	entity_t camera = GetCamera(scene, &transform, NULL, NULL);

	//Get player.
	View_t v = View_Create(scene, 2, Component_TRANSFORM, Component_PLAYER);
	mat4* playertransform = View_GetComponent(&v, 0);

	//Set camera position.
	vec3 Pos = new_vec3_v4(mat4x4_Mul_v(*playertransform, new_vec4(0.f, 0.f, 0.f, 1.f))); //Get the position of the player.
	Pos = vec3_Add(Pos, new_vec3_v2(vec2_Mul_s(input->LookDir, input->Thrust * peek_scale), 0.f)); //Shift it towards the cursor.
	*transform = mat4_Translate(mat4x4_Identity(), Pos); //Set transfrom.
}

void MovePlanes(Scene_t* scene, float dt)
{
	for (View_t view = View_Create(scene, 3, Component_TRANSFORM, Component_PLANE, Component_MOVEMENT);
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
		vec2 gravity = vec2_Mul_s(new_vec2(0.f, -1.f), g * plane->mass); //gravity.

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

int main(int argc, char* argv[])
{
#ifdef _DEBUG
#endif // _DEBUG

	/* SDL inicializálása és ablak megnyitása */
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	window = SDL_CreateWindow("SDL peldaprogram",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		660, 480,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (window == NULL)
	{
		SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
		exit(1);
	}

	//Init gl
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) exit(-1);

	//GLEnableDebugOutput();

	tex = LoadTex2D("jet.png");
	atlas = TextureAtlas_create(tex, new_uvec2(64, 32));
	planeTex = TextureAtlas_SubTexture(&atlas, new_uvec2(0, 3), new_uvec2(1, 1));

	weaponsTex = LoadTex2D("weapons.png");
	weaponsAtlas = TextureAtlas_create(weaponsTex, new_uvec2(16, 16));
	bulletSubTex = TextureAtlas_SubTexture(&weaponsAtlas, new_uvec2(0, 1), new_uvec2(1, 1));

	explosionTex = LoadTex2D("Explosion.png");
	explosionAtlas = TextureAtlas_create(explosionTex, new_uvec2(96, 96));

	Animation_FromIni("BoosterAnim.ini", &boosterAnim, &atlas);
	Animation_FromIni("CannonAnim.ini", &cannonAnim, &atlas);
	Animation_FromIni("MissileAnim.ini", &missileAnim, &weaponsAtlas);
	Animation_FromIni("ExplosionAnim.ini", &explosionAnim, &explosionAtlas);

	//Add components.
	Scene_t* scene = Scene_New();
	RegisterTransform(scene);
	RegisterSprite(scene);
	RegisterCamera(scene);
	RegisterColloider(scene);
	RegisterMovement(scene);
	RegisterLifetime(scene);
	ComponentInfo_t playerInfo = COMPONENT_DEF(Component_PLAYER, PlayerComponent);
	Scene_AddComponentType(scene, playerInfo);
	ComponentInfo_t planeInfo = COMPONENT_DEF(Component_PLANE, PlaneComponent);
	Scene_AddComponentType(scene, planeInfo);

	Renderer2D renderer;
	Renderer2D_Init(&renderer);

	particles = Particles_New();

	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	glViewport(0, 0, w, h);
	float aspect = (float)w / (float)h;

	entity_t e = Scene_CreateEntity(scene);
	mat4* tr = Scene_AddComponent(scene, e, Component_TRANSFORM);
	Sprite* s = Scene_AddComponent(scene, e, Component_SPRITE);
	PlayerComponent* pc = Scene_AddComponent(scene, e, Component_PLAYER);
	PlaneComponent* pm = Scene_AddComponent(scene, e, Component_PLANE);
	MovementComponent* mc = Scene_AddComponent(scene, e, Component_MOVEMENT);

	*tr = mat4x4_Identity(), new_vec3_v(-1.f);;
	s->subTex = planeTex;
	s->tintColor = new_vec4_v(1.f);
	s->size = new_vec2(2.f, 1.0f);
	for (int i = 0; i < 5; i++) s->overlays[i] = SubTexture_empty();
	PlaneComponent temp = { lift_coeff, drag_coeff, thrust_booster, plane_mass };
	*pm = temp;
	mc->acceleration = new_vec2_v(0.f);
	mc->velocity = new_vec2_v(0.f);

	entity_t came = Scene_CreateEntity(scene);
	mat4* cam_tr = Scene_AddComponent(scene, came, Component_TRANSFORM);
	Camera* cam = Scene_AddComponent(scene, came, Component_CAMERA);
	*cam_tr = mat4x4_Identity();	
	*cam = mat4_Ortho(-aspect * viewport_scale, aspect * viewport_scale, viewport_scale, -viewport_scale, -1000, 1000);

	Timer_t timer = MakeTimer();

	InputState inputstate;

	EventDispatcher_t ev;
	bool exit = false;
	while (!exit)
	{
		//Events.
		while (GetEvent(&ev))
		{
			//Dispatch events.
			DispatchEvent(&ev, SDL_WINDOWEVENT, OnWindowEvent, scene);

			exit |= !ev.handled && ev.e.type == SDL_QUIT; //Exit once there is an unhandled QUIT event.
		}
		ResetUserEventMemory(); //Reset user event allocator.

		//Timing.
		float timediff = GetElapsedSeconds(timer);
		timer = MakeTimer();

		//Input.
		InputSnapshot_t snapshot = GetInput();	
		ParseInput(window, &snapshot, &inputstate);

		//Update.
		MovePlanes(scene, timediff);
		UpdatePlayer(scene, &inputstate, timediff);
		UpdateMovement(scene, timediff);
		UpdateLifetimes(scene);
		UpdateCamera(scene, &inputstate);
		Particles_Update(particles, timediff);

		//Rendering.
		mat4 camera;
		GetCamera(scene, &cam_tr, &cam, &camera);

		Renderer2D_Clear(&renderer, new_vec4_v(0.f)); //Clear screen.
		RenderSprites(scene, &renderer, camera); //Draw sprites.
		Particles_Draw(particles, &renderer, camera); //Draw particles.

		//Render test background.
		Renderer2D_BeginScene(&renderer, camera);
		vec2 camPos = new_vec2_v4(mat4x4_Mul_v(*cam_tr, new_vec4(0.f, 0.f, 0.f, 1.f)));
		for (float x = camPos.x - aspect * viewport_scale; x < camPos.x + aspect * viewport_scale; x++)
		{
			float x_ = floorf(x / 5.f) * 5.f;
			Renderer2D_DrawLine(&renderer,
				new_vec3(x_, camPos.y + viewport_scale, -1.f),
				new_vec3(x_, camPos.y - viewport_scale, -1.f),
				new_vec4_v(1.f)
			);
		}

		for (float y = camPos.y - viewport_scale; y < camPos.y + viewport_scale; y++)
		{
			float y_ = floorf(y / 5.f) * 5.f;
			Renderer2D_DrawLine(&renderer,
				new_vec3(camPos.x + aspect * viewport_scale, y_, -1.f),
				new_vec3(camPos.x - aspect * viewport_scale, y_, -1.f),
				new_vec4_v(1.f)
			);
		}
		Renderer2D_EndScene(&renderer);

		SDL_GL_SwapWindow(window);

		SDL_PumpEvents();
	}

	Scene_Delete(scene);
	GLTexture_Destroy(tex);
	GLTexture_Destroy(weaponsTex);
	GLTexture_Destroy(explosionTex);
	Particles_Delete(particles);
	Renderer2D_Destroy(&renderer);
	SDL_GL_DeleteContext(glcontext);
	SDL_Quit();

	Tree_ResetPool();

	return 0;
}