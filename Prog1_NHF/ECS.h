#pragma once
#include "Core.h"
#include "Container.h"

typedef uint32_t entity_t; //The unique identifier of an entity at runtime. Should not be used to save relationships.
typedef uint32_t componentid_t; //The id of the component type.

typedef struct ComponentInfo_t //Runtime information about a component type.
{
	componentid_t id;
	size_t size;
	const char* name;
} ComponentInfo_t;

//Helper to define component types.
#define COMPONENT_DEF(cid, type) { .id = cid, .size = sizeof(type), .name = #type }

/*
* A way to map entity ids to a dense array of components. 
* As a bonus it provides a way to iterate entities that have this specific component type.
* 
* The idea comes from the ENTT library by skypjack, and the implementation closely resembles
* that of roig's destral_ecs library. Differences are:
	-allocation behaviour
	-the way component storages and component id-s relate to each other. This implementation uses a binary tree to look up component storages.
	-the way views iterate the most exclusive set of the component types.
	-no entity versioning.
*/
typedef struct SparseMap_t 
{
	entity_t* sparse;
	size_t sparse_size;

	entity_t* dense;
	size_t dense_size;
	size_t dense_capacity;
} SparseMap_t;

//Initialize a newly created sparse map.
SparseMap_t* SparseMap_Init(SparseMap_t* ptr);
//Free the resources allocated by the sparse map.
void SparseMap_Destroy(SparseMap_t* ptr);
//Allocate a new sparse map on the heap.
SparseMap_t* SparseMap_New();
//Free the resources allocated by the sparse map as well as the map itself.
void SparseMap_Delete(SparseMap_t* ptr);

//Check if an entity has a place in the map.
bool SparseMap_Contains(SparseMap_t* ptr, entity_t entity);
//Get the index of the entity in the dense set.
size_t SparseMap_Index(SparseMap_t* ptr, entity_t entity);

//Emplace a new entity in the map. Does not guarantee pointer retention.
size_t SparseMap_Emplace(SparseMap_t* ptr, entity_t entity);
//Remove a new entity from the map. Does not guarantee pointer reteintion.
size_t SparseMap_Remove(SparseMap_t* ptr, entity_t entity);

/*
* The array that actually stores component data. 
* Placements and indices managed by the sparse map.
* Also stores information about the type of component it holds.
*/
typedef struct ComponentStorage_t 
{
	SparseMap_t sparse;
	ComponentInfo_t comp;
	size_t comp_count;
	size_t data_capacity;
	void* components;
} ComponentStorage_t;

//Initialize a newly created storage.
ComponentStorage_t* ComponentStorage_Init(ComponentStorage_t* ptr, ComponentInfo_t comp);
//Free the resources allocated by a component storage.
void ComponentStorage_Destroy(ComponentStorage_t* ptr);
//Allocate a new storage on the heap.
ComponentStorage_t* ComponentStorage_New(ComponentInfo_t comp);
//Free a heap allocated component storage.
void ComponentStorage_Delete(ComponentStorage_t* ptr);

//Check if an entity has a component in the storage.
bool ComponentStorage_Contains(ComponentStorage_t* ptr, entity_t entity);
//Get the nth component in the dense storage.
void* ComponentStorage_GetByIndex(ComponentStorage_t* ptr, size_t index);
//Get the component associated with an entity.
void* ComponentStorage_Get(ComponentStorage_t* ptr, entity_t entity);

//Add a component and associate it with an entity.
void* ComponentStorage_Emplace(ComponentStorage_t* ptr, entity_t entity);
//Remove the component associated with an entity.
void ComponentStorage_Remove(ComponentStorage_t* ptr, entity_t entity);

/*
* 
*/
typedef struct Scene_t 
{
	entity_t* entities;
	size_t size;
	entity_t freelist;

	TreeNode_t* storageTree;
} Scene_t;

Scene_t* Scene_Init(Scene_t* ptr);
void Scene_Destroy(Scene_t* ptr);
Scene_t* Scene_New();
void Scene_Delete(Scene_t* ptr);

ComponentStorage_t* Scene_GetStorage(Scene_t* ptr, componentid_t cid);
ComponentStorage_t* Scene_AddComponentType(Scene_t* ptr, ComponentInfo_t comp);

entity_t Scene_CreateEntity(Scene_t* ptr);
bool Scene_EntityValid(Scene_t* ptr, entity_t entity);
void Scene_DeleteEntity(Scene_t* ptr, entity_t entity);

bool Scene_Has(Scene_t* ptr, entity_t entity, componentid_t cid);
void* Scene_Get(Scene_t* ptr, entity_t entity, componentid_t cid);
void* Scene_AddComponent(Scene_t* ptr, entity_t entity, componentid_t cid);
void Scene_RemoveComponent(Scene_t* ptr, entity_t entity, componentid_t cid);

typedef struct View_t 
{
#define VIEW_MAX_COMPONENTS 32
	ComponentStorage_t* storages[VIEW_MAX_COMPONENTS];
	size_t components;

	int smallestSetIndex;
	size_t currentIndex;
	entity_t currentEntity;
} View_t;

View_t View_Create(Scene_t* scene, size_t numComponents, ...);
bool View_HasAll(View_t* ptr);
bool View_End(View_t* ptr);
void View_Next(View_t* ptr);
void View_Reset(View_t* ptr);
void* View_GetComponent(View_t* ptr, uint32_t CompIndex);
entity_t View_GetCurrent(View_t* ptr);
