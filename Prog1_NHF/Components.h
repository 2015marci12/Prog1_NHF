#pragma once
#include "Core.h"
#include "Mathcell.h"
#include "ECS.h"
#include "Renderer2D.h"
#include "Animation.h"
#include "Input.h"

enum ComponentTypes
{
	Component_NONE = 0,
	Component_TRANSFORM,
	Component_SPRITE,
	Component_CAMERA,
	Component_COLLOIDER,
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
	entity_t a, b;
	vec2 normal;
	float penetration;
} CollisionEvent;

void FireCollisionEvents(Scene_t* scene);
