#pragma once
#include "Core.h"
#include "Log.h"

#include <stdlib.h>
#include <stdint.h>

/*
* A cascading linear allocator type with an initial size.
*/
typedef struct LinearAllocator_t
{
	ptrdiff_t offset;
	size_t size;
	struct LinearAllocator_t* next;
	char data[];
} LinearAllocator_t;

/*
* Allocate a new allocator block.
*/
LinearAllocator_t* LinearAllocator_Create(size_t size);
/*
* Free the given allocator and its continuation chain.
*/
void LinearAllocator_Destroy(LinearAllocator_t* alloc);
/*
* Allocate memory from the given allocator.
*/
void* LinearAllocator_Allocate(LinearAllocator_t* alloc, size_t size);
/*
* Reset the given allocator chain.
*/
void LinearAllocator_Reset(LinearAllocator_t* alloc);

