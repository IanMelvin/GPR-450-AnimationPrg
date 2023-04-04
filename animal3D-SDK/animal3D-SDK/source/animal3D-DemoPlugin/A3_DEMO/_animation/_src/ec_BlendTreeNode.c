#include "A3_DEMO/_animation/ec_BlendTreeNode.h"

#include <assert.h>
#include <stdlib.h>

#include "A3_DEMO/_animation/ec_Interpolation.h"

#pragma region Evaluate family

//Internal helper functions
a3ret ec_blendTreeNodeEvaluate_Lerp        (ec_BlendTreeNode* node);
a3ret ec_blendTreeNodeEvaluate_Add         (ec_BlendTreeNode* node);
a3ret ec_blendTreeNodeEvaluate_ScaleUniform(ec_BlendTreeNode* node);
a3ret ec_blendTreeNodeEvaluate_ScalePerNode(ec_BlendTreeNode* node);

a3ret ec_blendTreeNodeEvaluate(ec_BlendTreeNode* node)
{
	assert(node);

	switch (node->type)
	{
	case BT_LERP          : return ec_blendTreeNodeEvaluate_Lerp        (node);
	case BT_ADD           : return ec_blendTreeNodeEvaluate_Add         (node);
	case BT_SCALE_UNIFORM : return ec_blendTreeNodeEvaluate_ScaleUniform(node);
	case BT_SCALE_PER_NODE: return ec_blendTreeNodeEvaluate_ScalePerNode(node);

	default: assert(0); return -1; //Catch unhandled
	}
}

a3ret ec_blendTreeNodeEvaluate_Lerp(ec_BlendTreeNode* node)
{
	assert(node->data.lerp.x0);
	assert(node->data.lerp.x1);
	hierarchyPoseLerp(node->out, node->data.lerp.x0, node->data.lerp.x1, node->numNodes, node->data.lerp.param);
	return 1;
}

a3ret ec_blendTreeNodeEvaluate_Add(ec_BlendTreeNode* node)
{
	assert(node->data.add.a);
	assert(node->data.add.b);
	a3hierarchyPoseConcat(node->out, node->data.add.a, node->data.add.b, node->numNodes);
	return 1;
}

a3ret ec_blendTreeNodeEvaluate_ScaleUniform(ec_BlendTreeNode* node)
{
	hierarchyPoseScaleUniform(node->out, node->data.scaleUniform.in, node->data.scaleUniform.scaleFactor, node->numNodes);
	return 1;
}

a3ret ec_blendTreeNodeEvaluate_ScalePerNode(ec_BlendTreeNode* node)
{
	assert(node->data.scalePerNode.scaleFactors);
	hierarchyPoseScalePerNode(node->out, node->data.scalePerNode.in, node->data.scalePerNode.scaleFactors, node->numNodes);
	return 1;
}

#pragma endregion Evaluate family

#pragma region Lifecycle family

a3ret ec_blendTreeNodeCleanup(ec_BlendTreeNode* node_out)
{
	assert(node_out->out);
	free(node_out->out);
	node_out->out = NULL;

	//Special case: Is there a better way that's still clean?
	if (node_out->type == BT_SCALE_PER_NODE)
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
	assert(!node_out->out);
	node_out->type = type;
	node_out->numNodes = numNodes;
	node_out->out = calloc(numNodes, sizeof(a3_HierarchyPose));
	return node_out;
}

ec_BlendTreeNode* ec_blendTreeNodeCreateLerp(ec_BlendTreeNode* node_out, a3ui32 numNodes, a3_HierarchyPose* x0, a3_HierarchyPose* x1, a3real param)
{
	ec_blendTreeNodeInternalCreate(node_out, numNodes, BT_LERP);
	node_out->data.lerp.x0 = x0;
	node_out->data.lerp.x1 = x1;
	node_out->data.lerp.param = param;
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