#pragma once
#include "Core.h"
#include "Mathcell.h"
#include "ECS.h"
#include "Renderer2D.h"
#include "Animation.h"

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

void FireCollisionEvents(Scene_t* scene) 
{
	//check everything against everything. Not too efficient but this is the simplest solution.
	//Should be enough for a simple game.
	//TODO: switch to spatial partitioning once performance becomes a problem.
	for(View_t outter = View_Create(scene, 2, Component_TRANSFORM, Component_COLLOIDER);
		!View_End(&outter); View_Next(&outter)) 
	{
		for (View_t inner = View_Create(scene, 2, Component_TRANSFORM, Component_COLLOIDER);
			!View_End(&inner); View_Next(&inner))
		{
			//Avoid checking entites with themselves or duplicate checks.
			if (View_GetCurrentIndex(&outter) > View_GetCurrentIndex(&inner)
				&& View_GetCurrent(&outter) == View_GetCurrent(&inner)) //Should be redundant, still here just to be safe.
			{
				continue;
			}

			mat4* transform1 = View_GetComponent(&outter, 0);
			mat4* transform2 = View_GetComponent(&inner, 0);
			Colloider* colloider1 = View_GetComponent(&outter, 1);
			Colloider* colloider2 = View_GetComponent(&inner, 1);

			//Check collision layers.
			if (
				(colloider1->layermask & 1 << (colloider2->layer - 1))
				&& 
				(colloider2->layermask & 1 << (colloider1->layer - 1))
				) 
			{
				//TODO generate collision manifold and fire event in SDL
				// TODO create a per-frame linear allocator for event data. (cascading)
				// the user will clear this each frame after processing the events.
				// This will allow us to use the SDL event queue for our own purposes.
				//SDL_PushEvent()
			} 
			else
			{
				continue; //Layer masks incompatible.
			}
		}
	}
}
