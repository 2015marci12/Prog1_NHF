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

bool SparseMap_Contains(SparseMap_t* ptr, entity_t entity)
{
	ASSERT(ptr);
	ASSERT(entity != -1);
	return (entity < ptr->sparse_size) && (ptr->sparse[entity] != -1);
}

size_t SparseMap_Index(SparseMap_t* ptr, entity_t entity)
{
	ASSERT(SparseMap_Contains(ptr, entity));
	return ptr->sparse[entity];
}

size_t SparseMap_Emplace(SparseMap_t* ptr, entity_t entity)
{
	ASSERT(ptr);
	ASSERT(entity != -1);
	if (entity > ptr->sparse_size) //Realloc sparse if needed.
	{
		size_t newSize = entity + 1;
		ptr->sparse = (entity_t*)realloc(ptr->sparse, newSize);
		memset(ptr->sparse + ptr->sparse_size, -1, newSize - ptr->sparse_size);
		ptr->sparse_size = newSize;
	}
	ptr->sparse[entity] = (entity_t)ptr->dense_size;
	if (ptr->dense_size > ptr->dense_capacity) //Realloc dense if needed.
	{
		size_t newCap = max(ptr->dense_capacity * 2, 32);
		ptr->dense = (entity_t*)realloc(ptr->dense, newCap);
		memset(ptr->dense + ptr->dense_capacity, -1, newCap - ptr->dense_capacity);
		ptr->dense_capacity = newCap;
	}
	ptr->dense[ptr->dense_size] = entity;

	return ptr->dense_size++;
}

size_t SparseMap_Remove(SparseMap_t* ptr, entity_t entity)
{
	ASSERT(ptr);
	ASSERT(SparseMap_Contains(ptr, entity));

	size_t pos = SparseMap_Index(ptr, entity);
	entity_t end = ptr->dense[ptr->dense_size];

	ptr->sparse[end] = pos;
	ptr->dense[pos] = end;
	ptr->sparse[entity] = 0;

	ptr->dense[ptr->dense_size] = 0;
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

bool ComponentStorage_Contains(ComponentStorage_t* ptr, entity_t entity)
{
	ASSERT(ptr);
	ASSERT(entity != -1);
	return SparseMap_Contains(&ptr->sparse, entity);
}

void* ComponentStorage_GetByIndex(ComponentStorage_t* ptr, size_t index)
{
	ASSERT(ptr);
	ASSERT(index < ptr->comp_count);
	return &((char*)ptr->components)[ptr->comp.size * index];
}

void* ComponentStorage_Get(ComponentStorage_t* ptr, entity_t entity)
{
	ASSERT(ptr);
	ASSERT(entity != -1);
	if (!ComponentStorage_Contains(ptr, entity)) return NULL;
	return ComponentStorage_GetByIndex(ptr, SparseMap_Index(&ptr->sparse, entity));
}

void* ComponentStorage_Emplace(ComponentStorage_t* ptr, entity_t entity)
{
	ASSERT(ptr);
	ASSERT(entity != -1);
	size_t pos = SparseMap_Emplace(&ptr->sparse, entity);
	size_t cap = ptr->sparse.dense_capacity * ptr->comp.size;
	if (cap > ptr->data_capacity) ptr->components = realloc(ptr->components, cap);
	ptr->comp_count++;
	return ComponentStorage_GetByIndex(ptr, pos);
}

void ComponentStorage_Remove(ComponentStorage_t* ptr, entity_t entity)
{
	ASSERT(ptr);
	ASSERT(entity != -1);
	size_t pos = SparseMap_Remove(&ptr->sparse, entity);
	size_t end = ptr->comp_count * ptr->comp.size;
	memmove(
		ComponentStorage_GetByIndex(ptr, pos),
		ComponentStorage_GetByIndex(ptr, end),
		ptr->comp.size
	);
	ptr->comp_count--;
}
