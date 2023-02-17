/*
	Copyright 2011-2020 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	a3_HierarchyState.c
	Implementation of transform hierarchy state.
*/

#include "../a3_HierarchyState.h"

#include <stdlib.h>
#include <string.h>


//-----------------------------------------------------------------------------

// initialize pose set given an initialized hierarchy and key pose count
a3i32 a3hierarchyPoseGroupCreate(a3_HierarchyPoseGroup *poseGroup_out, const a3_Hierarchy *hierarchy, const a3ui32 poseCount)
{
	// validate params and initialization states
	//	(output is not yet initialized, hierarchy is initialized)
	if (poseGroup_out && hierarchy && !poseGroup_out->hierarchy && hierarchy->nodes)
	{
		// determine memory requirements

		// allocate everything (one malloc)
		//??? = (...)malloc(sz);

		// set pointers
		poseGroup_out->hierarchy = hierarchy;
		poseGroup_out->poseCount = poseCount;
		poseGroup_out->spatialPoseCount = poseCount * poseGroup_out->hierarchy->numNodes;
		poseGroup_out->spatialPosePool = malloc(sizeof(a3_SpatialPose)*poseGroup_out->spatialPoseCount);

		// reset all data
		for (a3ui32 i = 0; i < poseGroup_out->spatialPoseCount; i++)
		{
			a3spatialPoseInit(&poseGroup_out->spatialPosePool[i]);
		}

		for (a3ui32 i = 0; i < poseCount; i++)
		{
			//FIXME @rsc - I don't remember what this does but the program seems to run fine without it
			//poseGroup_out->hierarchalPoses[i].spatialPose = poseGroup_out->spatialPosePool;
		}

		// done
		return 1;
	}
	return -1;
}

// release pose set
a3i32 a3hierarchyPoseGroupRelease(a3_HierarchyPoseGroup *poseGroup)
{
	// validate param exists and is initialized
	if (poseGroup && poseGroup->hierarchy)
	{
		// release everything (one free)
		//free(???);
		free(poseGroup->hierarchy->nodes);
		free(poseGroup->spatialPosePool);

		// reset pointers
		poseGroup->hierarchy = 0;

		// done
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// initialize hierarchy state given an initialized hierarchy
a3i32 a3hierarchyStateCreate(a3_HierarchyState *state_out, const a3_Hierarchy *hierarchy)
{
	// validate params and initialization states
	//	(output is not yet initialized, hierarchy is initialized)
	if (state_out && hierarchy && !state_out->hierarchy && hierarchy->nodes)
	{
		// determine memory requirements

		// allocate everything (one malloc)
		//??? = (...)malloc(sz);
		//state_out->hierarchy = (a3_Hierarchy*)malloc(sizeof(a3_Hierarchy));

		// set pointers
		state_out->hierarchy = hierarchy;

		state_out->sampledDeltaPose = NULL; //Set externally
		state_out->bindPose         = NULL; //Set externally
		state_out->samplePose       = malloc(sizeof(a3_SpatialPose       )*hierarchy->numNodes);
		state_out->localPose        = malloc(sizeof(a3_HierarchyPose     )*hierarchy->numNodes);
		state_out->objectPose       = malloc(sizeof(a3_HierarchyPose     )*hierarchy->numNodes);
		state_out->channels         = malloc(sizeof(a3_SpatialPoseChannel)*hierarchy->numNodes);

		// reset all data
		for (a3ui32 i = 0; i < hierarchy->numNodes; i++)
		{
			a3spatialPoseInit  (&state_out->samplePose[i]);
			//state_out->hierarchy->nodes[i] = ;
		}
		a3hierarchyPoseInit(state_out-> localPose, hierarchy->numNodes);
		a3hierarchyPoseInit(state_out->objectPose, hierarchy->numNodes);

		// done
		return 1;
	}
	return -1;
}

// release hierarchy state
a3i32 a3hierarchyStateRelease(a3_HierarchyState *state)
{
	// validate param exists and is initialized
	if (state && state->hierarchy)
	{
		// release everything (one free)
		//free(???);
		free(state->hierarchy->nodes);
		free(state->samplePose);
		free(state->localPose);
		free(state->objectPose);
		free(state->channels);

		// reset pointers
		state->hierarchy = 0;

		// done
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// load HTR file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadHTR(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath)
{
	if (poseGroup_out && !poseGroup_out->poseCount && hierarchy_out && !hierarchy_out->numNodes && resourceFilePath && *resourceFilePath)
	{

	}
	return -1;
}

// load BVH file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadBVH(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath)
{
	if (poseGroup_out && !poseGroup_out->poseCount && hierarchy_out && !hierarchy_out->numNodes && resourceFilePath && *resourceFilePath)
	{

	}
	return -1;
}


//-----------------------------------------------------------------------------
