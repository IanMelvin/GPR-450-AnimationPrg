#include "A3_DEMO/_animation/ec_BlendTreeNode.h"

#include <assert.h>
#include <stdlib.h>

#include "A3_DEMO/_animation/ec_Interpolation.h"

#pragma region Evaluate family

//Internal helper functions
a3ret ec_blendTreeNodeEvaluate_LerpUniform (ec_BlendTreeNode* node);
a3ret ec_blendTreeNodeEvaluate_LerpPerNode (ec_BlendTreeNode* node);
a3ret ec_blendTreeNodeEvaluate_Add         (ec_BlendTreeNode* node);
a3ret ec_blendTreeNodeEvaluate_ScaleUniform(ec_BlendTreeNode* node);
a3ret ec_blendTreeNodeEvaluate_ScalePerNode(ec_BlendTreeNode* node);

a3ret ec_blendTreeNodeEvaluate(ec_BlendTreeNode* node)
{
	assert(node);

	switch (node->type)
	{
	case BT_NO_OP         : return 1; //No-op: Do nothing

	case BT_LERP_UNIFORM  : return ec_blendTreeNodeEvaluate_LerpUniform (node);
	case BT_LERP_PER_NODE : return ec_blendTreeNodeEvaluate_LerpPerNode (node);
	case BT_ADD           : return ec_blendTreeNodeEvaluate_Add         (node);
	case BT_SCALE_UNIFORM : return ec_blendTreeNodeEvaluate_ScaleUniform(node);
	case BT_SCALE_PER_NODE: return ec_blendTreeNodeEvaluate_ScalePerNode(node);

	default: assert(0); return -1; //Catch unhandled
	}
}

a3ret ec_blendTreeNodeEvaluate_LerpUniform(ec_BlendTreeNode* node)
{
	assert(node->data.lerpUniform.x0);
	assert(node->data.lerpUniform.x1);
	hierarchyPoseLerp(node->out, node->data.lerpUniform.x0, node->data.lerpUniform.x1, node->numHierarchyNodes, node->data.lerpUniform.param);
	return 1;
}

a3ret ec_blendTreeNodeEvaluate_LerpPerNode(ec_BlendTreeNode* node)
{
	assert(node->data.lerpPerNode.x0);
	assert(node->data.lerpPerNode.x1);
	assert(node->data.lerpPerNode.params);

	for (a3ui32 i = 0; i < node->numHierarchyNodes; i++)
	{
		vtable_SpatialPose.lerp(&node->out->pose[i], &node->data.lerpPerNode.x0->pose[i], &node->data.lerpPerNode.x1->pose[i], node->data.lerpPerNode.params[i], &vtable_SpatialPose);
	}

	return 1;
}

a3ret ec_blendTreeNodeEvaluate_Add(ec_BlendTreeNode* node)
{
	assert(node->data.add.a);
	assert(node->data.add.b);
	a3hierarchyPoseConcat(node->out, node->data.add.a, node->data.add.b, node->numHierarchyNodes);
	return 1;
}

a3ret ec_blendTreeNodeEvaluate_ScaleUniform(ec_BlendTreeNode* node)
{
	hierarchyPoseScaleUniform(node->out, node->data.scaleUniform.in, node->data.scaleUniform.scaleFactor, node->numHierarchyNodes);
	return 1;
}

a3ret ec_blendTreeNodeEvaluate_ScalePerNode(ec_BlendTreeNode* node)
{
	assert(node->data.scalePerNode.scaleFactors);
	hierarchyPoseScalePerNode(node->out, node->data.scalePerNode.in, node->data.scalePerNode.scaleFactors, node->numHierarchyNodes);
	return 1;
}

#pragma endregion Evaluate family

#pragma region Lifecycle family

a3ret ec_blendTreeNodeCleanup(ec_BlendTreeNode* node_out)
{
	assert(node_out->out);
	free(node_out->out->pose);
	node_out->out->pose = NULL;

	//Special case: Is there a better way that's still clean?
	if (node_out->type == BT_LERP_PER_NODE)
	{
		assert(node_out->data.lerpPerNode.params);
		free(node_out->data.lerpPerNode.params);
		node_out->data.lerpPerNode.params = NULL;
	}
	else if (node_out->type == BT_SCALE_PER_NODE)
	{
		assert(node_out->data.scalePerNode.scaleFactors);
		free(node_out->data.scalePerNode.scaleFactors);
		node_out->data.scalePerNode.scaleFactors = NULL;
	}

	return 1;
}

ec_BlendTreeNode* ec_blendTreeNodeInternalCreate(ec_BlendTreeNode* node_out, a3ui32 numNodes, ec_BlendTreeNodeType type)
{
	assert(node_out);
	assert(!node_out->out->pose);
	node_out->type = type;
	node_out->numHierarchyNodes = numNodes;
	node_out->out->pose = calloc(numNodes, sizeof(a3_SpatialPose));
	return node_out;
}

ec_BlendTreeNode* ec_blendTreeNodeCreateLerpUniform(ec_BlendTreeNode* node_out, a3ui32 numNodes, a3_HierarchyPose* x0, a3_HierarchyPose* x1, a3real param)
{
	ec_blendTreeNodeInternalCreate(node_out, numNodes, BT_LERP_UNIFORM);
	node_out->data.lerpUniform.x0 = x0;
	node_out->data.lerpUniform.x1 = x1;
	node_out->data.lerpUniform.param = param;
	return node_out;
}

ec_BlendTreeNode* ec_blendTreeNodeCreateLerpPerNode(ec_BlendTreeNode* node_out, a3ui32 numNodes, a3_HierarchyPose* x0, a3_HierarchyPose* x1, a3real defaultParam)
{
	ec_blendTreeNodeInternalCreate(node_out, numNodes, BT_LERP_PER_NODE);
	node_out->data.lerpPerNode.x0 = x0;
	node_out->data.lerpPerNode.x1 = x1;
	node_out->data.lerpPerNode.params = calloc(numNodes, sizeof(a3real));
	for (a3index i = 0; i < numNodes; ++i) node_out->data.lerpPerNode.params[i] = defaultParam;
	return node_out;
}

ec_BlendTreeNode* ec_blendTreeNodeCreateAdd(ec_BlendTreeNode* node_out, a3ui32 numNodes, a3_HierarchyPose* a, a3_HierarchyPose* b)
{
	ec_blendTreeNodeInternalCreate(node_out, numNodes, BT_ADD);
	node_out->data.add.a = a;
	node_out->data.add.b = b;
	return node_out;
}

ec_BlendTreeNode* ec_blendTreeNodeCreateScaleUniform(ec_BlendTreeNode* node_out, a3ui32 numNodes, a3_HierarchyPose* in, a3real scaleFactor)
{
	ec_blendTreeNodeInternalCreate(node_out, numNodes, BT_SCALE_UNIFORM);
	node_out->data.scaleUniform.in = in;
	node_out->data.scaleUniform.scaleFactor = scaleFactor;
	return node_out;
}

ec_BlendTreeNode* ec_blendTreeNodeCreateScalePerNode(ec_BlendTreeNode* node_out, a3ui32 numNodes, a3_HierarchyPose* in, a3real defaultScaleFactor)
{
	ec_blendTreeNodeInternalCreate(node_out, numNodes, BT_SCALE_PER_NODE);
	node_out->data.scalePerNode.in = in;
	node_out->data.scalePerNode.scaleFactors = calloc(numNodes, sizeof(a3real));
	for (a3index i = 0; i < numNodes; ++i) node_out->data.scalePerNode.scaleFactors[i] = defaultScaleFactor;
	return node_out;
}

#pragma endregion Lifecycle family