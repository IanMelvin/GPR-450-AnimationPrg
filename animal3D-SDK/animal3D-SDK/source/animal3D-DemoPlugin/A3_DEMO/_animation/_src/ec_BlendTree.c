#include "A3_DEMO/_animation/ec_BlendTree.h"

#include <assert.h>
#include <stdlib.h>

a3ret ec_blendTreeCreate(ec_BlendTree* node_out, a3ui32 numBtNodes)
{
	assert(node_out);
	assert(!node_out->btNodes);

	node_out->numBtNodes = numBtNodes;
	node_out->btNodes = calloc(numBtNodes, sizeof(ec_BlendTreeNode));

	return 1;
}

a3ret ec_blendTreeCleanup(ec_BlendTree* node)
{
	assert(node);
	assert(node->btNodes);

	free(node->btNodes);
	node->btNodes = NULL;

	return 1;
}

a3ret ec_blendTreeEvaluate(ec_BlendTree* node)
{
	assert(node);
	for (a3index i = 0; i < node->numBtNodes; ++i)
	{
		ec_blendTreeNodeEvaluate(&node->btNodes[i]);
	}
	return 1;
}
