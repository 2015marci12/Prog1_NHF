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
	Component_RELATIONSHIP, //Entity hierarchy.
	Component_COLLOIDER, //Fires collision events.
	Component_MOVEMENT, //moves according to the predefined characteristics.
	Component_LIFETIME, //destroys the entity after some time and calls the callback beforehand.
	Component_PHYSICS, //Physics

	Component_PLAYER,
	Component_PLANE,
	Component_HEALTH,
	Component_PROJECTILE,

	Component_GunTurretAI,
	Component_TankAI,
	Component_RadarStructureAI,
	Component_PlaneAI,
	Component_MissileGuidance,
	Component_MissileLauncer,
};

/*
* Transform
*/
void RegisterTransform(Scene_t* scene);
mat4 CalcWorldTransform(Scene_t* scene, entity_t e);

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
void RenderSprites(Scene_t* scene, Renderer2D* renderer);
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
	size_t children;
	entity_t parent;
	entity_t prevSibling;
	entity_t nextSibling;
	entity_t firstChild;
} Relationship;

void RegisterRelationship(Scene_t* scene);
Relationship Rel_init();

//Iteration / Query:

entity_t Parent(Scene_t* scene, entity_t e);
entity_t FirstChild(Scene_t* scene, entity_t e);
entity_t NextSibling(Scene_t* scene, entity_t e);
entity_t PrevSibling(Scene_t* scene, entity_t e);
bool RelationshipEnd(Scene_t* scene, entity_t e);

//Modification:

void RemoveChild(Scene_t* scene, entity_t parent, entity_t child);
void AddChild(Scene_t* scene, entity_t parent, entity_t child);
void RemoveChildren(Scene_t* scene, entity_t e);
//Recursively destroy all children and their hierarchies.
void KillChildren(Scene_t* scene, entity_t e);
void Orphan(Scene_t* scene, entity_t e);

/*
* AABB Colloider.
*/
typedef struct Colloider
{
	Rect body;
	uint64_t maskBits; //A mask that says what categories to check for collisions.
	uint64_t categoryBits; //A mask that says what categores the body belongs to.
	int32_t groupIndex; //0 will not check, negative only checks same indices, positive only checks different indices.
} Colloider;

typedef struct CollisionEvent
{
	entity_t a, b; //The two entities that colloided.
	vec2 normal; //The collision normal.
	float penetration; //The depth of the collision.
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
* Draw each and every colloider. Useful for debug purposes.
*/
void DebugDrawColloiders(Scene_t* scene, Renderer2D* renderer);

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

/*
* PhysicsBody.
*/

typedef struct PhysicsComponent
{
	float restitution;
	float mass; //0 mass means that the object is static.
	float inv_mass; //0 inverted mass means that the object is static.
} PhysicsComponent;

void RegisterPhysics(Scene_t* scene);
float CalcInvMass(float mass);
void PhysicsResolveCollision(Scene_t* scene, CollisionEvent* e);

void RegisterStandardComponents(Scene_t* scene);
