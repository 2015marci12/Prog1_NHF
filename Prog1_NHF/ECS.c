#include "ECS.h"

SparseMap_t* SparseMap_Init(SparseMap_t* ptr)
{
	if (ptr)
	{
		ptr->sparse = NULL;
		ptr->sparse_size = 0;

		ptr->dense = NULL;
		ptr->dense_size = 0;
		ptr->dense_capacity = 0;
	}
	return ptr;
}

void SparseMap_Destroy(SparseMap_t* ptr)
{
	if (ptr)
	{
		free(ptr->sparse);
		free(ptr->dense);
	}
}

SparseMap_t* SparseMap_New()
{
	return SparseMap_Init(malloc(sizeof(SparseMap_t)));
}

void SparseMap_Delete(SparseMap_t* ptr)
{
	SparseMap_Destroy(ptr);
	free(ptr);
}

bool SparseMap_Contains(SparseMap_t* ptr, entity_t entity)
{
	ASSERT(ptr, "SparseMap_Contains does not permit ptr to be NULL!");
	ASSERT(entity != -1, "SparseMap_Contains does not permit entity to be invalid (-1)!");
	return (entity < ptr->sparse_size) && (ptr->sparse[entity] != -1);
}

size_t SparseMap_Index(SparseMap_t* ptr, entity_t entity)
{
	ASSERT(SparseMap_Contains(ptr, entity), "SparseMap_Index cannot return an index for entites not in the sparse map ptr!");
	return ptr->sparse[entity];
}

size_t SparseMap_Emplace(SparseMap_t* ptr, entity_t entity)
{
	ASSERT(ptr, "SparseMap_Emplace does not permit ptr to be NULL!");
	ASSERT(entity != -1, "SparseMap_Emplace does not permit entity to be invalid (-1)!");
	if (entity >= ptr->sparse_size) //Realloc sparse if needed.
	{
		size_t newSize = entity + 1;
		ptr->sparse = (entity_t*)realloc(ptr->sparse, newSize * sizeof(entity_t));
		memset(ptr->sparse + ptr->sparse_size, UINT32_MAX, newSize - ptr->sparse_size);
		ptr->sparse_size = newSize;
	}
	ptr->sparse[entity] = (entity_t)ptr->dense_size;
	if (ptr->dense_size >= ptr->dense_capacity) //Realloc dense if needed.
	{
		size_t newCap = max(ptr->dense_capacity * 2, 32);
		ptr->dense = (entity_t*)realloc(ptr->dense, newCap * sizeof(entity_t));
		memset(ptr->dense + ptr->dense_capacity, UINT32_MAX, newCap - ptr->dense_capacity);
		ptr->dense_capacity = newCap;
	}
	ptr->dense[ptr->dense_size] = entity;

	return ptr->dense_size++;
}

size_t SparseMap_Remove(SparseMap_t* ptr, entity_t entity)
{
	ASSERT(ptr, "SparseMap_Remove does not permit ptr to be NULL!");
	ASSERT(SparseMap_Contains(ptr, entity), "SparseMap_Remove cannot remove an entity not contained in the map!");

	size_t pos = SparseMap_Index(ptr, entity);
	entity_t end = ptr->dense[ptr->dense_size - 1];

	ptr->sparse[end] = (entity_t)pos;
	ptr->dense[pos] = end;
	ptr->sparse[entity] = UINT32_MAX;

	ptr->dense[ptr->dense_size] = UINT32_MAX;
	ptr->dense_size--;

	return pos;
}

ComponentStorage_t* ComponentStorage_Init(ComponentStorage_t* ptr, ComponentInfo_t comp)
{
	if (ptr)
	{
		SparseMap_Init(&ptr->sparse);
		ptr->comp = comp;
		ptr->comp_count = 0;
		ptr->data_capacity = 0;
		ptr->components = NULL;
	}
	return ptr;
}

void ComponentStorage_Destroy(ComponentStorage_t* ptr)
{
	if (ptr)
	{
		SparseMap_Destroy(&ptr->sparse);
		free(ptr->components);
	}
}

ComponentStorage_t* ComponentStorage_New(ComponentInfo_t comp)
{
	return ComponentStorage_Init((ComponentStorage_t*)malloc(sizeof(ComponentStorage_t)), comp);
}

void ComponentStorage_Delete(ComponentStorage_t* ptr)
{
	ComponentStorage_Destroy(ptr);
	free(ptr);
}

bool ComponentStorage_Contains(ComponentStorage_t* ptr, entity_t entity)
{
	ASSERT(ptr, "ComponentStorage_Contains does not permit ptr to be NULL");
	ASSERT(entity != -1, "ComponentStorage_Contains does not permit entity to be invalid (-1)!");
	return SparseMap_Contains(&ptr->sparse, entity);
}

void* ComponentStorage_GetByIndex(ComponentStorage_t* ptr, size_t index)
{
	ASSERT(ptr, "ComponentStorage_GetByIndex does not permit ptr to be NULL");
	ASSERT((index < ptr->comp_count), "ComponentStorage_GetByIndex index out of range!");
	return &((char*)ptr->components)[ptr->comp.size * index];
}

void* ComponentStorage_Get(ComponentStorage_t* ptr, entity_t entity)
{
	ASSERT(ptr, "ComponentStorage_Get does not permit ptr to be NULL");
	ASSERT(entity != -1, "ComponentStorage_Get does not permit entity to be invalid (-1)!");
	if (!ComponentStorage_Contains(ptr, entity)) return NULL;
	return ComponentStorage_GetByIndex(ptr, SparseMap_Index(&ptr->sparse, entity));
}

void* ComponentStorage_Emplace(ComponentStorage_t* ptr, entity_t entity)
{
	ASSERT(ptr, "ComponentStorage_Emplace does not permit ptr to be NULL");
	ASSERT(entity != -1, "ComponentStorage_Emplace does not permit entity to be invalid (-1)!");
	size_t pos = SparseMap_Emplace(&ptr->sparse, entity);
	size_t cap = ptr->sparse.dense_capacity * ptr->comp.size;
	if (cap > ptr->data_capacity) ptr->components = realloc(ptr->components, cap);
	ptr->comp_count++;
	return ComponentStorage_GetByIndex(ptr, pos);
}

void ComponentStorage_Remove(ComponentStorage_t* ptr, entity_t entity)
{
	ASSERT(ptr, "ComponentStorage_Remove does not permit ptr to be NULL");
	ASSERT(entity != -1, "ComponentStorage_Remove does not permit entity to be invalid (-1)!");
	size_t pos = SparseMap_Remove(&ptr->sparse, entity);
	size_t end = ptr->comp_count - 1;
	memmove(
		ComponentStorage_GetByIndex(ptr, pos),
		ComponentStorage_GetByIndex(ptr, end),
		ptr->comp.size
	);
	ptr->comp_count--;
}

static void Scene_TreeFree(TreeNode_t* storages)
{
	if (!storages) return;
	ComponentStorage_Delete(storages->data);
	Scene_TreeFree(storages->left);
	Scene_TreeFree(storages->right);
}

Scene_t* Scene_Init(Scene_t* ptr)
{
	if (ptr)
	{
		ptr->freelist = -1;
		ptr->entities = NULL;
		ptr->size = 0;
		ptr->storageTree = NULL;
	}
	return ptr;
}

void Scene_Destroy(Scene_t* ptr)
{
	if (ptr)
	{
		free(ptr->entities);
		Scene_TreeFree(ptr->storageTree);
		Tree_Clear(ptr->storageTree);
	}
}

Scene_t* Scene_New()
{
	return Scene_Init(malloc(sizeof(Scene_t)));
}

void Scene_Delete(Scene_t* ptr)
{
	Scene_Destroy(ptr);
	free(ptr);
}

ComponentStorage_t* Scene_GetStorage(Scene_t* ptr, componentid_t cid)
{
	ASSERT(ptr, "Scene_GetStorage does not permit ptr to be NULL");
	TreeNode_t* storage = Tree_Find(ptr->storageTree, cid);
	return storage ? (ComponentStorage_t*)storage->data : NULL;
}

ComponentStorage_t* Scene_AddComponentType(Scene_t* ptr, ComponentInfo_t comp)
{
	ASSERT(ptr, "Scene_AddComponentType does not permit ptr to be NULL");
	ASSERT(!Tree_Find(ptr->storageTree, comp.id), " Scene_AddComponentType cannot add a component type that already has storage!");
	ComponentStorage_t* storage = ComponentStorage_New(comp);
	ptr->storageTree = Tree_Insert(ptr->storageTree, comp.id, storage);
	return storage;
}

entity_t Scene_CreateEntity(Scene_t* ptr)
{
	ASSERT(ptr, "Scene_CreateEntity does not permit ptr to be NULL");
	if (ptr->freelist != -1) //We have new candidates in the freelist.
	{
		entity_t ret = ptr->freelist;
		ptr->freelist = ptr->entities[ret];
		ptr->entities[ret] = -1;
		return ret;
	}

	//Expand the entity array.
	size_t newsize = max(ptr->size * 2, 1024);
	ptr->entities = realloc(ptr->entities, newsize * sizeof(entity_t));
	for (entity_t i = newsize - 1; i > ptr->size; i--)
	{
		ptr->entities[i] = ptr->freelist;
		ptr->freelist = i;
	}
	ptr->entities[ptr->size] = ptr->freelist;
	ptr->freelist = ptr->size;
	ptr->size = newsize;

	//Recurse.
	return Scene_CreateEntity(ptr);
}

bool Scene_EntityValid(Scene_t* ptr, entity_t entity)
{
	ASSERT(ptr, "Scene_EntityValid does not permit ptr to be NULL");
	return (entity != -1) && (entity < ptr->size) && (ptr->entities[entity] == -1);
}

static void Scene_DeleteAllComponents(TreeNode_t* compTree, entity_t e)
{
	if (!compTree) return;
	ComponentStorage_t* storage = compTree->data;
	ASSERT(storage, "Scene_DeleteAllComponents: Invalid node on component type tree!");
	if(ComponentStorage_Contains(storage, e)) ComponentStorage_Remove(storage, e);
	Scene_DeleteAllComponents(compTree->left, e);
	Scene_DeleteAllComponents(compTree->right, e);
}


void Scene_DeleteEntity(Scene_t* ptr, entity_t entity)
{
	ASSERT(ptr, "Scene_DeleteEntity does not permit ptr to be NULL");
	ASSERT(Scene_EntityValid(ptr, entity), "Scene_DeleteEntity: entity must be valid!");
	Scene_DeleteAllComponents(ptr->storageTree, entity);
	ptr->entities[entity] = ptr->freelist;
	ptr->freelist = entity;
}

bool Scene_Has(Scene_t* ptr, entity_t entity, componentid_t cid)
{
	ASSERT(ptr, "Scene_Has does not permit ptr to be NULL");
	ASSERT(Scene_EntityValid(ptr, entity), "Scene_Has: entity must be valid!");
	ComponentStorage_t* storage = Scene_GetStorage(ptr, cid);
	return storage && ComponentStorage_Contains(storage, entity);
}

void* Scene_Get(Scene_t* ptr, entity_t entity, componentid_t cid)
{
	ASSERT(ptr, "Scene_Get does not permit ptr to be NULL");
	ASSERT(Scene_EntityValid(ptr, entity), "Scene_Get: entity must be valid!");
	ComponentStorage_t* storage = Scene_GetStorage(ptr, cid);
	return storage ? ComponentStorage_Get(storage, entity) : NULL;
}

void* Scene_AddComponent(Scene_t* ptr, entity_t entity, componentid_t cid)
{
	ASSERT(ptr, "Scene_AddComponent does not permit ptr to be NULL");
	ASSERT(Scene_EntityValid(ptr, entity), "Scene_AddComponent: entity must be valid!");
	ComponentStorage_t* storage = Scene_GetStorage(ptr, cid);
	return storage ? ComponentStorage_Emplace(storage, entity) : NULL;
}

void Scene_RemoveComponent(Scene_t* ptr, entity_t entity, componentid_t cid)
{
	ASSERT(ptr, "Scene_RemoveComponent does not permit ptr to be NULL");
	ASSERT(Scene_EntityValid(ptr, entity), "Scene_RemoveComponent: entity must be valid!");
	ComponentStorage_t* storage = Scene_GetStorage(ptr, cid);
	if (storage) ComponentStorage_Remove(storage, entity);
}

View_t View_Create(Scene_t* scene, size_t numComponents, ...)
{
	//Checks and init.
	ASSERT(scene, "View_Create does not permit scene to be NULL!");
	ASSERT(numComponents < VIEW_MAX_COMPONENTS, "Cannot create view with more than %d component types. Increase VIEW_MAX_COMPONENTS.", VIEW_MAX_COMPONENTS);
	View_t ret;
	ret.components = numComponents;
	ret.currentEntity = -1;
	ret.smallestSetIndex = 0;
	ret.scene = scene;
	size_t smallestSetSize = SIZE_MAX;

	//Getting component storages.
	va_list va;
	va_start(va, numComponents);
	for (int i = 0; i < numComponents; i++)
	{
		componentid_t cid = va_arg(va, componentid_t);
		ComponentStorage_t* storage = Scene_GetStorage(scene, cid);
		ASSERT(storage, "Component type %d does not exist in the scene! View could not be created.", cid);
		ret.storages[i] = storage;

		if (smallestSetSize > storage->comp_count)
		{
			ret.smallestSetIndex = i;
			smallestSetSize = storage->comp_count;
		}
	}
	va_end(va);

	//If there are entities that fit the bill, set the current one as the first of the
	//most exclusive component set.
	if (!(smallestSetSize == 0))
	{
		ret.currentEntity = ret.storages[ret.smallestSetIndex]->sparse.dense[0];
	}
	ret.currentIndex = 0;

	return ret;
}

bool View_HasAll(View_t* ptr)
{
	ASSERT(ptr, "View_HasAll does not permit ptr to be NULL");
	ASSERT(ptr->currentEntity != -1, "View_HasAll cannot check when the current entity is invalid!");
	for (int i = 0; i < ptr->components; i++)
	{
		if (!ComponentStorage_Contains(ptr->storages[i], ptr->currentEntity)) return false;
	}
	return true;
}

bool View_End(View_t* ptr)
{
	ASSERT(ptr, "View_End does not permit ptr to be NULL");
	return ptr->currentEntity == -1;
}

void View_Next(View_t* ptr)
{
	ASSERT(ptr, "View_Next does not permit ptr to be NULL");
	if (ptr->currentEntity == -1) return;
	ComponentStorage_t* storage = ptr->storages[ptr->smallestSetIndex];
	do
	{
		if ((++ptr->currentIndex) < storage->comp_count)
			ptr->currentEntity = storage->sparse.dense[ptr->currentIndex];
		else
			ptr->currentEntity = -1;
	} while (ptr->currentEntity != -1 && !View_HasAll(ptr));
}

void View_Reset(View_t* ptr)
{
	ASSERT(ptr, "View_Reset does not permit ptr to be NULL");
	if (ptr->components != 0) 
	{
		ptr->currentEntity = ptr->storages[ptr->smallestSetIndex]->sparse.dense[0];
		ptr->currentIndex = 0;
	}
}

size_t View_GetCurrentIndex(View_t* ptr)
{
	if (ptr) return ptr->currentIndex;
	return 0;
}

void* View_GetComponent(View_t* ptr, uint32_t CompIndex)
{
	ASSERT(ptr, "View_GetComponent does not permit ptr to be NULL");
	ASSERT(!View_End(ptr), "View_GetComponent: cannot get components of invalid entity! (end reached)");
	ASSERT(CompIndex < ptr->components, "View_GetComponent: Invalid component index %d! View only has %d component types!", CompIndex, (uint32_t)ptr->components);
	return ComponentStorage_Get(ptr->storages[CompIndex], ptr->currentEntity);
}

entity_t View_GetCurrent(View_t* ptr)
{
	ASSERT(ptr, "View_GetCurrent does not permit ptr to be NULL");
	return ptr->currentEntity;
}

void View_DestroyCurrent_FindNext(View_t* ptr)
{
	ASSERT(ptr, "View_DestroyCurrent does not permit ptr to be NULL");
	if (ptr->currentEntity == -1) return;
	Scene_DeleteEntity(ptr->scene, ptr->currentEntity);
	ComponentStorage_t* storage = ptr->storages[ptr->smallestSetIndex];
	ptr->currentIndex--;
	//Find next entity.
	do
	{
		if ((++ptr->currentIndex) < storage->comp_count)
			ptr->currentEntity = storage->sparse.dense[ptr->currentIndex];
		else
			ptr->currentEntity = -1;
	} while (ptr->currentEntity != -1 && !View_HasAll(ptr));
}
