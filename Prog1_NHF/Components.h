#pragma once
#include "Core.h"
#include "Mathcell.h"
#include "ECS.h"
#include "Renderer2D.h"
#include "Animation.h"
#include "Input.h"
#include "Timer.h"

/*
* The types of components used in the game.
*/
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
	Component_BonusTowerAnim,
	Component_PowerUp,
	Component_FighterAI,

	Component_EnemyTag,
	Component_WallTag,
};

/*
* Transform
*/
void RegisterTransform(Scene_t* scene);
/*
* Calculate the world-space transform of an entity within a hierarchy.
*/
mat4 CalcWorldTransform(Scene_t* scene, entity_t e);

/*
* Sprite
*/

/*
* The number of animation overlays a sprite can have.
*/
#define MAX_ANIM_OVERLAY_COUNT 5

/*
* A structure representing a moving 2D graphical object.
*/
typedef struct Sprite
{
	vec4 tintColor; //The color to tint the texture with.
	vec2 size; //The size of the sprite.
	SubTexture subTex; //The texture itself.
	SubTexture overlays[MAX_ANIM_OVERLAY_COUNT]; //The textures of each overlay.
} Sprite;

//Register the sprite component type in a scene.
void RegisterSprite(Scene_t* scene);
/*
* Render every sprite in a scene to the screen.
*/
void RenderSprites(Scene_t* scene, Renderer2D* renderer);
/*
* Fill a sprite with default values.
*/
Sprite Sprite_init();

/*
* Camera
*/
/*
* A camera projection matrix.
*/
typedef mat4 Camera;
//Register the camera component type in a scene.
void RegisterCamera(Scene_t* scene);
//Get the primary camera in the scene and get its transform, projection, and model-view-projection matrices.
//NULL output parameters are ignored.
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
//Register the relationship component type in a scene.
void RegisterRelationship(Scene_t* scene);
//Fill a relationship component with default values.
Relationship Rel_init();

//Iteration / Query:
//Get the parent of an entity.
entity_t Parent(Scene_t* scene, entity_t e);
//Get the first child of an entity.
entity_t FirstChild(Scene_t* scene, entity_t e);
//Get the next sibling in a hierarchy.
entity_t NextSibling(Scene_t* scene, entity_t e);
//Get the previous sibling in a hierarchy.
entity_t PrevSibling(Scene_t* scene, entity_t e);
//Used for for loops. Checks if the entity in question is at the end of the iterable space.
//Since invalid -1 entityies are used to denote the end of a chain, this is equivalent to
//Scene_EntityValid().
bool RelationshipEnd(Scene_t* scene, entity_t e);

//Modification:

//Separate a child from its parent.
void RemoveChild(Scene_t* scene, entity_t parent, entity_t child);
//Adopt a child into the parent's hierarchy.
void AddChild(Scene_t* scene, entity_t parent, entity_t child);
//Remove all children of a parent entity.
void RemoveChildren(Scene_t* scene, entity_t e);
//Recursively destroy all children and their hierarchies.
void KillChildren(Scene_t* scene, entity_t e);
//Make an entity a root node with no parents.
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

/*
* An event fired when a collision happens in the scene.
*/
typedef struct CollisionEvent
{
	entity_t a, b; //The two entities that colloided.
	vec2 normal; //The collision normal.
	float penetration; //The depth of the collision.
} CollisionEvent;

//Register the colloider component type in a scene.
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
//Register the movement component type in a scene.
void RegisterMovement(Scene_t* scene);
//Update entities with movement components.
void UpdateMovement(Scene_t* scene, float dt);

/*
* Lifetime.
*/
//A callback taht can be fired when an entities lifetime expires.
//If it returns false, the entity is kept alive.
typedef bool(*LifetimeCallback_t)(Scene_t* scene, entity_t e, void* userdata);

/*
* A component that destroys an entity after a certain amount of time.
*/
typedef struct LifetimeComponent
{
	Timer_t timer;
	float lifetime;
	void* userdata;

	LifetimeCallback_t callback;
} LifetimeComponent;

//Register the lifetime component type in a scene.
void RegisterLifetime(Scene_t* scene);
//Update entities with lifetime components.
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

//Register the physics component type in a scene.
void RegisterPhysics(Scene_t* scene);
//Calculate the inverse mass given the mass of an object.
float CalcInvMass(float mass);
//Resolve a collision between physicsbodies.
void PhysicsResolveCollision(Scene_t* scene, CollisionEvent* e);

//Register all standard component types in a scene.
void RegisterStandardComponents(Scene_t* scene);
