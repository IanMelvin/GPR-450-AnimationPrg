#include "A3_DEMO/_animation/ec_BlendTree.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

a3ret ec_blendTreeCreate(ec_BlendTree* tree_out, a3ui32 numBtNodes)
{
	assert(tree_out);
	assert(!tree_out->btNodes);

	tree_out->numBtNodes = numBtNodes;
	tree_out->btNodes = calloc(numBtNodes, sizeof(ec_BlendTreeNode));
	memset(tree_out->btNodes, 0, numBtNodes*sizeof(ec_BlendTreeNode));

	return 1;
}

a3ret ec_blendTreeCleanup(ec_BlendTree* tree)
{
	assert(tree);
	assert(tree->btNodes);

	free(tree->btNodes);
	tree->btNodes = NULL;

	return 1;
}

a3ret ec_blendTreeEvaluate(ec_BlendTree* tree)
{
	assert(tree);
	for (a3index i = 0; i < tree->numBtNodes; ++i)
	{
		ec_blendTreeNodeEvaluate(&tree->btNodes[i]);
	}
	return 1;
}
