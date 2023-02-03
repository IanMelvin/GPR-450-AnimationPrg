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

#include "../a3_KeyframeAnimationController.h"

#include <string.h>
#include <assert.h>

//See https://stackoverflow.com/a/4609795
#define sign(val) ( (0 < val) - (val < 0) )
#define abs(val) ( val>0 ? val : -val )

//-----------------------------------------------------------------------------

a3boolean ec_clipController_processTerminusAction(a3_ClipController* clipCtrl)
{
	a3_Clip* currentClip = &( clipCtrl->clipPool->clip[clipCtrl->clipIndex] );

	//Don't process terminus action if not yet hit
	if (0 <= clipCtrl->clipTime && clipCtrl->clipTime < currentClip->duration) return 0;

	//Detect which terminus action to process
	ec_terminusAction* action = sign(clipCtrl->clipTime) > 0 ? &currentClip->forwardTransition : &currentClip->reverseTransition;
	a3i8 entryDirection = (action->flags&EC_TERMINUSACTION_REVERSE ? -1 : 0) + (action->flags&EC_TERMINUSACTION_FORWARD ? 1 : 0);

	assert(sign(clipCtrl->clipTime) == sign(clipCtrl->speed)); //Ensure we didn't understep
	assert(action->flags != 0); //Must have some form of terminus action
	assert(!( action->flags&EC_TERMINUSACTION_REVERSE && action->flags&EC_TERMINUSACTION_FORWARD )); //Cannot be both forward and reverse
	assert(entryDirection != 0 || action->flags == EC_TERMINUSACTION_PAUSE); //Only allow missing entry direction if our only instruction is to pause

	//Calculate overstep
	float overstep = clipCtrl->speed > 0 //Take away duration of clip we just processed
		? clipCtrl->clipTime - currentClip->duration //Forward playback case: Leave at duration
		: -clipCtrl->clipTime; //Reverse playback case: Leave at 0
	assert(overstep >= 0);

	//Special case for pause
	if (action->flags & EC_TERMINUSACTION_PAUSE)
	{
		clipCtrl->isPaused = a3true;
		overstep = 0; //Ignore all overstep
	}

	//Play next clip, if it exists. Otherwise take no action, as we will just operate on the current clip.
	if (action->targetClipID != NULL_CLIP_ID) clipCtrl->clipIndex = action->targetClipID;

	//Process forward/reverse entry
	clipCtrl->speed = entryDirection * abs(clipCtrl->speed);
	if (action->flags & EC_TERMINUSACTION_FORWARD)
	{
		//Forward playback case: Enter at t=0
		clipCtrl->clipTime = overstep;
		clipCtrl->keyframe = 0;
		clipCtrl->keyframeTime = 0;
		clipCtrl->keyframeParameter = 0;
	}
	if (action->flags & EC_TERMINUSACTION_REVERSE)
	{
		//Reverse playback case: Enter at t=duration
		clipCtrl->clipTime = currentClip->duration - overstep;
		clipCtrl->keyframe = currentClip->keyframeCount - 1;
		clipCtrl->keyframeTime = currentClip->keyframePool->keyframe[clipCtrl->keyframe].duration;
		clipCtrl->keyframeParameter = 1;
	}
	
	//Process >> and << (skip first/last)
	//if (action->flags & EC_TERMINUSACTION_SKIP) clipCtrl->clipTime += entryDirection * timePerFrame;

	return !(action->flags & EC_TERMINUSACTION_PAUSE); //Keep processing unless we hit a pause action
}

// update clip controller
a3i32 a3clipControllerUpdate(a3_ClipController* clipCtrl, const a3real dt)
{
	a3real effectiveDt = !clipCtrl->isPaused ? dt * clipCtrl->speed : 0;

	//Negative speed = reverse

	//Update clip time
	clipCtrl->clipTime += effectiveDt;

	//Update keyframe time
	a3_Clip* currentClip = &clipCtrl->clipPool->clip[clipCtrl->clipIndex];
#if EC_USE_RELATIVE_KEYFRAME_DT
	//Relative method: potential floating point error, but more performant
	clipCtrl->keyframeTime += effectiveDt;
#else
	//Absolute method: more reliable but slower
	//Keyframe time = clip time - duration of previous keyframes
	clipCtrl->keyframe = 0;
	clipCtrl->keyframeTime = 0;
#endif
	//Resolve keyframe overstep
	while (clipCtrl->keyframeTime >= currentClip->keyframePool->keyframe[clipCtrl->keyframe].duration && clipCtrl->keyframe < currentClip->keyframeCount)
	{
		clipCtrl->keyframeTime -= currentClip->keyframePool->keyframe[clipCtrl->keyframe].duration;
		clipCtrl->keyframe++;
	}

	//Resolve clip overstep & terminus actions
	while (!clipCtrl->isPaused && ec_clipController_processTerminusAction(clipCtrl));

	currentClip = &(clipCtrl->clipPool->clip[clipCtrl->clipIndex]);
	assert(0 <= clipCtrl->clipTime && clipCtrl->clipTime < currentClip->duration);

	//Update keyframe normalized parameter
	clipCtrl->keyframeParameter = clipCtrl->keyframeTime / currentClip->keyframePool->keyframe[clipCtrl->keyframe].duration;

	return a3true; //TODO @rsc what is this supposed to return?
}

// set clip to play
a3i32 a3clipControllerSetClip(a3_ClipController* clipCtrl, const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool)
{
	return -1;
}

// initialize clip controller
a3i32 a3clipControllerInit(a3_ClipController* clipCtrl_out, const a3byte ctrlName[a3keyframeAnimation_nameLenMax], const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool)
{
	strcpy_s(clipCtrl_out->name, a3keyframeAnimation_nameLenMax, ctrlName);
	clipCtrl_out->clipPool = (a3_ClipPool*)clipPool;
	clipCtrl_out->clipIndex = clipIndex_pool;
	clipCtrl_out->clipTime = 0;
	clipCtrl_out->clipParameter = 0;
	clipCtrl_out->keyframe = 0;
	clipCtrl_out->keyframeTime = 0;
	clipCtrl_out->keyframeParameter = 0;
	clipCtrl_out->speed = 1;

	clipCtrl_out->isPaused = a3false;

	return -1;
}


//-----------------------------------------------------------------------------
