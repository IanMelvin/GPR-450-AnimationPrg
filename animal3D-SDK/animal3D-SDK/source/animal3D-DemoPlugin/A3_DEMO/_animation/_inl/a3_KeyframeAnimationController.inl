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
	
	a3_KeyframeAnimationController.h
	inline definitions for keyframe animation controller.
*/

#ifdef __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H
#ifndef __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL
#define __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL


//-----------------------------------------------------------------------------

inline a3i8 ec_clipController_processTerminusAction(a3_ClipController* clipCtrl)
{
	//Don't process terminus action if not yet hit
	if (abs(clipCtrl->playhead.time) < clipCtrl->currentClip->duration) return 0;

	//Detect which terminus action to process
	ec_terminusAction* action = sign(clipCtrl->playhead.time) > 0 ? clipCtrl->currentClip->forwardTerminusAction : clipCtrl->currentClip->reverseTerminusAction);
	a3i8 entryDirection = (action->flags&EC_TERMINUSACTION_REVERSE ? -1 : 0) + (action->flags&EC_TERMINUSACTION_FORWARD ? 1 : 0);

	//Calculate overstep
	float overstep = abs(clipCtrl->playhead.time) - clipCtrl->currentClip->duration; //Otherwise take away duration of clip we just processed

	//Play next clip, if queued
	if (action->targetClip) clipCtrl->currentClip = action->targetClip;

	//Process forward/reverse entry
	clipCtrl->speed = entryDirection * abs(clipCtrl->speed);
	clipCtrl->playhead.time = overstep;

	//Special case for pause
	if (action->flags & EC_TERMINUSACTION_PAUSE)
	{
		clipCtrl->speed = 0; //Entry is paused
		clipCtrl->playhead.time = 0; //Ignore all overstep
	}
	
	//Process >> and << (skip first/last)
	if (action->flags & EC_TERMINUSACTION_SKIP) clipCtrl->playhead.time += entryDirection * timePerFrame;

	return ! (action->flags & EC_TERMINUSACTION_PAUSE); //Keep processing unless we hit a pause action
}

// update clip controller
inline a3i32 a3clipControllerUpdate(a3_ClipController* clipCtrl, const a3real dt)
{
	//Negative speed = reverse
	//Playhead time = playhead time - clip duration
	clipCtrl->playhead.time += dt*clipCtrl->speed;

	//Process overstep & terminus actions
	while (ec_clipController_processTerminusAction(clipCtrl));

	//Update keyframe pointers & normalized parameter
	//TODO @rsc linear or binary search to find next
	clipCtrl->playhead.keyframePrev = ???;
	clipCtrl->playhead.keyframeNext = ???;
	clipCtrl->playhead.keyframeParam = inverseLerp(
		clipCtrl->currentClip->keyframes[clipCtrl->playhead.keyframePrev].time,
		clipCtrl->currentClip->keyframes[clipCtrl->playhead.keyframeNext].time,
		clipCtrl->playhead.time
	);

	return -1; //TODO @rsc what is this supposed to return?
}

// set clip to play
inline a3i32 a3clipControllerSetClip(a3_ClipController* clipCtrl, const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool)
{
	return -1;
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL
#endif	// __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H