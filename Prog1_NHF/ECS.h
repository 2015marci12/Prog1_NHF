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
#define COMPONENT_DEF(id, type) { .id = id, .size = sizeof(type), .name = #type }

/*
* A way to map entity ids to a dense array of components. 
* As a bonus it provides a way to iterate entities that have this specific component type.
* 
* The idea comes from the ENTT library by skypjack, and the implementation closely resembles
* that of roig's destral_ecs library. Differences are:
	-allocation behaviour
	-the way component storages and component id-s relate to each other
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

SparseMap_t;

static SparseMap_t* SparseMap_Init(SparseMap_t* ptr);
static void SparseMap_Destroy(SparseMap_t* ptr);

static bool SparseMap_Contains(SparseMap_t* ptr, entity_t entity);
static size_t SparseMap_Index(SparseMap_t* ptr, entity_t entity);

static size_t SparseMap_Emplace(SparseMap_t* ptr, entity_t entity);
static size_t SparseMap_Remove(SparseMap_t* ptr, entity_t entity);

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

ComponentStorage_t* ComponentStorage_Init(ComponentStorage_t* ptr, ComponentInfo_t comp);
void ComponentStorage_Destroy(ComponentStorage_t* ptr);

bool ComponentStorage_Contains(ComponentStorage_t* ptr, entity_t entity);
void* ComponentStorage_GetByIndex(ComponentStorage_t* ptr, size_t index);
void* ComponentStorage_Get(ComponentStorage_t* ptr, entity_t entity);

void* ComponentStorage_Emplace(ComponentStorage_t* ptr, entity_t entity);
void ComponentStorage_Remove(ComponentStorage_t* ptr, entity_t entity);

//TODO scene, views
