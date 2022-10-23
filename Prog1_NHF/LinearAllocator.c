#include "LinearAllocator.h"

LinearAllocator_t* LinearAllocator_Create(size_t size)
{
	LinearAllocator_t* ptr = malloc(sizeof(LinearAllocator_t) + size);
	ptr->size = size;
	ptr->offset = 0;
	ptr->next = NULL;
	return ptr;
}

void LinearAllocator_Destroy(LinearAllocator_t* alloc)
{
	if (!alloc) return;
	LinearAllocator_Destroy(alloc->next); //Free the allocator chain.
	free(alloc);
}

void* LinearAllocator_Allocate(LinearAllocator_t* alloc, size_t size)
{
	if (!alloc) return NULL;
	if (alloc->offset + size >= alloc->size)
	{
		//Allocate a next allocator if this one is out of space, and pass the allocation to that one.
		if (!alloc->next)
		{
			alloc->next = LinearAllocator_Create(alloc->size * 2);
		}
		return LinearAllocator_Allocate(alloc->next, size);
	}
	void* ptr = &alloc->data[alloc->offset];
	alloc->offset += size;
	return ptr;
}

void LinearAllocator_Reset(LinearAllocator_t * alloc)
{
	if (!alloc) return;
	LinearAllocator_Reset(alloc->next); //Reset the chain.
	alloc->offset = 0;
}
