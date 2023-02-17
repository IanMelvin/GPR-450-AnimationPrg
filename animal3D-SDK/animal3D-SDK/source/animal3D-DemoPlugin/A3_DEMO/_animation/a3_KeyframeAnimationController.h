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
	Keyframe animation clip controller. Basically a frame index manager. Very 
	limited in what one can do with this; could potentially be so much more.
*/

/*
	Modified by Ian Melvin and Robert Christensen
	Purpose: Define the clip controller and it's functions
	Ian - Defined variables
	Robert - Defined variables
*/

#ifndef __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H
#define __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H


#include "a3_KeyframeAnimation.h"
#include "ec_TerminusAction.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_ClipController			a3_ClipController;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

//Option: Use relative vs absolute keyframe time calculation (see usage in a3clipControllerUpdate for more details)
#define EC_USE_RELATIVE_KEYFRAME_DT 0
//#define EC_USE_RELATIVE_KEYFRAME_DT 1

// clip controller
// metaphor: playhead
struct a3_ClipController
{
	// controller name
	a3byte name[a3keyframeAnimation_nameLenMax];

	// index of clip to control in referenced clip pool
	a3ui32 clipIndex;

	// current time relative to start of clip; should always be between 0 and current clip's duration
	a3f32 clipTime;

	// normalized clip time; should always be between 0 and 1. 
	a3f32 clipParameter;

	// index of current keyframe in referenced keyframe pool
	a3ui32 keyframe;

	// current time relative to current keyframe; should always be between 0 and current keyframe's duration
	a3f32 keyframeTime;

	// normalized keyframe time; should always be between 0 and 1
	a3f32 keyframeParameter;

	// the active behavior of playback; try +1 for forward and -1 for reverse
	a3f32 speed;

	// true if paused and false if playing
	a3boolean isPaused;

	// pointer to clip pool
	a3_ClipPool* clipPool;
};

//-----------------------------------------------------------------------------

// initialize clip controller
a3i32 a3clipControllerInit(a3_ClipController* clipCtrl_out, const a3byte ctrlName[a3keyframeAnimation_nameLenMax], const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool);

// update clip controller
a3i32 a3clipControllerUpdate(a3_ClipController* clipCtrl, const a3real dt);

// evaluate the current value
a3i32 ec_clipController_evaluateValue(a3_Keyframe_data_t* out, a3_ClipController const* clipCtrl);

// time-ticking functions
a3i32 ec_clipController_incrementTimeScaled(a3_ClipController* clipCtrl, a3real wallDt); // tick using wall time
a3i32 ec_clipController_incrementTimeUnscaled(a3_ClipController* clipCtrl, a3real animDt); // tick using animation time
a3i32 ec_clipController_updateParameterTime(a3_ClipController* clipCtrl); // update clipParameter, keyframeParameter

// set clip to play
a3i32 a3clipControllerSetClip(a3_ClipController* clipCtrl, const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool);

// get currently played clip
a3_Clip* ec_clipController_getClip(a3_ClipController const* clipCtrl);

// check if time is within bounds
a3boolean ec_clipController_isTimeWithinBounds(a3_ClipController const* clipCtrl, a3boolean endInclusive /* = a3false*/);

// calculate how far out of bounds time is
a3real ec_clipController_getClipOverstep(a3_ClipController const* clipCtrl);

// process the indicated terminus action (does not check conditions!)
a3i32 ec_clipController_processTerminusAction(a3_ClipController* clipCtrl, ec_terminusAction* action);

//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus

#include "_inl/a3_KeyframeAnimationController.inl"

#endif	// !__ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H
