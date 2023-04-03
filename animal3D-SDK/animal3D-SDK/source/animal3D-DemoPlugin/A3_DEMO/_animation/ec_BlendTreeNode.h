/*
	Team Evistential Crisis
	Robert Christensen & Ian Melvin

	ec_BlendTreeNode.h
	Polymorphic node on a blend tree
*/

#ifndef EC_BLEND_TREE_NODE_H
#define EC_BLEND_TREE_NODE_H

#include "a3_HierarchyState.h"

typedef struct	ec_BlendTreeNode		ec_BlendTreeNode;
typedef enum	ec_BlendTreeNodeType	ec_BlendTreeNodeType;

a3ret ec_blendTreeNodeEvaluate(ec_BlendTreeNode* node);
a3ret ec_blendTreeNodeCleanup(ec_BlendTreeNode* node);
a3ret ec_blendTreeNodeCreateLerp        (ec_BlendTreeNode* node_out, a3ui32 numNodes, a3_HierarchyPose* x0, a3_HierarchyPose* x1, a3real param);
a3ret ec_blendTreeNodeCreateAdd         (ec_BlendTreeNode* node_out, a3ui32 numNodes, a3_HierarchyPose* a, a3_HierarchyPose* b);
a3ret ec_blendTreeNodeCreateScaleUniform(ec_BlendTreeNode* node_out, a3ui32 numNodes, a3_HierarchyPose* in, a3real scaleFactor);
a3ret ec_blendTreeNodeCreateScalePerNode(ec_BlendTreeNode* node_out, a3ui32 numNodes, a3_HierarchyPose* in, a3real defaultScaleFactor);

enum ec_BlendTreeNodeType
{
	BT_LERP,
	BT_ADD,
	BT_SCALE_PER_NODE,
	BT_SCALE_UNIFORM
};

struct ec_BlendTreeNode
{
	ec_BlendTreeNodeType type;

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
			a3real* scaleFactors;
		} scalePerNode;

		struct {
			a3_HierarchyPose* in;
			a3real scaleFactor;
		} scaleUniform;

	} data;
};

#endif