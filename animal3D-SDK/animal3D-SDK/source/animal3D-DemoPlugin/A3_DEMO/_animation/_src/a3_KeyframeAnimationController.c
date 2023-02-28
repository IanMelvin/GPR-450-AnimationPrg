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
	
	a3_KeyframeAnimationController.c
	Implementation of keyframe animation controller.
*/

/*
	Modified by Ian Melvin and Robert Christensen
	Purpose: Hold the implementation of the functions related to Keyframe,KeyFramepool, clip and clippool
	Ian - Implemented a3clipControllerInit
	Robert - Implemented a3ClipControllerUpdate and ec_clipController_*
*/

#include "../a3_KeyframeAnimationController.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

//See https://stackoverflow.com/a/4609795
#define sign(val) ( (0 < val) - (val < 0) )
#define abs(val) ( val>0 ? val : -val )

//-----------------------------------------------------------------------------

// initialize clip controller
a3i32 a3clipControllerInit(a3_ClipController* clipCtrl_out, const a3byte ctrlName[a3keyframeAnimation_nameLenMax], const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool)
{
	//Assign default values
	strcpy_s(clipCtrl_out->name, a3keyframeAnimation_nameLenMax, ctrlName);
	clipCtrl_out->clipPool = (a3_ClipPool*)clipPool;
	clipCtrl_out->clipIndex = clipIndex_pool;
	clipCtrl_out->clipTime = 0;
	clipCtrl_out->clipParameter = 0;
	clipCtrl_out->channelPlayheads = NULL;
	clipCtrl_out->channelPlayheadsAllocated = 0;
	clipCtrl_out->speed = 1;

	clipCtrl_out->isPaused = a3false;

	if (clipPool) ec_clipController_preparePlayheads(clipCtrl_out, ec_clipController_getClip(clipCtrl_out));

	return -1;
}

// update clip controller
a3i32 a3clipControllerUpdate(a3_ClipController* clipCtrl, const a3real dt)
{
	//Tick time
	ec_clipController_incrementTimeScaled(clipCtrl, dt);
	
	//Resolve clip overstep & terminus actions
	while (!clipCtrl->isPaused && !ec_clipController_isTimeWithinBounds(clipCtrl, a3false))
	{
		ec_clipController_processTerminusAction(
			clipCtrl,
			ec_clip_getTerminusAction(ec_clipController_getClip(clipCtrl), sign(clipCtrl->clipTime))
		);
	}

	//Update normalized parameters
	ec_clipController_updateParameterTime(clipCtrl);

	return 1;
}

// evaluate the current value
a3i32 ec_clipController_evaluateValue(a3_Keyframe_data_t* out, a3_ClipController const* clipCtrl, const channel_id_t channelIndex)
{
	a3_Clip* currentClip = ec_clipController_getClip(clipCtrl);
	assert(0 <= channelIndex && channelIndex < currentClip->channelCount);
	assert(currentClip->channels);
	a3_KeyframeChannel* channel = &currentClip->channels[channelIndex];
	a3_ChannelPlayhead* playhead = &clipCtrl->channelPlayheads[channelIndex];
	size_t keyframeValSize = channel->keyframePool->interpolationFuncs->valSize;

	a3_Keyframe* x0 = ec_channel_getKeyframe(channel, playhead->keyframe);
	
	//Special case for very last keyframe
	if (playhead->keyframe == channel->keyframeCount - 1)
	{
		//*out = *x0->data;
		memcpy(out, x0->data, keyframeValSize);
		return 1;
	}

	a3_Keyframe* x1 = ec_channel_getKeyframe(channel, playhead->keyframe+1);

	//Do normal interpolation
	ec_interpolate(out, x0->data, x1->data, playhead->keyframeParameter, channel->keyframePool->interpolationFuncs, x0->interpolationMode);
	return 1;
}

// time-ticking functions
a3i32 ec_clipController_incrementTimeScaled(a3_ClipController* clipCtrl, a3real wallDt) //Uses wall clock time
{
	a3real effectiveDt = !clipCtrl->isPaused ? wallDt * clipCtrl->speed : 0;
	ec_clipController_incrementTimeUnscaled(clipCtrl, effectiveDt);
	return a3true;
}
a3i32 ec_clipController_incrementTimeUnscaled(a3_ClipController* clipCtrl, a3real animDt) //Uses animation time
{
	//Update clip time
	clipCtrl->clipTime += animDt;

	//Update keyframe time
	a3_Clip* currentClip = ec_clipController_getClip(clipCtrl);
	assert(currentClip->channels);
	for (a3ui32 i = 0; i < currentClip->channelCount; ++i)
	{
		//TODO @rsc this should probably be its own function
		a3_KeyframeChannel* channel = &currentClip->channels[i];
		a3_ChannelPlayhead* playhead = &clipCtrl->channelPlayheads[i];
		
	#if EC_USE_RELATIVE_KEYFRAME_DT
		//Relative method: potential floating point error, but more performant
		playhead->keyframeTime += animDt;
	#else
		//Absolute method: more reliable but slower
		//Keyframe time = clip time - duration of previous keyframes
		playhead->keyframe = 0;
		playhead->keyframeTime = clipCtrl->clipTime;
	#endif
		//Resolve keyframe overstep
		while (playhead->keyframeTime >= ec_channel_getKeyframe(channel, playhead->keyframe)->duration && playhead->keyframe < channel->keyframeCount)
		{
			playhead->keyframeTime -= ec_channel_getKeyframe(channel, playhead->keyframe)->duration;
			playhead->keyframe++;
		}
	}


	return a3true;
}
a3i32 ec_clipController_updateParameterTime(a3_ClipController* clipCtrl)
{
	a3_Clip* currentClip = ec_clipController_getClip(clipCtrl);
	assert(currentClip->channels);
	for (a3ui32 i = 0; i < currentClip->channelCount; ++i)
	{
		a3_KeyframeChannel* channel = &currentClip->channels[i];
		a3_ChannelPlayhead* playhead = &clipCtrl->channelPlayheads[i];
		playhead->keyframeParameter = playhead->keyframeTime * ec_channel_getKeyframe(channel, playhead->keyframe)->durationInv;
	}
	clipCtrl->clipParameter = clipCtrl->clipTime * currentClip->durationInv;
	return a3true;
}

// set clip to play
a3i32 a3clipControllerSetClip(a3_ClipController* clipCtrl, const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool)
{
	return -1;
}

// get currently played clip
a3_Clip* ec_clipController_getClip(a3_ClipController const* clipCtrl)
{
	assert(clipCtrl);
	assert(0 <= clipCtrl->clipIndex && clipCtrl->clipIndex < clipCtrl->clipPool->count);
	return &(clipCtrl->clipPool->clip[clipCtrl->clipIndex]);
}

// check if time is within bounds
a3boolean ec_clipController_isTimeWithinBounds(a3_ClipController const* clipCtrl, a3boolean endInclusive /* = a3false*/)
{
	assert(clipCtrl);
	a3_Clip* currentClip = ec_clipController_getClip(clipCtrl);
	if (!endInclusive) return 0 <= clipCtrl->clipTime && clipCtrl->clipTime <  currentClip->duration;
	else               return 0 <= clipCtrl->clipTime && clipCtrl->clipTime <= currentClip->duration;
}

// calculate how far out of bounds time is
a3real ec_clipController_getClipOverstep(a3_ClipController const* clipCtrl)
{
	a3_Clip* currentClip = ec_clipController_getClip(clipCtrl);
	assert(clipCtrl->speed);
	if (clipCtrl->speed > 0) return clipCtrl->clipTime - currentClip->duration; //Forward playback case: Exits at duration
	else                     return -clipCtrl->clipTime; //Reverse playback case: Exits at 0
}

// process the indicated terminus action (does not check conditions!)
a3i32 ec_clipController_processTerminusAction(a3_ClipController* clipCtrl, ec_terminusAction* action)
{
	assert(!ec_clipController_isTimeWithinBounds(clipCtrl, false));

	a3i8 entryDirection = ec_terminusActionFlags_getDirection(action->flags);
	assert(entryDirection != 0 || action->flags == EC_TERMINUSACTION_PAUSE); //Only allow full stop if our only instruction is to pause

	//Calculate overstep
	float overstep = ec_clipController_getClipOverstep(clipCtrl); //Take away duration of clip we just processed
	assert(overstep >= 0); //Ensure we didn't understep

	//Special case for pause
	if (action->flags & EC_TERMINUSACTION_PAUSE)
	{
		clipCtrl->isPaused = a3true;
		overstep = 0; //Ignore all overstep
	}

	//Play next clip, if it exists. Otherwise take no action, as we will just operate on the current clip.
	if (action->targetClipID != NULL_CLIP_ID)
	{
		clipCtrl->clipIndex = action->targetClipID;
		ec_clipController_preparePlayheads(clipCtrl, ec_clipController_getClip(clipCtrl));
	}

	//Process forward/reverse entry
	clipCtrl->speed = entryDirection * abs(clipCtrl->speed);
	if (action->flags & EC_TERMINUSACTION_FORWARD)
	{
		//Forward playback case: Enter at t=0
		clipCtrl->clipTime = overstep;
		//No need to set manually, playheads are already zeroed by preparePlayheads
	}
	if (action->flags & EC_TERMINUSACTION_REVERSE)
	{
		//Reverse playback case: Enter at t=duration
		a3_Clip* currentClip = ec_clipController_getClip(clipCtrl);
		clipCtrl->clipTime = currentClip->duration - overstep;
		assert(currentClip->channels);
		for (a3ui32 i = 0; i < currentClip->channelCount; ++i)
		{
			a3_KeyframeChannel* channel = &currentClip->channels[i];
			a3_ChannelPlayhead* playhead = &clipCtrl->channelPlayheads[i];
			playhead->keyframe = channel->keyframeCount - 1;
			playhead->keyframeTime = ec_channel_getKeyframe(channel, playhead->keyframe)->duration;
			playhead->keyframeParameter = 1;
		}
	}
	
	//Process >> and << (skip first/last)
	//if (action->flags & EC_TERMINUSACTION_SKIP) clipCtrl->clipTime += entryDirection * timePerFrame;

	return a3true;
}

a3i32 ec_clipController_preparePlayheads(a3_ClipController* clipCtrl, const a3_Clip* clip)
{
	if (clipCtrl->channelPlayheadsAllocated < clip->channelCount)
	{
		if (clipCtrl->channelPlayheads) free(clipCtrl->channelPlayheads);
		clipCtrl->channelPlayheads = calloc(clip->channelCount, sizeof(a3_ChannelPlayhead));
		clipCtrl->channelPlayheadsAllocated = clip->channelCount;
	}
	else
	{
		memset(clipCtrl->channelPlayheads, 0, clip->channelCount*sizeof(a3_ChannelPlayhead));
	}

	return 1;
}

//-----------------------------------------------------------------------------
