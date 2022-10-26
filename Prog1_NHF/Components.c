#include "Components.h"

void RegisterTransform(Scene_t* scene)
{
	ComponentInfo_t trinf = COMPONENT_DEF(Component_TRANSFORM, mat4);
	Scene_AddComponentType(scene, trinf);
}

mat4 CalcWorldTransform(Scene_t* scene, entity_t e)
{
	mat4 transform = mat4x4_Identity();
	for (entity_t entity = e; Scene_EntityValid(scene, entity); entity = Parent(scene, entity)) 
	{
		mat4* tr = Scene_Get(scene, entity, Component_TRANSFORM);
		if(tr) transform = mat4x4x4_Mul(*tr, transform);
	}
	return transform;
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
		mat4 transform = CalcWorldTransform(scene, View_GetCurrent(&sprites));
		Sprite* sprite = View_GetComponent(&sprites, 1);

		Renderer2D_DrawSprite(renderer, transform, sprite->size, sprite->tintColor, sprite->subTex);
		for (int i = 0; i < MAX_ANIM_OVERLAY_COUNT; i++)
		{
			//Render overlay if applicable.
			if (sprite->overlays[i].texture != NULL)
			{
				Renderer2D_DrawSprite(renderer, transform, sprite->size, sprite->tintColor, sprite->overlays[i]);
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

void RegisterRelationship(Scene_t* scene)
{
	ComponentInfo_t cinf = COMPONENT_DEF(Component_RELATIONSHIP, Relationship);
	Scene_AddComponentType(scene, cinf);
}

Relationship Rel_init()
{
	Relationship ret;
	ret.children = 0;
	ret.parent = -1;
	ret.prevSibling = -1;
	ret.nextSibling = -1;
	ret.firstChild = -1;
	return ret;
}

entity_t Parent(Scene_t* scene, entity_t e)
{
	Relationship* rel = Scene_Get(scene, e, Component_RELATIONSHIP);
	if (!rel) return -1;
	return rel->parent;
}

entity_t FirstChild(Scene_t* scene, entity_t e)
{
	Relationship* rel = Scene_Get(scene, e, Component_RELATIONSHIP);
	if (!rel) return -1;
	return rel->firstChild;
}

entity_t NextSibling(Scene_t* scene, entity_t e)
{
	Relationship* rel = Scene_Get(scene, e, Component_RELATIONSHIP);
	if (!rel) return -1;
	return rel->nextSibling;
}

entity_t PrevSibling(Scene_t* scene, entity_t e)
{
	Relationship* rel = Scene_Get(scene, e, Component_RELATIONSHIP);
	if (!rel) return -1;
	return rel->prevSibling;
}

bool RelationshipEnd(Scene_t* scene, entity_t e) { return !Scene_EntityValid(scene, e); }

void RemoveChild(Scene_t* scene, entity_t parent, entity_t child)
{
	ASSERT(Scene_EntityValid(scene, parent) && Scene_EntityValid(scene, parent), "Parent and child must be valid!\n");
	Relationship* pr = Scene_Get(scene, parent, Component_RELATIONSHIP);
	Relationship* cr = Scene_Get(scene, child, Component_RELATIONSHIP);
	ASSERT(pr && cr, "Parent and child must have relationship components!\n");
	//First child case.
	if (pr->firstChild == child)
	{
		pr->firstChild = cr->nextSibling;
		if (!RelationshipEnd(scene, cr->nextSibling))
		{
			Relationship* nsr = Scene_Get(scene, cr->nextSibling, Component_RELATIONSHIP);
			nsr->prevSibling = -1;
		}
	}
	else
	{
		//Otherwise tie the doubly linked list together.
		if (!RelationshipEnd(scene, cr->nextSibling))
		{
			Relationship* nsr = Scene_Get(scene, cr->nextSibling, Component_RELATIONSHIP);
			nsr->prevSibling = cr->prevSibling;
		}
		if (!RelationshipEnd(scene, cr->prevSibling))
		{
			Relationship* psr = Scene_Get(scene, cr->prevSibling, Component_RELATIONSHIP);
			psr->nextSibling = cr->nextSibling;
		}
	}
	cr->parent = -1;
	pr->children--;
}

void AddChild(Scene_t* scene, entity_t parent, entity_t child)
{
	Relationship default_rel = Rel_init();
	ASSERT(Scene_EntityValid(scene, parent) && Scene_EntityValid(scene, parent), "Parent and child must be valid!\n");
	Relationship* pr = Scene_Get_Or_Emplace(scene, parent, Component_RELATIONSHIP, &default_rel);
	Relationship* cr = Scene_Get_Or_Emplace(scene, child, Component_RELATIONSHIP, &default_rel);

	//Remove from previous relationship.
	if (Scene_EntityValid(scene, cr->parent))
	{
		RemoveChild(scene, cr->parent, child);
	}

	//Add to the beginning of the new linked list.
	cr->parent = parent;
	cr->nextSibling = pr->firstChild;
	pr->firstChild = child;
	pr->children++;
}

void RemoveChildren(Scene_t* scene, entity_t e)
{
	entity_t next = -1;
	for (entity_t current = FirstChild(scene, e); Scene_EntityValid(scene, current); current = next) 
	{
		next = NextSibling(scene, current);
		RemoveChild(scene, e, current);
	}
}

void KillChildren(Scene_t* scene, entity_t e)
{
	entity_t next = -1;
	for (entity_t current = FirstChild(scene, e); Scene_EntityValid(scene, current); current = next)
	{
		next = NextSibling(scene, current);
		RemoveChild(scene, e, current);
		KillChildren(scene, current);
		Scene_DeleteEntity(scene, current);
	}
}

void Orphan(Scene_t* scene, entity_t e)
{
	entity_t parent = Parent(scene, e);
	if (Scene_EntityValid(scene, e)) RemoveChild(scene, parent, e);
}

uint32_t collisionEventType = 0;

void RegisterColloider(Scene_t* scene)
{
	ComponentInfo_t cinf = COMPONENT_DEF(Component_COLLOIDER, Colloider);
	Scene_AddComponentType(scene, cinf);
}

uint32_t CollisionEventType()
{
	//Register event if necessary.
	if (!collisionEventType) collisionEventType = SDL_RegisterEvents(1);
	return collisionEventType;
}

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

			//Get components.
			mat4 transform1 = CalcWorldTransform(scene, View_GetCurrent(&outter));
			mat4 transform2 = CalcWorldTransform(scene, View_GetCurrent(&inner));
			Colloider* colloider1 = View_GetComponent(&outter, 1);
			Colloider* colloider2 = View_GetComponent(&inner, 1);

			//Prefill predictable event data.
			CollisionEvent e = {0};
			e.a = View_GetCurrent(&outter);
			e.b = View_GetCurrent(&inner);

			//Check collision layers.
			e.AWantedToColloideWithB = colloider1->layermask & BIT(colloider2->layer - 1);
			e.BWantedToColloideWithA = colloider2->layermask & BIT(colloider1->layer - 1);
			//Same layer-layermask logic that is used in the GODOT engine, because that sounded useful.

			if (e.AWantedToColloideWithB || e.BWantedToColloideWithA) //Check collison if either entity wants to check the other.
			{
				//Calculate the transformed AABBs. TODO may want to recalculate the size according to rotation.
				vec2 aPos = new_vec2_v4(mat4x4_Mul_v(transform1, new_vec4_v2(colloider1->body.Pos, 0.f, 1.f))),
					bPos = new_vec2_v4(mat4x4_Mul_v(transform2, new_vec4_v2(colloider2->body.Pos, 0.f, 1.f)));
				Rect aMov = new_Rect_ps(aPos, colloider1->body.Size),
					bMov = new_Rect_ps(bPos, colloider2->body.Size);

				//Fill out event manifold and check collision.			
				if (Rect_Intersects(aMov, bMov, &e.normal, &e.penetration))
				{
					//Fire collision event. data1: the event parameters; data2: a pointer to the scene.
					int ret = PushEvent(NULL, CollisionEventType(), 0, &e, sizeof(CollisionEvent), scene);
					//Report warning if the push is unsuccessful.
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

void RegisterMovement(Scene_t* scene)
{
	ComponentInfo_t cinf = COMPONENT_DEF(Component_MOVEMENT, MovementComponent);
	Scene_AddComponentType(scene, cinf);
}

void UpdateMovement(Scene_t* scene, float dt)
{
	for (View_t v = View_Create(scene, 2, Component_TRANSFORM, Component_MOVEMENT);
		!View_End(&v);  View_Next(&v))
	{
		mat4* transform = View_GetComponent(&v, 0);
		MovementComponent* movement = View_GetComponent(&v, 1);

		*transform = mat4x4x4_Mul(mat4_Translate(mat4x4_Identity(), new_vec3_v2(vec2_Mul_s(movement->velocity, dt), 0.f)), *transform);
		movement->velocity = vec2_Add(movement->velocity, vec2_Mul_s(movement->acceleration, dt));
	}
}

void RegisterLifetime(Scene_t* scene)
{
	ComponentInfo_t cinf = COMPONENT_DEF(Component_LIFETIME, LifetimeComponent);
	Scene_AddComponentType(scene, cinf);
}

void UpdateLifetimes(Scene_t* scene)
{
	View_t v = View_Create(scene, 1, Component_LIFETIME);
	while (!View_End(&v))
	{
		LifetimeComponent* lifetime = View_GetComponent(&v, 0);
		if (GetElapsedSeconds(lifetime->timer) >= lifetime->lifetime)
		{
			if (!lifetime->callback) View_DestroyCurrent_FindNext(&v);
			else if (!(lifetime->callback)(scene, View_GetCurrent(&v), lifetime->userdata)) View_DestroyCurrent_FindNext(&v);
			else View_Next(&v);
		}
		else View_Next(&v);
	}
}

void RegisterStandardComponents(Scene_t* scene)
{
	RegisterTransform(scene);
	RegisterSprite(scene);
	RegisterCamera(scene);
	RegisterRelationship(scene);
	RegisterColloider(scene);
	RegisterMovement(scene);
	RegisterLifetime(scene);
}
