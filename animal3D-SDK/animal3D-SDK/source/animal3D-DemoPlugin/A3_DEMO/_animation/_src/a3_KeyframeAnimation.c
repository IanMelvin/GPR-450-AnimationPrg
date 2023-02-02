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

#include "../a3_KeyframeAnimation.h"

#include <stdlib.h>
#include <string.h>


// macros to help with names
#define A3_CLIP_DEFAULTNAME		("unnamed clip")
#define A3_CLIP_SEARCHNAME		((clipName && *clipName) ? clipName : A3_CLIP_DEFAULTNAME)


//-----------------------------------------------------------------------------

// allocate keyframe pool initializing all to default values.
// Need to initialize to default values
a3i32 a3keyframePoolCreate(a3_KeyframePool* keyframePool_out, const a3ui32 count)
{
	keyframePool_out->keyframe = (a3_Keyframe*)malloc(count * sizeof(a3_Keyframe));
	keyframePool_out->count = count;
	
	for (a3ui32 i = 0; i < keyframePool_out->count; i++)
	{
		// //Come back to later
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
a3i32 a3keyframeInit(a3_Keyframe* keyframe_out, const a3real duration, const a3ui32 value_x)
{
	keyframe_out->duration = duration;
	keyframe_out->durationInv = 1 / duration;
	keyframe_out->data = value_x;
	return 1;
}


// allocate clip pool
// Need to initialize to default values
a3i32 a3clipPoolCreate(a3_ClipPool* clipPool_out, const a3ui32 count)
{
	clipPool_out->clip = (a3_Clip*)malloc(count * sizeof(a3_Clip));
	clipPool_out->count = count;

	for (a3ui32 i = 0; i < count; i++)
	{
		a3clipInit(&clipPool_out->clip[i], NULL, NULL, 0, 0);
		clipPool_out->clip[i].index = i;
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
a3i32 a3clipInit(a3_Clip* clip_out, const a3byte clipName[a3keyframeAnimation_nameLenMax], const a3_KeyframePool* keyframePool, const a3ui32 firstKeyframeIndex, const a3ui32 finalKeyframeIndex)
{
	*clip_out->name = *clipName;
	*clip_out->keyframePool = *keyframePool;
	clip_out->firstKeyframe = firstKeyframeIndex;
	clip_out->lastKeyframe = finalKeyframeIndex;

	return 1;
}

// get clip index from pool
a3i32 a3clipGetIndexInPool(const a3_ClipPool* clipPool, const a3byte clipName[a3keyframeAnimation_nameLenMax])
{
	for (a3ui32 i = 0; i < clipPool->count; i++)
	{
		if (clipPool->clip[i].name == clipName)
		{
			return i;
		}
	}
	return 1;
}


//-----------------------------------------------------------------------------
