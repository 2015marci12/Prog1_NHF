#include "Components.h"

void RegisterTransform(Scene_t* scene)
{
	ComponentInfo_t trinf = COMPONENT_DEF(Component_TRANSFORM, mat4);
	Scene_AddComponentType(scene, trinf);
}

void RegisterSprite(Scene_t* scene)
{
	ComponentInfo_t cinf = COMPONENT_DEF(Component_SPRITE, Sprite);
	Scene_AddComponentType(scene, cinf);
}

void RenderSprites(Scene_t* scene, Renderer2D* renderer, mat4 cameraMVP)
{
	Renderer2D_BeginScene(renderer, cameraMVP);

	//Render each renderable.
	for (View_t sprites = View_Create(scene, 2, Component_TRANSFORM, Component_SPRITE);
		!View_End(&sprites); View_Next(&sprites))
	{
		mat4* transform = View_GetComponent(&sprites, 0);
		Sprite* sprite = View_GetComponent(&sprites, 1);

		Renderer2D_DrawSprite(renderer, *transform, sprite->size, sprite->tintColor, sprite->subTex);
		for (int i = 0; i < MAX_ANIM_OVERLAY_COUNT; i++)
		{
			//Render overlay if applicable.
			if (sprite->overlays[i].texture != NULL)
			{
				mat4 tr = mat4_Translate(*transform, new_vec3(0.f, 0.f, i * 0.01f));
				Renderer2D_DrawSprite(renderer, tr, sprite->size, sprite->tintColor, sprite->overlays[i]);
			}
		}
	}

	Renderer2D_EndScene(renderer);
}

Sprite Sprite_init()
{
	Sprite ret;
	memset(&ret, 0, sizeof(Sprite));
	return ret;
}

void RegisterCamera(Scene_t* scene)
{
	ComponentInfo_t cinf = COMPONENT_DEF(Component_CAMERA, Camera);
	Scene_AddComponentType(scene, cinf);
}

entity_t GetCamera(Scene_t* scene, mat4** transform, mat4** projection, mat4* mvp)
{
	View_t v = View_Create(scene, 2, Component_TRANSFORM, Component_CAMERA);
	if (View_End(&v)) return -1;

	mat4* tr = View_GetComponent(&v, 0);
	mat4* proj = View_GetComponent(&v, 1);

	if (transform) { *transform = tr; }
	if (projection) { *projection = proj; }
	if (mvp) { *mvp = mat4x4x4_Mul(*proj, mat4_Inverse(*tr)); }

	return View_GetCurrent(&v);
}

uint32_t collisionEventType = 0;

void FireCollisionEvents(Scene_t* scene)
{
	//check everything against everything. Not too efficient but this is the simplest solution.
	//Should be enough for a simple game.
	//TODO: switch to spatial partitioning once performance becomes a problem.
	//TODO: custom event queue if the 65536 max events in the SDL queue becomes the limiting factor somehow.
	for (View_t outter = View_Create(scene, 2, Component_TRANSFORM, Component_COLLOIDER);
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
				(colloider1->layermask & 1ull << (colloider2->layer - 1))
				&&
				(colloider2->layermask & 1ull << (colloider1->layer - 1))
				)
			{
				//Calculate the transformed AABBs.
				vec2 aPos = new_vec2_v4(mat4x4_Mul_v(*transform1, new_vec4_v2(colloider1->body.Pos, 0.f, 1.f))),
					bPos = new_vec2_v4(mat4x4_Mul_v(*transform2, new_vec4_v2(colloider2->body.Pos, 0.f, 1.f)));
				Rect aMov = new_Rect_ps(aPos, colloider1->body.Size),
					bMov = new_Rect_ps(bPos, colloider2->body.Size);

				//Register event if necessary.
				if (!collisionEventType) collisionEventType = SDL_RegisterEvents(1);

				//Fill out event and check collision.
				CollisionEvent e;
				e.a = View_GetCurrent(&outter);
				e.b = View_GetCurrent(&inner);
				if (Rect_Intersects(aMov, bMov, &e.normal, &e.penetration))
				{
					//Fire collision event. data1: the event parameters; data2: a pointer to the scene.
					int ret = PushEvent(NULL, collisionEventType, 0, &e, sizeof(CollisionEvent), scene);
					//Report warning if unsuccessful.
					if (ret != 1) WARN("Collision events not handled: Failed to push event. Error code %d\n", ret);
				}
			}
			else
			{
				continue; //Layer masks incompatible.
			}
		}
	}
}
