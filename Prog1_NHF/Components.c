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
