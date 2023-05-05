/*
	Team Evistential Crisis
	Robert Christensen & Ian Melvin

	ec_BlendTreeNode.h
	Polymorphic node on a blend tree
*/

#ifndef EC_BLEND_TREE_NODE_H
#define EC_BLEND_TREE_NODE_H

#include "a3_HierarchyState.h"
#include "A3_DEMO/_animation/ec_Interpolation.h"

typedef struct	ec_BlendTreeNode		ec_BlendTreeNode;
typedef enum	ec_BlendTreeNodeType	ec_BlendTreeNodeType;

size_t ec_blendTreeNode_ensureHasSpace(ec_BlendTreeNode* node, ec_DataVtable* vtable);
a3ret ec_blendTreeNodeEvaluate(ec_BlendTreeNode* node, ec_DataVtable* vtable);
a3ret ec_blendTreeNodeCleanup(ec_BlendTreeNode* node);
ec_BlendTreeNode* ec_blendTreeNodeCreateDummy(ec_BlendTreeNode* node_out); //Good for getting input poses into the blend tree
ec_BlendTreeNode* ec_blendTreeNodeCreateLerpUniform(ec_BlendTreeNode* node_out, ec_BlendTreeNode* x0, ec_BlendTreeNode* x1, a3real param);
ec_BlendTreeNode* ec_blendTreeNodeCreateLerpPerNode(ec_BlendTreeNode* node_out, a3ui32 numNodes, ec_BlendTreeNode* x0, ec_BlendTreeNode* x1, a3real defaultParam);
ec_BlendTreeNode* ec_blendTreeNodeCreateAdd(ec_BlendTreeNode* node_out, ec_BlendTreeNode* a, ec_BlendTreeNode* b);
ec_BlendTreeNode* ec_blendTreeNodeCreateScaleUniform(ec_BlendTreeNode* node_out, ec_BlendTreeNode* in, a3real scaleFactor);
ec_BlendTreeNode* ec_blendTreeNodeCreateScalePerNode(ec_BlendTreeNode* node_out, a3ui32 numNodes, ec_BlendTreeNode* in, a3real defaultScaleFactor);

//Utility function for per-node operations
a3ret ec_setChain(a3real* paramsArray, a3real setValue, a3ret startIndex, const a3_Hierarchy* hierarchy);

enum ec_BlendTreeNodeType
{
	BT_NO_OP = 0,
	BT_LERP_UNIFORM,
	BT_LERP_PER_NODE,
	BT_ADD,
	BT_SCALE_UNIFORM,
	BT_SCALE_PER_NODE
};

struct ec_BlendTreeNode
{
	void* out; //Managed by self. Usually a SpatialPose array, but not always.
	size_t outAllocSize;

	ec_BlendTreeNodeType type;

	//Data
	union {

		struct {
			ec_BlendTreeNode* x0; //External
			ec_BlendTreeNode* x1; //External
			a3real param;
			a3real paramMin;
			a3real paramMax;
		} lerpUniform;

		struct {
			ec_BlendTreeNode* x0; //External
			ec_BlendTreeNode* x1; //External
			a3real* params; //Managed by self
		} lerpPerNode;

		struct {
			ec_BlendTreeNode* a; //External
			ec_BlendTreeNode* b; //External
		} add;

		struct {
			ec_BlendTreeNode* in; //External
			a3real* scaleFactors; //Managed by self
		} scalePerNode;

		struct {
			ec_BlendTreeNode* in; //External
			a3real scaleFactor;
		} scaleUniform;

	} data;
};

#endif