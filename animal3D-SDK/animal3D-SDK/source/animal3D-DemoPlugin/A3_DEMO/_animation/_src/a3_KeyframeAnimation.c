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
	
	a3_KeyframeAnimation.c
	Implementation of keyframe animation interfaces.
*/

/*
	Modified by Ian Melvin and Robert Christensen
	Purpose: Hold the implementation of the functions related to Keyframe,KeyFramepool, clip and clippool
	Ian - Implemented functions
	Robert - Modified and polished functions
*/

#include "../a3_KeyframeAnimation.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

// macros to help with names
#define A3_CLIP_DEFAULTNAME		("unnamed clip")
#define A3_CLIP_SEARCHNAME		((clipName && *clipName) ? clipName : A3_CLIP_DEFAULTNAME)


//-----------------------------------------------------------------------------

a3i32 a3keyframeChannelInit(a3_KeyframeChannel* channel_out, const a3_KeyframePool* keyframePool, const a3ui32 firstKeyframeIndex, const a3ui32 finalKeyframeIndex)
{
	assert(keyframePool);
	assert(0 < firstKeyframeIndex);
	assert(firstKeyframeIndex < finalKeyframeIndex);
	assert(finalKeyframeIndex < keyframePool->count);

	channel_out->keyframePool = (a3_KeyframePool*)keyframePool;
	channel_out->firstKeyframe = firstKeyframeIndex;
	channel_out->lastKeyframe = finalKeyframeIndex;

	channel_out->keyframeCount = finalKeyframeIndex - firstKeyframeIndex + 1;

	return 1;
}

// allocate keyframe pool initializing all to default values.
a3i32 a3keyframePoolCreate(a3_KeyframePool* keyframePool_out, const a3ui32 count, const ec_InterpolationFuncFamily* interpolationFuncs)
{
	keyframePool_out->keyframe = (a3_Keyframe*)malloc(count * sizeof(a3_Keyframe));
	keyframePool_out->count = count;
	keyframePool_out->interpolationFuncs = interpolationFuncs;

	//Define default values
	for (a3ui32 i = 0; i < keyframePool_out->count; i++)
	{
		a3keyframeInit(&keyframePool_out->keyframe[i], 0, 0);
		keyframePool_out->keyframe[i].index = i;
	}

	return 1;
}

// release keyframe pool
a3i32 a3keyframePoolRelease(a3_KeyframePool* keyframePool)
{
	free(keyframePool->keyframe); //Alternate way to do it: realloc(keyframePool->keyframe, 0)
	return 1;
}

// initialize keyframe
// NOTE: value_x stored elsewhere
a3i32 a3keyframeInit(a3_Keyframe* keyframe_out, const a3real duration, const a3_Keyframe_data_t* value_x)
{
	keyframe_out->duration = duration;
	keyframe_out->durationInv = 1 / duration;
	keyframe_out->data = value_x;
	keyframe_out->interpolationMode = EC_INTERPOLATE_DEFAULT;
	return 1;
}


// allocate clip pool
a3i32 a3clipPoolCreate(a3_ClipPool* clipPool_out, const a3ui32 count)
{
	clipPool_out->clip = (a3_Clip*)calloc(count, sizeof(a3_Clip));
	clipPool_out->count = count;

	//Define Default values
	for (a3ui32 i = 0; i < count; i++)
	{
		a3clipInit(&clipPool_out->clip[i], "", 0);
	}

	return 1;
}

// release clip pool
a3i32 a3clipPoolRelease(a3_ClipPool* clipPool)
{
	free(clipPool->clip); //Alternate way to do it: realloc(clipPool->clip, 0)
	return 1;
}

// initialize clip with first and last indices
a3i32 a3clipInit(a3_Clip* clip_out, const a3byte clipName[a3keyframeAnimation_nameLenMax], const a3ui32 channelCount)
{
	strcpy_s(clip_out->name, a3keyframeAnimation_nameLenMax, clipName);

	clip_out->channelCount = channelCount;
	if (!clip_out->channels) clip_out->channels = calloc(channelCount, sizeof(a3_KeyframeChannel));
	else                     clip_out->channels = realloc(clip_out->channels, channelCount * sizeof(a3_KeyframeChannel));

	// Default transition: loop self
	clip_out->forwardTransition.flags = EC_TERMINUSACTION_FORWARD;
	clip_out->forwardTransition.targetClipID = NULL_CLIP_ID;
	clip_out->reverseTransition.flags = EC_TERMINUSACTION_REVERSE;
	clip_out->reverseTransition.targetClipID = NULL_CLIP_ID;

	return 1;
}

// get clip index from pool
a3i32 a3clipGetIndexInPool(const a3_ClipPool* clipPool, const a3byte clipName[a3keyframeAnimation_nameLenMax])
{;
 	// Iterate through clip pool looking for correct clip
	for (a3ui32 i = 0; i < clipPool->count; i++)
	{
		if (clipPool->clip[i].name == clipName)
		{
			return i;
		}
	}
	return 1;
}

// get a keyframe by id
a3_Keyframe* ec_channel_getKeyframe(a3_KeyframeChannel const* channel, a3ui32 id)
{
	assert(0 <= id && id < channel->keyframeCount);
	return &(channel->keyframePool->keyframe[channel->firstKeyframe + id]);
}

// retrieve the indicated terminus action
ec_terminusAction* ec_clip_getTerminusAction(a3_Clip* clip, ec_sign direction)
{
	assert(direction != 0);
	assert(clip);
	return direction>0 ? &clip->forwardTransition : &clip->reverseTransition;
}

// get the direction indicated by flags
ec_sign ec_terminusActionFlags_getDirection(ec_terminusActionFlags flags)
{
	assert(flags != 0); //Must have some form of terminus action
	assert(!(flags&EC_TERMINUSACTION_REVERSE && flags&EC_TERMINUSACTION_FORWARD)); //Cannot be both forward and reverse
	return (flags&EC_TERMINUSACTION_REVERSE ? -1 : 0) + (flags&EC_TERMINUSACTION_FORWARD ? 1 : 0);
}


//-----------------------------------------------------------------------------
