#pragma once
#include "Core.h"
#include "Mathcell.h"
#include "ECS.h"
#include "Renderer2D.h"
#include "Animation.h"
#include "Input.h"
#include "Timer.h"

enum ComponentTypes
{
	Component_NONE = 0,
	Component_TRANSFORM, //The world space transform of each entity.
	Component_SPRITE, //A simple 2d image with some overlays.
	Component_CAMERA, //The camera the scene will be rendered with.
	Component_COLLOIDER, //Fires collision events.
	Component_MOVEMENT, //moves according to the predefined characteristics.
	Component_LIFETIME, //destroys the entity after some time and calls the callback beforehand.
	Component_PLAYER,
	Component_PLANE,
};

/*
* Transform
*/
void RegisterTransform(Scene_t* scene);


/*
* Sprite
*/
#define MAX_ANIM_OVERLAY_COUNT 5

typedef struct Sprite
{
	vec4 tintColor;
	vec2 size;
	SubTexture subTex;
	SubTexture overlays[MAX_ANIM_OVERLAY_COUNT];
} Sprite;

void RegisterSprite(Scene_t* scene);
void RenderSprites(Scene_t* scene, Renderer2D* renderer, mat4 cameraMVP);
Sprite Sprite_init();


/*
* Camera
*/
typedef mat4 Camera;
void RegisterCamera(Scene_t* scene);
entity_t GetCamera(Scene_t* scene, mat4** transform, mat4** projection, mat4* mvp);


/*
* Relationships (entity hierarchy)
*/
typedef struct Relationship
{
	entity_t parent;
	entity_t prevSibling;
	entity_t nextSibling;
	entity_t firstChild;
} Relationship;

//TODO

/*
* AABB Colloider.
*/
typedef struct Colloider 
{
	Rect body;
	uint64_t layermask; //A mask that what layers to check for collisions.
	uint32_t layer : 6; //64 layers.
} Colloider;

typedef struct CollisionEvent 
{
	entity_t a, b; //The two entities that colloided.
	vec2 normal; //The collision normal.
	float penetration; //The depth of the collision.
	uint32_t AWantedToColloideWithB : 1; //Whether b is in a's layer mask.
	uint32_t BWantedToColloideWithA : 1; //Whether a is in b's layer mask.
} CollisionEvent;

void RegisterColloider(Scene_t* scene);
/*
* Get the sdl event type code for collisions.
*/
uint32_t CollisionEventType();
/*
* Fire collision events in the scene.
*/
void FireCollisionEvents(Scene_t* scene);

/*
* Movement.
*/
typedef struct MovementComponent 
{
	vec2 velocity;
	vec2 acceleration;
} MovementComponent;

void RegisterMovement(Scene_t* scene);
void UpdateMovement(Scene_t* scene, float dt);

/*
* Lifetime.
*/
typedef bool(*LifetimeCallback_t)(Scene_t* scene, entity_t e, const void* userdata);
typedef struct LifetimeComponent 
{
	Timer_t timer;
	float lifetime;
	const void* userdata;

	LifetimeCallback_t callback;
} LifetimeComponent;

void RegisterLifetime(Scene_t* scene);
void UpdateLifetimes(Scene_t* scene);
