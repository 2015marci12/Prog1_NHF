#include "PoolAllocator.h"

PoolBlock_t* Pool_newBlock(size_t elem_size, size_t count, FreeListNode_t** freelist)
{
	size_t totalSize = elem_size * count;
	PoolBlock_t* block = malloc(sizeof(PoolBlock_t) * totalSize);
	if (!block) return NULL; //Out of memory.
	block->size = totalSize;
	block->next = NULL;
	for(char* ptr = block->data; ptr != &block->data[totalSize]; ptr += elem_size)
	{
		FreeListNode_t* temp = ptr;
		temp->next = *freelist;
		*freelist = temp;
	}
	return block;
}

PoolAllocator_t Pool_Create(size_t elem_size, size_t initial_count)
{
	PoolAllocator_t alloc = { max(elem_size, sizeof(FreeListNode_t)), NULL, NULL };
	alloc.blocks = Pool_newBlock(alloc.elem_size, initial_count, &alloc.freelist);
	return alloc;
}

void* Pool_Allocate(PoolAllocator_t* allocator)
{
	if (!allocator) return NULL;
	if (allocator->freelist) //Freelist has elements.
	{
		void* ret = allocator->freelist;
		allocator->freelist = allocator->freelist->next;
		return ret;
	}
	//Allocate new block.
	size_t size = 1024;
	if (allocator->blocks) size = allocator->blocks->size * 2;
	PoolBlock_t* newblock = Pool_newBlock(allocator->elem_size, size, &allocator->freelist);
	if (!newblock)
	{
		ASSERT(false, "Pool allocator: out of memory!");
		return NULL;
	}
	newblock->next = allocator->blocks;
	allocator->blocks = newblock;
	return Pool_Allocate(allocator);
}

/*
* Adds the given memory to the freelist.
*/
void Pool_Free(PoolAllocator_t* allocator, void* ptr)
{
	ASSERT(allocator, "Invalid!");
	FreeListNode_t* temp = ptr;
	temp->next = allocator->freelist;
	allocator->freelist = temp;
}

void Pool_Reset(PoolAllocator_t* allocator)
{
	if (!allocator) return;

	PoolBlock_t* current = allocator->blocks;
	while (current != NULL)
	{
		PoolBlock_t* temp = current->next;
		free(current);
		current = temp;
	}

	allocator->blocks = NULL;
	allocator->freelist = NULL;
}
