/*
	Team Evistential Crisis
	Robert Christensen & Ian Melvin

	ec_BlendTreeNode.h
	Node-based blend tree
*/

#ifndef EC_BLEND_TREE_H
#define EC_BLEND_TREE_H

#include "ec_BlendTreeNode.h"

typedef struct	ec_BlendTree		ec_BlendTree;

//Effectively both a pool and a tree in postfix-like notation
struct ec_BlendTree
{
	ec_BlendTreeNode* btNodes;
	a3ui32 numBtNodes;
};

a3ret ec_blendTreeCreate(ec_BlendTree* tree_out, a3ui32 numBtNodes);
a3ret ec_blendTreeCleanup(ec_BlendTree* tree);
a3ret ec_blendTreeEvaluate(ec_BlendTree* tree, ec_DataVtable* vtable);

a3ret ec_blendTree_ensureHasSpace(ec_BlendTree* tree, ec_DataVtable* vtable);

#endif