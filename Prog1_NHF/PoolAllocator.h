#pragma once
#include "Log.h"

#include <stdlib.h>
#include <stdint.h>

typedef struct FreeListNode_t 
{
	struct FreeListNode_t* next;
} FreeListNode_t;

typedef struct PoolBlock_t 
{
	size_t size;
	struct PoolBlock_t* next;
	char data[];
} PoolBlock_t;

typedef struct PoolAllocator_t 
{
	size_t elem_size;
	FreeListNode_t* freelist;
	PoolBlock_t* blocks;
} PoolAllocator_t;

/*
* Allocate a new pool memory block and add its freelist to the given chain.
*/
PoolBlock_t* Pool_newBlock(size_t elem_size, size_t count, FreeListNode_t** freelist);
/*
* Create a pool with a given initial size.
*/
PoolAllocator_t Pool_Create(size_t elem_size, size_t initial_count);
/*
* Allocate space for one element in the pool. Allocates a new block if necessary.
*/
void* Pool_Allocate(PoolAllocator_t* allocator);
/*
* Adds the given memory to the freelist. TODO bounds checking for each block. (Owns method)
*/
void Pool_Free(PoolAllocator_t* allocator, void* ptr);
/*
* Release all resources consumed by the pool.
*/
void Pool_Reset(PoolAllocator_t* allocator);
