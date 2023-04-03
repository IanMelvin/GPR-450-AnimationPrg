/*
	Team Evistential Crisis
	Robert Christensen & Ian Melvin

	ec_BlendTreeNode.h
	Generic vtables and functions that can be used to make polymorphic interpolation
*/

#ifndef EC_BLEND_TREE_NODE_H
#define EC_BLEND_TREE_NODE_H

#include "a3_HierarchyState.h"

typedef struct ec_BlendTreeNode ec_BlendTreeNode;

a3ret ec_blendTreeNodeEvaluate(ec_BlendTreeNode* node);

struct ec_BlendTreeNode
{
	enum {
		BT_LERP,
		BT_ADD,
		BT_SCALE
	} type;

	a3ui32 numNodes;
	a3_HierarchyPose* out;

	//Data
	union {

		struct {
			a3_HierarchyPose* x0;
			a3_HierarchyPose* x1;
			a3real param;
		} lerp;

		struct {
			a3_HierarchyPose* a;
			a3_HierarchyPose* b;
		} add;

		struct {
			a3_HierarchyPose* in;
			a3real scaleFactor;
		} scale;

	} data;
};

#endif