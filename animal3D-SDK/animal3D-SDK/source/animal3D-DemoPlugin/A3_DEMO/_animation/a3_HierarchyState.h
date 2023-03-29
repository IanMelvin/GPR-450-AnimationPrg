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
	
	a3_HierarchyState.h
	Hierarchy transformation state.
*/

#ifndef __ANIMAL3D_HIERARCHYSTATE_H
#define __ANIMAL3D_HIERARCHYSTATE_H


// A3 hierarchy
#include "a3_Hierarchy.h"

// A3 spatial pose
#include "a3_SpatialPose.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_HierarchyPose			a3_HierarchyPose;
typedef struct a3_HierarchyPoseGroup	a3_HierarchyPoseGroup;
typedef struct a3_HierarchyState		a3_HierarchyState;
#endif	// __cplusplus
	
#define fileLineMaxLength ((size_t)256)
//-----------------------------------------------------------------------------

// single pose for a collection of nodes
// makes algorithms easier to keep this as a separate data type
struct a3_HierarchyPose
{
	// Array of SpatialPoses
	a3_SpatialPose* pose;

	// Number of SpatialPoses in array
	a3ui32 poseCount;
};


// pose group
struct a3_HierarchyPoseGroup
{
	// pointer to hierarchy
	const a3_Hierarchy* hierarchy;

	// number of hierarchical poses
	a3ui32 poseCount;

	// number of spatial poses hierarchicalPoseCount x hierarchy node count
	a3ui32 spatialPoseCount;

	// Array of SpatialPoses
	a3_SpatialPose* spatialPosePool;

	// Array of Hierarchal Poses
	a3_HierarchyPose* hierarchalPoses;
};


// hierarchy state structure
struct a3_HierarchyState
{
	// pointer to hierarcy
	const a3_Hierarchy* hierarchy;

	//Inputs to step 2
	//Arrays
	a3_SpatialPose* bindPose;
	a3_SpatialPose* sampledDeltaPose; //Output of step 1

	//An array of spatial poses representing each node's animated pose at the current time.
	//Result of step 2
	a3_SpatialPose* samplePose;

	//An array of hierarchical poses representing each node's transformation relative to its parent's space.
	//Result of step 3
	a3mat4* localPose;

	//A hierarchical pose representing each node's transformation relative to the root's parent space (the actual object that the hierarchy represents).
	//Result of step 4
	a3mat4* objectPose;
	a3mat4* objectPoseInv;
	a3mat4* objectPoseBindToCurrent;

	//Array of constraints
	a3_SpatialPoseChannel* channels;

	// Some global flag for the pool that describes the concatenation order of orientation channels
	a3_SpatialPoseEulerOrder eulerOrder;
};


//-----------------------------------------------------------------------------

// initialize pose set given an initialized hierarchy and key pose count
a3i32 a3hierarchyPoseGroupCreate(a3_HierarchyPoseGroup *poseGroup_out, const a3_Hierarchy *hierarchy, const a3ui32 poseCount);

// release pose set
a3i32 a3hierarchyPoseGroupRelease(a3_HierarchyPoseGroup *poseGroup);

// get offset to hierarchy pose in contiguous set
a3i32 a3hierarchyPoseGroupGetPoseOffsetIndex(const a3_HierarchyPoseGroup *poseGroup, const a3ui32 poseIndex);

// get offset to single node pose in contiguous set
a3i32 a3hierarchyPoseGroupGetNodePoseOffsetIndex(const a3_HierarchyPoseGroup *poseGroup, const a3ui32 poseIndex, const a3ui32 nodeIndex);


//-----------------------------------------------------------------------------

// reset full hierarchy pose
a3i32 a3hierarchyPoseReset(a3_HierarchyPose* pose_inout, const a3ui32 nodeCount);

// convert full hierarchy pose to hierarchy transforms
a3i32 a3hierarchyPoseConvert(a3mat4* transforms_out, const a3_SpatialPose* poses_in, const a3ui32 nodeCount, const a3_SpatialPoseChannel* channel, const a3_SpatialPoseEulerOrder order);

// copy full hierarchy pose
a3i32 a3hierarchyPoseCopy(a3_HierarchyPose* pose_out, const a3_HierarchyPose* pose_in, const a3ui32 nodeCount);

// init a3hierarchyPose
a3i32 a3hierarchyPoseInit(a3_HierarchyPose* pose_out, a3ui32 poseCount);


//-----------------------------------------------------------------------------

// initialize hierarchy state given an initialized hierarchy
a3i32 a3hierarchyStateCreate(a3_HierarchyState *state_out, const a3_Hierarchy *hierarchy);

// release hierarchy state
a3i32 a3hierarchyStateRelease(a3_HierarchyState *state);

// update inverse object-space matrices
a3i32 a3hierarchyStateUpdateObjectInverse(const a3_HierarchyState *state);


//-----------------------------------------------------------------------------

// load HTR file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadHTR(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath);

// load BVH file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadBVH(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath);


//-----------------------------------------------------------------------------

//
// File Input support for HTR file reading
//

// Compare Header to known headers
a3i32 ec_checkHeader(const a3_FileStream* inStream, a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out);

// check next line for comments or headers
a3i32 ec_specialCaseChecker(a3_FileStream const* inStream);

//Skip to nearest \n
a3i32 ec_skipLine(a3_FileStream const* inStream);

// check next line for comments or headers, unlike ec_specialCaseChecker if a normal line is found it ungets the checked value
a3i32 ec_peakNextInput(a3_FileStream const* inStream);

//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_HierarchyState.inl"


#endif	// !__ANIMAL3D_HIERARCHYSTATE_H