#pragma once
#include "Core.h"
#include "PoolAllocator.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/*
* Binary search tree.
*/

typedef uint32_t Tree_key_t;

typedef struct TreeNode_t 
{
	uint32_t key;
	int height;
	struct TreeNode_t* left;
	struct TreeNode_t* right;

	void* data;
} TreeNode_t;

TreeNode_t* Tree_NewNode(Tree_key_t key, void* data);
int Tree_GetHeight(TreeNode_t* node);
int Tree_BalanceDiff(TreeNode_t* node);

/*
*	o						o
*  / \					   / \
* a   o			->		  o	  c
*	 / \				 / \
*   b   c				a	b
* 
* Returns the new top node of the subtree.
*/
TreeNode_t* Tree_RotateLeft(TreeNode_t* oldtop);

/*
*     o				  o		
*    / \			 / \		
*   o	c	->		a   o		
*  / \				   / \	
* a   b				  b   c	
*
* Returns the new top node of the subtree.
*/
TreeNode_t* Tree_RotateRight(TreeNode_t* oldtop);
/*
* Insert and return new root. If the new key already exists, no changes are made.
*/
TreeNode_t* Tree_Insert(TreeNode_t* node, Tree_key_t key, void* data);
/*
* Insert and return new root. If the new key already exists, the data is replaced.
*/
TreeNode_t* Tree_Insert_or_Replace(TreeNode_t* node, Tree_key_t key, void* data);
/*
* Get the node that contains the key. returns 0 if that does not exist.
*/
TreeNode_t* Tree_Find(TreeNode_t* node, Tree_key_t key);
/*
* Find the value associated with the key, if it exists in the tree. otherwise returns the given default value.
*/
void* Tree_Find_or_Default(TreeNode_t* node, Tree_key_t key, void* def);
/*
* Find the smallest element in the subtree.
*/
TreeNode_t* Tree_Leftmost(TreeNode_t* root);
/*
* Remove a node from the tree. return the new root node.
*/
TreeNode_t* Tree_Remove(TreeNode_t* node, Tree_key_t key);
/*
* Free all elements of a (sub)tree.
*/
void Tree_Clear(TreeNode_t* root);
/*
* Call at the end of the program. Releases all resources held by the global pool allocator.
*/
void Tree_ResetPool();

/*
* A test to check wether the cached heights remain valid after some operations, as they should.
* Used to unit test the tree logic.
*/
static bool Tree_TestCheckCachedHeights(TreeNode_t* root, int* height) 
{
	if (!root) 
	{
		*height = 0;
		return true;
	}
	
	int left, right;
	bool ret = true;
	ret &= Tree_TestCheckCachedHeights(root->left, &left);
	ret &= Tree_TestCheckCachedHeights(root->right, &right);
	*height = max(left, right) + 1;
	return ret && (*height == root->height);
}
