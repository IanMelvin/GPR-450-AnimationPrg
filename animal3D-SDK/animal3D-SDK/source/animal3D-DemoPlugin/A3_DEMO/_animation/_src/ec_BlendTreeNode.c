#include "A3_DEMO/_animation/ec_BlendTreeNode.h"

#include <assert.h>

#include "A3_DEMO/_animation/ec_Interpolation.h"

//Internal helper functions
a3ret ec_blendTreeNodeEvaluate_Lerp (ec_BlendTreeNode* node);
a3ret ec_blendTreeNodeEvaluate_Add  (ec_BlendTreeNode* node);
a3ret ec_blendTreeNodeEvaluate_Scale(ec_BlendTreeNode* node);

a3ret ec_blendTreeNodeEvaluate(ec_BlendTreeNode* node)
{
	assert(node);

	switch (node->type)
	{
	case BT_LERP : return ec_blendTreeNodeEvaluate_Lerp (node);
	case BT_ADD  : return ec_blendTreeNodeEvaluate_Add  (node);
	case BT_SCALE: return ec_blendTreeNodeEvaluate_Scale(node);

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

a3ret ec_blendTreeNodeEvaluate_Scale(ec_BlendTreeNode* node)
{
	assert(node->data.scale.scaleFactor);
	hierarchyPoseScale(node->out, node->data.scale.in, node->data.scale.scaleFactor, node->numNodes);
	return 1;
}
