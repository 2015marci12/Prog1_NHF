#include "Container.h"

PoolAllocator_t Tree_global_pool = { sizeof(TreeNode_t), NULL, NULL };

TreeNode_t* Tree_NewNode(Tree_key_t key, void* data)
{
	TreeNode_t* ret = (TreeNode_t*)Pool_Allocate(&Tree_global_pool);
	ret->key = key;
	ret->data = data;
	ret->left = 0;
	ret->right = 0;
	ret->height = 1;

	return ret;
}

int Tree_GetHeight(TreeNode_t* node)
{
	if (node) return node->height;
	return 0;
}

int Tree_BalanceDiff(TreeNode_t* node)
{
	if (!node) return 0;
	return Tree_GetHeight(node->left) - Tree_GetHeight(node->right);
}

/*
*	o						o
*  / \					   / \
* a   o			->		  o	  c
*	 / \				 / \
*   b   c				a	b
*
* Returns the new top node of the subtree.
*/
TreeNode_t* Tree_RotateLeft(TreeNode_t* oldtop)
{
	TreeNode_t* newtop = oldtop->right;
	TreeNode_t* a = oldtop->left;
	TreeNode_t* b = newtop->left;
	TreeNode_t* c = newtop->right;

	oldtop->left = a;
	oldtop->right = b;
	newtop->left = oldtop;
	newtop->right = c;

	oldtop->height = max(Tree_GetHeight(a), Tree_GetHeight(b)) + 1;
	newtop->height = max(Tree_GetHeight(c), oldtop->height) + 1;

	return newtop;
}

/*
*     o				  o
*    / \			 / \
*   o	c	->		a   o
*  / \				   / \
* a   b				  b   c
*
* Returns the new top node of the subtree.
*/
TreeNode_t* Tree_RotateRight(TreeNode_t* oldtop)
{
	TreeNode_t* newtop = oldtop->left;
	TreeNode_t* a = newtop->left;
	TreeNode_t* b = newtop->left;
	TreeNode_t* c = oldtop->right;

	newtop->left = a;
	newtop->right = oldtop;
	oldtop->left = b;
	oldtop->right = c;

	oldtop->height = max(Tree_GetHeight(b), Tree_GetHeight(c)) + 1;
	newtop->height = max(Tree_GetHeight(a), oldtop->height) + 1;

	return newtop;
}

/*
* Insert and return new root.
*/
TreeNode_t* Tree_Insert(TreeNode_t* node, Tree_key_t key, void* data)
{
	if (!node) return Tree_NewNode(key, data); //Handle empty tree.

	//BST insert.
	if (key < node->key)
	{
		node->left = Tree_Insert(node->left, key, data);
		node->height = max(Tree_GetHeight(node->left), Tree_GetHeight(node->right)) + 1;
	}
	else if (key > node->key)
	{
		node->right = Tree_Insert(node->right, key, data);
		node->height = max(Tree_GetHeight(node->left), Tree_GetHeight(node->right)) + 1;
	}
	else //key found, do nothing.
	{
		return node;
	}

	//Balance.
	int balance = Tree_BalanceDiff(node);

	if (balance > 1 && key < node->left->key)
		return Tree_RotateRight(node);

	if (balance < -1 && key > node->right->key)
		return Tree_RotateLeft(node);

	if (balance > 1 && key > node->left->key)
	{
		node->left = Tree_RotateLeft(node->left);
		return Tree_RotateRight(node);
	}

	if (balance < -1 && key < node->right->key)
	{
		node->right = Tree_RotateRight(node->right);
		return Tree_RotateLeft(node);
	}

	return node;
}

TreeNode_t* Tree_Insert_or_Replace(TreeNode_t* node, Tree_key_t key, void* data)
{
	if (!node) return Tree_NewNode(key, data); //Handle empty tree.

	//BST insert.
	if (key < node->key)
	{
		node->left = Tree_Insert(node->left, key, data);
		node->height = max(Tree_GetHeight(node->left), Tree_GetHeight(node->right)) + 1;
	}
	else if (key > node->key)
	{
		node->right = Tree_Insert(node->right, key, data);
		node->height = max(Tree_GetHeight(node->left), Tree_GetHeight(node->right)) + 1;
	}
	else //key found, replace data.
	{
		node->data = data;
		return node;
	}

	//Balance.
	int balance = Tree_BalanceDiff(node);

	if (balance > 1 && key < node->left->key)
		return Tree_RotateRight(node);

	if (balance < -1 && key > node->right->key)
		return Tree_RotateLeft(node);

	if (balance > 1 && key > node->left->key)
	{
		node->left = Tree_RotateLeft(node->left);
		return Tree_RotateRight(node);
	}

	if (balance < -1 && key < node->right->key)
	{
		node->right = Tree_RotateRight(node->right);
		return Tree_RotateLeft(node);
	}

	return node;
}

/*
* Get the node that contains the key. returns 0 if that does not exist.
*/
TreeNode_t* Tree_Find(TreeNode_t* node, Tree_key_t key)
{
	if (!node) return NULL;
	else if (key == node->key) return node;
	else if (key < node->key) return Tree_Find(node->left, key);
	else return Tree_Find(node->right, key);
}

void* Tree_Find_or_Default(TreeNode_t* node, Tree_key_t key, void* def)
{
	TreeNode_t* findres = Tree_Find(node, key);
	return findres ? findres->data : def;
}

/*
* Find the smallest element in the subtree.
*/
TreeNode_t* Tree_Leftmost(TreeNode_t* root)
{
	TreeNode_t* current = root;
	while (current->left)
		current = current->left;
	return current;
}

/*
* Remove a node from the tree. return the new root node.
*/
TreeNode_t* Tree_Remove(TreeNode_t* node, Tree_key_t key)
{
	//BST delete.
	if (!node) return NULL; //Empty subtree.
	else if (node->key > key)
	{
		node->left = Tree_Remove(node->left, key); //Left.
		node->height = max(Tree_GetHeight(node->left), Tree_GetHeight(node->right)) + 1;
	}
	else if (node->key < key)
	{
		node->right = Tree_Remove(node->right, key); //Right.
		node->height = max(Tree_GetHeight(node->left), Tree_GetHeight(node->right)) + 1;
	}
	else if (!node->left && !node->right) //Found node, no children.
	{
		Pool_Free(&Tree_global_pool, node);
		//free(node);
		return NULL; //Subtree empty, no need to balance.
	}
	else if (!(node->left && node->right)) //Found node, 1 child.
	{
		TreeNode_t* child = node->left ? node->left : node->right;
		Pool_Free(&Tree_global_pool, node);
		//free(node);
		return child; //Subtree has 1 element, balanced by definition.
	}
	else //Found node, 2 children.
	{
		TreeNode_t* successor = Tree_Leftmost(node->right);

		//Copy successor node params.
		node->key = successor->key;
		node->data = successor->data;

		//Remove successor from right tree.
		node->right = Tree_Remove(node->right, successor->key);

		node->height = max(Tree_GetHeight(node->left), Tree_GetHeight(node->right)) + 1;
	}

	//Balance using the cached heights.
	int balance = Tree_BalanceDiff(node);
	int leftbalance = Tree_BalanceDiff(node->left);
	int rightbalance = Tree_BalanceDiff(node->right);

	if (balance > 1 && leftbalance >= 0) return Tree_RotateRight(node);
	if (balance > 1 && leftbalance < 0)
	{
		node->left = Tree_RotateLeft(node->left);
		return Tree_RotateRight(node);
	}
	if (balance < -1 && rightbalance <= 0) return Tree_RotateLeft(node);
	if (balance < -1 && rightbalance > 0)
	{
		node->right = Tree_RotateRight(node->right);
		return Tree_RotateLeft(node);
	}

	return node;
}

void Tree_Clear(TreeNode_t* root)
{
	if (!root) return;
	Tree_Clear(root->left);
	Tree_Clear(root->right);
	Pool_Free(&Tree_global_pool, root);
	//free(node);
}

void Tree_ResetPool()
{
	Pool_Reset(&Tree_global_pool);
}
