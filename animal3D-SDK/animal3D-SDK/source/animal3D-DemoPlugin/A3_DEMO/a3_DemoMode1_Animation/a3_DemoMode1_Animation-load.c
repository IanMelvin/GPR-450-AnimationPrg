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

	a3_DemoMode1_Animation-load.c
	Demo mode implementations: animation scene.

	********************************************
	*** LOADING FOR ANIMATION SCENE MODE     ***
	********************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoMode1_Animation.h"

#include "../a3_DemoState.h"


//-----------------------------------------------------------------------------

a3i32 createPoseClip(a3_Clip* clip, char* name, const a3_HierarchyState* hierarchyState, const a3_KeyframePool* keyframes, const a3ui32 firstKeyframe, const a3ui32 lastKeyframe, const a3ui32 animFrameCount)
{
	a3clipInit(clip, name, hierarchyState->hierarchy->numNodes);
	for (a3ui32 i = 0; i < hierarchyState->hierarchy->numNodes; ++i)
	{
		a3_HierarchyNode* node = &hierarchyState->hierarchy->nodes[i];
		a3ui32 startOffset = hierarchyState->hierarchy->numNodes + i * animFrameCount;
		a3keyframeChannelInit(&clip->channels[i], node->name, keyframes, startOffset + firstKeyframe, startOffset + lastKeyframe);
	}
	a3clipCalculateDuration(clip);
	clip->forwardTransition.flags = EC_TERMINUSACTION_PAUSE; clip->forwardTransition.targetClipID = NULL_CLIP_ID;
	clip->reverseTransition.flags = EC_TERMINUSACTION_PAUSE; clip->reverseTransition.targetClipID = NULL_CLIP_ID;
	return 1;
}

// utility to load animation
void a3animation_init_animation(a3_DemoState const* demoState, a3_DemoMode1_Animation* demoMode)
{
	// general counters
	//a3ui32 j, p;

	// object pointers
	a3_Hierarchy* hierarchy = 0;
	a3_HierarchyState* hierarchyState = 0;
	a3_HierarchyPoseGroup* hierarchyPoseGroup = 0;
	a3_SpatialPose* spatialPose = 0;

	a3_FileStream fileStream[1] = { 0 };
	const a3byte* const geometryStream = "./data/anim_data_gpro_base.dat";

	// stream animation assets
	if (demoState->streaming && a3fileStreamOpenRead(fileStream, geometryStream))
	{
		// load hierarchy assets
		hierarchy = demoMode->hierarchy_skel;
		a3hierarchyLoadBinary(hierarchy, fileStream);

		// done
		a3fileStreamClose(fileStream);
	}
	// not streaming or stream doesn't exist
	else if (!demoState->streaming || a3fileStreamOpenWrite(fileStream, geometryStream))
	{
	/*	// manually set up a skeleton
		// first is the hierarchy: the general non-spatial relationship between bones
		const a3ui32 jointCount = 32;

		// indices of joints, their parents and branching joints
		a3ui32 jointIndex = 0;
		a3i32 jointParentIndex = -1;
		a3i32 rootJointIndex, upperSpineJointIndex, clavicleJointIndex, pelvisJointIndex;

		// initialize hierarchy
		hierarchy = demoMode->hierarchy_skel;
		a3hierarchyCreate(hierarchy, jointCount, 0);

		// set up joint relationships
		jointParentIndex = rootJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:root");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:spine_lower");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:spine_mid");
		jointParentIndex = upperSpineJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:spine_upper");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:neck");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:head");
		jointParentIndex = upperSpineJointIndex;
		jointParentIndex = clavicleJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:clavicle");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shoulderblade_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shoulder_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:elbow_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:forearm_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:wrist_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:hand_r");
		jointParentIndex = clavicleJointIndex;
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shoulderblade_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shoulder_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:elbow_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:forearm_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:wrist_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:hand_l");
		jointParentIndex = rootJointIndex;
		jointParentIndex = pelvisJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:pelvis");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:hip_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:knee_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shin_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:ankle_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:foot_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:toe_r");
		jointParentIndex = pelvisJointIndex;
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:hip_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:knee_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shin_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:ankle_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:foot_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:toe_l");

		// save hierarchy assets
		a3hierarchySaveBinary(hierarchy, fileStream);
	*/
		// done
		a3fileStreamClose(fileStream);
	}


	// scene objects
	demoMode->obj_skeleton->position.y = +a3real_four;
	demoMode->obj_skeleton->euler.z = +a3real_oneeighty;
	demoMode->obj_skeleton->euler.x = -a3real_ninety;

	// next set up hierarchy poses
	hierarchy = demoMode->hierarchy_skel;
	hierarchyPoseGroup = demoMode->hierarchyPoseGroup_skel;
	hierarchyPoseGroup->hierarchy = 0;

/*
	// allocate poses
	a3hierarchyPoseGroupCreate(hierarchyPoseGroup, hierarchy, 5, a3poseEulerOrder_xyz);

	// define "bind pose" or "base pose" or the initial transformation 
	//	description for each joint (not a literal transform)
	p = 0;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetTranslation(spatialPose, 0.0f, 0.0f, +3.6f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz | a3poseChannel_scale_xyz | a3poseChannel_translate_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_lower");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, -90.0f, -5.0f);
	a3spatialPoseSetTranslation(spatialPose, 0.0f, -0.1f, +0.1f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_mid");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +10.0f);
	a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_upper");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -5.0f);
	a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:neck");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:head");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:clavicle");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, +90.0f, 0.0f);
	a3spatialPoseSetTranslation(spatialPose, -0.1f, +0.1f, 0.0f);

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulderblade_r");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetTranslation(spatialPose, +0.5f, -0.1f, -0.5f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_translate_yz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulder_r");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, +30.0f, -10.0f);
	a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, +0.5f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:elbow_r");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +20.0f);
	a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_z;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:forearm_r");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_x;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:wrist_r");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -10.0f);
	a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:hand_r");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulderblade_l");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetTranslation(spatialPose, -0.5f, -0.1f, -0.5f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_translate_yz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulder_l");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, -30.0f, +10.0f);
	a3spatialPoseSetTranslation(spatialPose, -0.5f, 0.0f, +0.5f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:elbow_l");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -20.0f);
	a3spatialPoseSetTranslation(spatialPose, -2.0f, 0.0f, 0.0f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_z;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:forearm_l");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_x;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:wrist_l");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +10.0f);
	a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:hand_l");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetTranslation(spatialPose, -0.5f, 0.0f, 0.0f);

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:pelvis");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetTranslation(spatialPose, 0.0f, 0.0f, -0.1f);

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:hip_r");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, +90.0f, +10.0f);
	a3spatialPoseSetTranslation(spatialPose, +1.0f, -0.1f, +0.5f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:knee_r");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -20.0f);
	a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_z;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shin_r");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_x;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:ankle_r");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +90.0f);
	a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:foot_r");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +10.0f);
	a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_z;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:toe_r");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetTranslation(spatialPose, +0.2f, 0.0f, 0.0f);

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:hip_l");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, -90.0f, -10.0f);
	a3spatialPoseSetTranslation(spatialPose, -1.0f, -0.1f, +0.5f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:knee_l");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +20.0f);
	a3spatialPoseSetTranslation(spatialPose, -2.0f, 0.0f, 0.0f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_z;
	
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shin_l");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_x;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:ankle_l");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -90.0f);
	a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:foot_l");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -10.0f);
	a3spatialPoseSetTranslation(spatialPose, -0.5f, 0.0f, 0.0f);
	hierarchyPoseGroup->channel[j] = a3poseChannel_orient_z;
	
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:toe_l");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetTranslation(spatialPose, -0.2f, 0.0f, 0.0f);


	// each remaining pose represents a "delta" from the base
	// initialize the changes where applicable
	// (note: the channels describe which pose components can change)
	p = 2;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetRotation(spatialPose, +45.0f, +60.0f, +90.0f);	// rotate whole figure about all axes


	p = 3;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetScale(spatialPose, 2.0f, 2.0f, 2.0f);	// uniformly scale whole figure up to 200%


	p = 4;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetTranslation(spatialPose, +3.0f, +4.0f, +5.0f);	// shift whole figure by some vector
*/

	// load from file
	a3hierarchyPoseGroupLoadHTR(demoMode->hierarchyPoseGroup_skel, demoMode->hierarchy_skel,
		"../../../../resource/animdata/egnaro/egnaro_skel_anim.htr");
	a3hierarchyStateCreate(demoMode->hierarchyState_skel, demoMode->hierarchy_skel);
	

	// finally set up hierarchy states
/*
	// base state for skeleton
	hierarchyState = demoMode->hierarchyState_skel;
	hierarchyState->hierarchy = 0;
	a3hierarchyStateCreate(hierarchyState, hierarchy);
	a3hierarchyPoseCopy(hierarchyState->localSpace, hierarchyPoseGroup->hpose, hierarchy->numNodes);
	a3hierarchyPoseConvert(hierarchyState->localSpace, hierarchy->numNodes, hierarchyPoseGroup->channel, hierarchyPoseGroup->order);
	a3kinematicsSolveForward(hierarchyState);
	a3hierarchyStateUpdateObjectInverse(hierarchyState);
*/
	// real-time state
	hierarchyState = demoMode->hierarchyState_skel + 1;
	hierarchyState->hierarchy = 0;
	a3hierarchyStateCreate(hierarchyState, hierarchy);


#pragma region //////////////////////// TEMP TESTING: Set up animator ////////////////////////

#pragma region //////////// Init keyframe data ////////////

	const a3real frameRate = 1.0f / 24; //FIXME don't hard code
	
	a3_KeyframePool* keyframes = malloc(sizeof(a3_KeyframePool));
	a3keyframePoolCreate(keyframes, demoMode->hierarchyPoseGroup_skel->spatialPoseCount, &a3_SpatialPoseInterpolationInfo);
	for (a3ui32 i = 0; i < demoMode->hierarchyPoseGroup_skel->spatialPoseCount; ++i)
	{
		a3keyframeInit(&keyframes->keyframe[i], frameRate, &demoMode->hierarchyPoseGroup_skel->spatialPosePool[i]);
	}

#pragma endregion

#pragma region //////////// Init clips ////////////

	const a3ui32 animFrameCount = 81; //FIXME don't hard code

	a3_ClipPool* clips = malloc(sizeof(a3_ClipPool));
	a3clipPoolCreate(clips, 4);

	// Init base pose - special case, cannot use utility function
	// # @ clip_name	duration_s	first_frame	last_frame	reverse_transition	forward_transition	comments (ignored)
	// @ 	basepose	0.0			0			0			|					|
	a3_Clip* clip = &clips->clip[0];
	{
		a3clipInit(clip, "basepose", demoMode->hierarchyState_skel->hierarchy->numNodes);
		for (a3ui32 i = 0; i < demoMode->hierarchyState_skel->hierarchy->numNodes; ++i)
		{
			a3_HierarchyNode* node = &demoMode->hierarchyState_skel->hierarchy->nodes[i];
			a3keyframeChannelInit(clip->channels, node->name, keyframes, i, i);
		}
		a3clipCalculateDuration(clip);
		clip->forwardTransition.flags = EC_TERMINUSACTION_PAUSE; clip->forwardTransition.targetClipID = 0;
		clip->reverseTransition.flags = EC_TERMINUSACTION_PAUSE; clip->reverseTransition.targetClipID = 0;
	}
	
	// Init "calibration"
	// # @ clip_name	duration_s	first_frame	last_frame	reverse_transition	forward_transition	comments (ignored)
	// @ 	calibration	1.0			1			27			> calibration		>| calibration
	clip = &clips->clip[1];
	createPoseClip(clip, "calibration", demoMode->hierarchyState_skel, keyframes, 1, 27, animFrameCount);
	clip->reverseTransition.flags = EC_TERMINUSACTION_FORWARD                          ; clip->reverseTransition.targetClipID = 1;
	clip->forwardTransition.flags = EC_TERMINUSACTION_FORWARD | EC_TERMINUSACTION_PAUSE; clip->forwardTransition.targetClipID = 1;

	// Init "idle"
	// # @ clip_name	duration_s	first_frame	last_frame	reverse_transition	forward_transition	comments (ignored)
	// @ 	idle		4.0			28			52			<<					>
	clip = &clips->clip[2];
	createPoseClip(clip, "idle", demoMode->hierarchyState_skel, keyframes, 28, 52, animFrameCount);
	clip->reverseTransition.flags = EC_TERMINUSACTION_REVERSE | EC_TERMINUSACTION_SKIP;
	clip->forwardTransition.flags = EC_TERMINUSACTION_FORWARD                         ;

	// Init "dance"
	// # @ clip_name	duration_s	first_frame	last_frame	reverse_transition	forward_transition	comments (ignored)
	// @ 	dance		1.5			54			78			<<					>
	clip = &clips->clip[3];
	createPoseClip(clip, "dance", demoMode->hierarchyState_skel, keyframes, 54, 78, animFrameCount);
	clip->reverseTransition.flags = EC_TERMINUSACTION_REVERSE | EC_TERMINUSACTION_SKIP;
	clip->forwardTransition.flags = EC_TERMINUSACTION_FORWARD                         ;
	
#pragma endregion

	a3clipControllerInit(&demoMode->skeletonAnimator, "Skeleton animator", clips, 3);
	
	// Write base pose
	demoMode->hierarchyState_skel->bindPose = demoMode->hierarchyPoseGroup_skel->spatialPosePool;

#pragma endregion
}


//-----------------------------------------------------------------------------

void a3animation_input(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode, a3f64 const dt);
void a3animation_update(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode, a3f64 const dt);
void a3animation_render(a3_DemoState const* demoState, a3_DemoMode1_Animation const* demoMode, a3f64 const dt);
void a3animation_input_keyCharPress(a3_DemoState const* demoState, a3_DemoMode1_Animation* demoMode, a3i32 const asciiKey, a3i32 const state);
void a3animation_input_keyCharHold(a3_DemoState const* demoState, a3_DemoMode1_Animation* demoMode, a3i32 const asciiKey, a3i32 const state);

void a3animation_loadValidate(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode)
{
	// initialize callbacks
	a3_DemoModeCallbacks* const callbacks = demoState->demoModeCallbacks + demoState_modeAnimation;
	callbacks->demoMode = demoMode;
	callbacks->handleInput =	(a3_DemoMode_EventCallback)		a3animation_input;
	callbacks->handleUpdate =	(a3_DemoMode_EventCallback)		a3animation_update;
	callbacks->handleRender =	(a3_DemoMode_EventCallbackConst)a3animation_render;
	callbacks->handleKeyPress = (a3_DemoMode_InputCallback)		a3animation_input_keyCharPress;
	callbacks->handleKeyHold =	(a3_DemoMode_InputCallback)		a3animation_input_keyCharHold;

	// initialize cameras dependent on viewport
	demoMode->proj_camera_main->aspect = demoState->frameAspect;
	a3demo_updateProjectorProjectionMat(demoMode->proj_camera_main);
	a3demo_setProjectorSceneObject(demoMode->proj_camera_main, demoMode->obj_camera_main);

	// initialize cameras not dependent on viewport

	// animation
	demoMode->hierarchyState_skel->hierarchy = demoMode->hierarchy_skel;
	demoMode->hierarchyPoseGroup_skel->hierarchy = demoMode->hierarchy_skel;
}


void a3animation_load(a3_DemoState const* demoState, a3_DemoMode1_Animation* demoMode)
{
	a3ui32 i;

	a3_DemoSceneObject* currentSceneObject;
	a3_DemoProjector* projector;


	// camera's starting orientation depends on "vertical" axis
	// we want the exact same view in either case
	const a3real sceneCameraAxisPos = 20.0f;
	const a3vec3 sceneCameraStartPos = {
		+sceneCameraAxisPos,
		-sceneCameraAxisPos,
		+sceneCameraAxisPos + 5.0f,
	};
	const a3vec3 sceneCameraStartEuler = {
		+55.0f,
		+0.0f,
		+45.0f,
	};
	const a3f32 sceneObjectDistance = 8.0f;
	const a3f32 sceneObjectHeight = 2.0f;


	// all objects
	for (i = 0; i < animationMaxCount_sceneObject; ++i)
		a3demo_initSceneObject(demoMode->object_scene + i);
	for (i = 0; i < animationMaxCount_cameraObject; ++i)
		a3demo_initSceneObject(demoMode->object_camera + i);
	for (i = 0; i < animationMaxCount_projector; ++i)
		a3demo_initProjector(demoMode->projector + i);

	currentSceneObject = demoMode->obj_skybox;
	currentSceneObject->scaleMode = 1;
	currentSceneObject->scale.x = 256.0f;

	currentSceneObject = demoMode->obj_skeleton;
	currentSceneObject->scaleMode = 0;


	// set up cameras
	projector = demoMode->proj_camera_main;
	projector->perspective = a3true;
	projector->fovy = a3real_fortyfive;
	projector->znear = 1.0f;
	projector->zfar = 1024.0f;
	projector->ctrlMoveSpeed = 10.0f;
	projector->ctrlRotateSpeed = 5.0f;
	projector->ctrlZoomSpeed = 5.0f;
	projector->sceneObject->position = sceneCameraStartPos;
	projector->sceneObject->euler = sceneCameraStartEuler;


	// set flags
	demoMode->render = animation_renderPhong;
	demoMode->display = animation_displayTexture;
	demoMode->activeCamera = animation_cameraSceneViewer;

	demoMode->pipeline = animation_forward;
	demoMode->pass = animation_passComposite;

	demoMode->targetIndex[animation_passScene] = animation_scene_finalcolor;
	demoMode->targetIndex[animation_passComposite] = animation_scene_finalcolor;

	demoMode->targetCount[animation_passScene] = animation_target_scene_max;
	demoMode->targetCount[animation_passComposite] = 1;


	// setup
	a3animation_init_animation(demoState, demoMode);
}


//-----------------------------------------------------------------------------
