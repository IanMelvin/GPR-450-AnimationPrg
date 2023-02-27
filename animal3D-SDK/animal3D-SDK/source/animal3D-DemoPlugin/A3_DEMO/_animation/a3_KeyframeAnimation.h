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
	
	a3_KeyframeAnimation.h
	Data structures for fundamental keyframe animation.
*/

/*
	Modified by Ian Melvin and Robert Christensen
	Purpose: Hold the defininition for Keyframe,KeyFramePool, clip and clipPool
	Ian - Defined variables for Keyframe,KeyFramePool, clip and clipPool
	Robert - Defined variables for clipPool, added ec_KeyframeInterpolationMode, added utility functions ec_clip_getTerminusAction and ec_terminusActionFlags_getDirection
*/

#ifndef __ANIMAL3D_KEYFRAMEANIMATION_H
#define __ANIMAL3D_KEYFRAMEANIMATION_H


#include "animal3D-A3DM/a3math/a3vector.h"
#include "animal3D-A3DM/a3math/a3interpolation.h"

#include "../ec_Interpolation.h"
#include "ec_TerminusAction.h"

//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_KeyframeChannel			a3_KeyframeChannel;
typedef struct a3_ChannelPlayhead			a3_ChannelPlayhead;
typedef struct a3_Keyframe					a3_Keyframe;
typedef struct a3_KeyframePool				a3_KeyframePool;
typedef struct a3_Clip						a3_Clip;
typedef struct a3_ClipPool					a3_ClipPool;
typedef void								a3_Keyframe_data_t;
typedef a3ui32								channel_id_t;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

// description of a channel containing keyframes
// metaphor: timeline, but for one property only
struct a3_KeyframeChannel
{
	// identifiers
	const char* name;

	// number of keyframes referenced by clip (including first and last)
	a3ui32 keyframeCount;

	// index of first keyframe in pool referenced
	a3ui32 firstKeyframe;

	// index of final keyframe in pool referenced
	a3ui32 lastKeyframe;

	// data storage
	a3_KeyframePool* keyframePool;
};

struct a3_ChannelPlayhead
{
	// index of current keyframe in referenced keyframe pool
	a3ui32 keyframe;

	// current time relative to current keyframe; should always be between 0 and current keyframe's duration
	a3f32 keyframeTime;

	// normalized keyframe time; should always be between 0 and 1
	a3f32 keyframeParameter;
};

a3i32 a3keyframeChannelInit(a3_KeyframeChannel* channel_out, const char* name, const a3_KeyframePool* keyframePool, const a3ui32 firstKeyframeIndex, const a3ui32 finalKeyframeIndex);

// calculate keyframes' durations by distributing clip's duration
a3i32 a3keyframesDistributeDuration(a3_KeyframeChannel* keyframes, const a3real newDuration);

// get a keyframe by id
a3_Keyframe* ec_channel_getKeyframe(a3_KeyframeChannel const* channel, a3ui32 id);

//-----------------------------------------------------------------------------

// constant values
enum
{
	a3keyframeAnimation_nameLenMax = 32,
};

// description of single keyframe
// metaphor: moment
struct a3_Keyframe
{
	// index in keyframe pool
	a3ui32 index;

	// interval of time for which this keyframe is active; cannot be zero
	a3f32 duration;

	// reciprocal of duration
	a3f32 durationInv;

	// value of the sample described by a keyframe
	// NOTE: stored elsewhere
	const a3_Keyframe_data_t* data;

	// how to interpolate data
	ec_InterpolationMode interpolationMode;
};

// pool of keyframe descriptors
struct a3_KeyframePool
{
	// array of keyframes
	a3_Keyframe *keyframe;

	// number of keyframes
	a3ui32 count;
	
	// how do we interpolate between keyframe values?
	const ec_InterpolationFuncFamily* interpolationFuncs;
};

// allocate keyframe pool
a3i32 a3keyframePoolCreate(a3_KeyframePool* keyframePool_out, const a3ui32 count, const ec_InterpolationFuncFamily* interpolationFuncs);

// release keyframe pool
a3i32 a3keyframePoolRelease(a3_KeyframePool* keyframePool);

// initialize keyframe
// NOTE: value_x stored elsewhere
a3i32 a3keyframeInit(a3_Keyframe* keyframe_out, const a3real duration, const a3_Keyframe_data_t* value_x);


//-----------------------------------------------------------------------------

// description of single clip
// metaphor: timeline
struct a3_Clip
{
	// clip name
	a3byte name[a3keyframeAnimation_nameLenMax];

	// duration of clip; can be calculated as the sum of all of the referenced keyframes or set first and distributed uniformly across keyframes; cannot be zero
	a3f32 duration;

	// reciprocal of duration
	a3f32 durationInv;

	// channels
	a3ui32 channelCount;
	a3_KeyframeChannel* channels;

	ec_terminusAction forwardTransition;
	ec_terminusAction reverseTransition;
};

// group of clips
struct a3_ClipPool
{
	// array of clips
	a3_Clip* clip;

	// number of clips
	a3ui32 count;
};


// allocate clip pool
a3i32 a3clipPoolCreate(a3_ClipPool* clipPool_out, const a3ui32 count);

// release clip pool
a3i32 a3clipPoolRelease(a3_ClipPool* clipPool);

// initialize clip with first and last indices
a3i32 a3clipInit(a3_Clip* clip_out, const a3byte clipName[a3keyframeAnimation_nameLenMax], const a3ui32 channelCount);

// get clip index from pool
a3i32 a3clipGetIndexInPool(const a3_ClipPool* clipPool, const a3byte clipName[a3keyframeAnimation_nameLenMax]);

// calculate clip duration as sum of keyframes' durations
a3i32 a3clipCalculateDuration(a3_Clip* clip);

typedef a3i8 ec_sign;

// retrieve the indicated terminus action
ec_terminusAction* ec_clip_getTerminusAction(a3_Clip* clip, ec_sign direction);

// get the direction indicated by flags
ec_sign ec_terminusActionFlags_getDirection(ec_terminusActionFlags flags);

// get a channel by name
a3ui32 a3clipGetChannelID(const a3_Clip* clip, const char* name);
a3_KeyframeChannel* a3clipGetChannelByName(const a3_Clip* clip, const char* name);

//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_KeyframeAnimation.inl"


#endif	// !__ANIMAL3D_KEYFRAMEANIMATION_H
