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
	
	a3_Kinematics.c
	Implementation of kinematics solvers.
*/

#include "../a3_Kinematics.h"


//-----------------------------------------------------------------------------

a3i32 a3kinematicsPoseConcat(const a3_HierarchyState* hierarchyState)
{
	for (a3ui32 i = 0; i < hierarchyState->hierarchy->numNodes; ++i)
	{
		a3spatialPoseConcat(&hierarchyState->samplePose[i], &hierarchyState->bindPose[i], &hierarchyState->sampledDeltaPose[i]);
	}

	return 1;
}

a3i32 a3kinematicsPosesToMatrices(const a3_HierarchyState* hierarchyState)
{
	for (a3ui32 i = 0; i < hierarchyState->hierarchy->numNodes; ++i)
	{
		a3spatialPoseConvert(
			&hierarchyState->localPose[i].transform,
			&hierarchyState->samplePose[i],
			hierarchyState->samplePose[i].channels,
			hierarchyState->eulerOrder
		);
	}
	
	return 1;
}

// partial FK solver
a3i32 a3kinematicsSolveForwardPartial(const a3_HierarchyState *hierarchyState, const a3ui32 firstIndex, const a3ui32 nodeCount)
{
	if (hierarchyState && hierarchyState->hierarchy && 
		firstIndex < hierarchyState->hierarchy->numNodes && nodeCount)
	{
		//	- for all nodes starting at first index
		//		- if node is not root (has parent node)
		//			- object matrix = parent object matrix * local matrix
		//		- else
		//			- copy local matrix to object matrix

		for (a3ui32 i = firstIndex; i < firstIndex + nodeCount; ++i)
		{
			a3_HierarchyNode* node = &hierarchyState->hierarchy->nodes[i];
			
			a3mat4 ownDelta = hierarchyState->localPose[node->index].transform;

			if (node->parentIndex != -1) //If not root, concatenate
			{
				a3mat4 parentPosed = hierarchyState->objectPose[node->parentIndex].transform;
				a3real4x4Concat(parentPosed.m, ownDelta.m);
			}
			
			//Apply
			hierarchyState->objectPose[node->index].transform = ownDelta;
		}
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// partial IK solver
a3i32 a3kinematicsSolveInversePartial(const a3_HierarchyState *hierarchyState, const a3ui32 firstIndex, const a3ui32 nodeCount)
{
	if (hierarchyState && hierarchyState->hierarchy &&
		firstIndex < hierarchyState->hierarchy->numNodes && nodeCount)
	{
		// ****TO-DO: implement inverse kinematics algorithm
		//	- for all nodes starting at first index
		//		- if node is not root (has parent node)
		//			- local matrix = inverse parent object matrix * object matrix
		//		- else
		//			- copy object matrix to local matrix
	}
	return -1;
}


//-----------------------------------------------------------------------------
