#include "A3_DEMO/_animation/ec_BlendTreeNode.h"

#include <assert.h>
#include <stdlib.h>

#pragma region Evaluate family

//Internal helper functions
a3ret ec_blendTreeNodeEvaluate_LerpUniform (ec_BlendTreeNode* node, ec_DataVtable* vtable);
a3ret ec_blendTreeNodeEvaluate_LerpPerNode (ec_BlendTreeNode* node, ec_DataVtable* vtable);
a3ret ec_blendTreeNodeEvaluate_Add         (ec_BlendTreeNode* node, ec_DataVtable* vtable);
a3ret ec_blendTreeNodeEvaluate_ScaleUniform(ec_BlendTreeNode* node, ec_DataVtable* vtable);
a3ret ec_blendTreeNodeEvaluate_ScalePerNode(ec_BlendTreeNode* node, ec_DataVtable* vtable);

size_t ec_blendTreeNode_ensureHasSpace(ec_BlendTreeNode* node, ec_DataVtable* vtable)
{
	//*
	//Note that this will also work on dummy no-op nodes, which is intentional!
	size_t minSize = vtable->unitSize * vtable->arrayCount;
	if (node->outAllocSize < minSize)
	{
		if (node->out) vtable->release(node->out); //No need to copy old data as it will likely be overwritten soon
		node->out = vtable->alloc(vtable);
		node->outAllocSize = minSize;
	}
	
	return node->outAllocSize;
}

a3ret ec_blendTreeNodeEvaluate(ec_BlendTreeNode* node, ec_DataVtable* vtable)
{
	assert(node);

	//Ensure we have enough space for output
	ec_blendTreeNode_ensureHasSpace(node, vtable);
	
	switch (node->type)
	{
	case BT_NO_OP         : return 1; //No-op: Do nothing

	case BT_LERP_UNIFORM  : return ec_blendTreeNodeEvaluate_LerpUniform (node, vtable);
	case BT_LERP_PER_NODE : return ec_blendTreeNodeEvaluate_LerpPerNode (node, vtable);
	case BT_ADD           : return ec_blendTreeNodeEvaluate_Add         (node, vtable);
	case BT_SCALE_UNIFORM : return ec_blendTreeNodeEvaluate_ScaleUniform(node, vtable);
	case BT_SCALE_PER_NODE: return ec_blendTreeNodeEvaluate_ScalePerNode(node, vtable);

	default: assert(0); return -1; //Catch unhandled
	}
}

a3ret ec_blendTreeNodeEvaluate_LerpUniform(ec_BlendTreeNode* node, ec_DataVtable* vtable)
{
	assert(node->data.lerpUniform.x0);
	assert(node->data.lerpUniform.x1);
	a3real t = a3lerpInverse(node->data.lerpUniform.paramMin, node->data.lerpUniform.paramMax, node->data.lerpUniform.param);
	t = a3clamp(0, 1, t);
	vtable->lerp(node->out, node->data.lerpUniform.x0, node->data.lerpUniform.x1, t, vtable);
	return 1;
}

a3ret ec_blendTreeNodeEvaluate_LerpPerNode(ec_BlendTreeNode* node, ec_DataVtable* vtable_in)
{
	assert(node->data.lerpPerNode.x0);
	assert(node->data.lerpPerNode.x1);
	assert(node->data.lerpPerNode.params);

	//We need to walk one-by-one
	ec_DataVtable vtable = *vtable_in;
	vtable.arrayCount = 1;

	for (a3ui32 i = 0; i < vtable_in->arrayCount; i++)
	{
		vtable.lerp(
			((char*)node->out)+vtable.unitSize*i,
			((char*)node->data.lerpPerNode.x0)+vtable.unitSize*i,
			((char*)node->data.lerpPerNode.x1)+vtable.unitSize*i,
			node->data.lerpPerNode.params[i],
			&vtable
		);
	}

	return 1;
}

a3ret ec_blendTreeNodeEvaluate_Add(ec_BlendTreeNode* node, ec_DataVtable* vtable)
{
	assert(node->data.add.a);
	assert(node->data.add.b);
	vtable->arrayConcat(node->out, node->data.add.a, node->data.add.b, vtable);
	return 1;
}

a3ret ec_blendTreeNodeEvaluate_ScaleUniform(ec_BlendTreeNode* node, ec_DataVtable* vtable)
{
	vtable->copy(node->out, node->data.scaleUniform.in, vtable);
	vtable->arrayScale(node->out, node->data.scaleUniform.scaleFactor, vtable);
	return 1;
}

a3ret ec_blendTreeNodeEvaluate_ScalePerNode(ec_BlendTreeNode* node, ec_DataVtable* vtable_in)
{
	assert(node->data.scalePerNode.scaleFactors);

	//We need to walk one-by-one
	ec_DataVtable vtable = *vtable_in;
	vtable.arrayCount = 1;

	for (a3ui32 i = 0; i < vtable_in->arrayCount; i++)
	{
		vtable.copy(node->out, node->data.scalePerNode.in, &vtable);
		vtable.unitScale(node->out, node->data.scalePerNode.scaleFactors[i]);
	}

	return 1;
}

#pragma endregion Evaluate family

#pragma region Lifecycle family

a3ret ec_blendTreeNodeCleanup(ec_BlendTreeNode* node_out)
{
	if (node_out->out)
	{
		free(node_out->out);
		node_out->out = NULL;
		node_out->outAllocSize = 0;
	}

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

ec_BlendTreeNode* ec_blendTreeNodeInternalCreate(ec_BlendTreeNode* node_out, ec_BlendTreeNodeType type)
{
	assert(node_out);
	assert(!node_out->out);
	node_out->type = type;
	node_out->out = NULL;
	node_out->outAllocSize = 0;
	return node_out;
}

ec_BlendTreeNode* ec_blendTreeNodeCreateDummy(ec_BlendTreeNode* node_out)
{
	return ec_blendTreeNodeInternalCreate(node_out, BT_NO_OP);
}

ec_BlendTreeNode* ec_blendTreeNodeCreateLerpUniform(ec_BlendTreeNode* node_out, ec_BlendTreeNode* x0, ec_BlendTreeNode* x1, a3real param)
{
	ec_blendTreeNodeInternalCreate(node_out, BT_LERP_UNIFORM);
	node_out->data.lerpUniform.x0 = x0;
	node_out->data.lerpUniform.x1 = x1;
	node_out->data.lerpUniform.param = param;
	node_out->data.lerpUniform.paramMin = 0;
	node_out->data.lerpUniform.paramMax = 1;
	return node_out;
}

ec_BlendTreeNode* ec_blendTreeNodeCreateLerpPerNode(ec_BlendTreeNode* node_out, a3ui32 numNodes, ec_BlendTreeNode* x0, ec_BlendTreeNode* x1, a3real defaultParam)
{
	ec_blendTreeNodeInternalCreate(node_out, BT_LERP_PER_NODE);
	node_out->data.lerpPerNode.x0 = x0;
	node_out->data.lerpPerNode.x1 = x1;
	node_out->data.lerpPerNode.params = calloc(numNodes, sizeof(a3real));
	for (a3index i = 0; i < numNodes; ++i) node_out->data.lerpPerNode.params[i] = defaultParam;
	return node_out;
}

ec_BlendTreeNode* ec_blendTreeNodeCreateAdd(ec_BlendTreeNode* node_out, ec_BlendTreeNode* a, ec_BlendTreeNode* b)
{
	ec_blendTreeNodeInternalCreate(node_out, BT_ADD);
	node_out->data.add.a = a;
	node_out->data.add.b = b;
	return node_out;
}

ec_BlendTreeNode* ec_blendTreeNodeCreateScaleUniform(ec_BlendTreeNode* node_out, ec_BlendTreeNode* in, a3real scaleFactor)
{
	ec_blendTreeNodeInternalCreate(node_out, BT_SCALE_UNIFORM);
	node_out->data.scaleUniform.in = in;
	node_out->data.scaleUniform.scaleFactor = scaleFactor;
	return node_out;
}

ec_BlendTreeNode* ec_blendTreeNodeCreateScalePerNode(ec_BlendTreeNode* node_out, a3ui32 numNodes, ec_BlendTreeNode* in, a3real defaultScaleFactor)
{
	ec_blendTreeNodeInternalCreate(node_out, BT_SCALE_PER_NODE);
	node_out->data.scalePerNode.in = in;
	node_out->data.scalePerNode.scaleFactors = calloc(numNodes, sizeof(a3real));
	for (a3index i = 0; i < numNodes; ++i) node_out->data.scalePerNode.scaleFactors[i] = defaultScaleFactor;
	return node_out;
}

#pragma endregion Lifecycle family