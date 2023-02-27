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
	
	a3_KeyframeAnimation.inl
	Inline definitions for keyframe animation.
*/

/*
	Modified by Ian Melvin
	Purpose: Hold the implementation of the functions related to determining clip Duration
	Ian - Implemented functions
*/

#ifdef __ANIMAL3D_KEYFRAMEANIMATION_H
#ifndef __ANIMAL3D_KEYFRAMEANIMATION_INL
#define __ANIMAL3D_KEYFRAMEANIMATION_INL


//-----------------------------------------------------------------------------

// calculate clip duration as sum of keyframes' durations
inline a3i32 a3clipCalculateDuration(a3_Clip* clip)
{
	clip->duration = 0;

	for (a3ui32 channel = 0; channel < clip->channelCount; channel++)
	{
		//Sum up keyframe duration
		a3f32 channelDuration = 0;
		for (a3ui32 i = clip->channels[channel].firstKeyframe; i < clip->channels[channel].lastKeyframe; i++)
		{
			channelDuration += clip->channels[channel].keyframePool->keyframe[i].duration;
		}
		clip->duration = a3maximum(clip->duration, channelDuration);
	}

	clip->durationInv = 1 / clip->duration;

	return 1;
}

// calculate keyframes' durations by distributing clip's duration
inline a3i32 a3keyframesDistributeDuration(a3_KeyframeChannel* keyframes, const a3real newDuration)
{
	a3real keyframeDur = newDuration / keyframes->keyframeCount;
	a3real keyframeDurInv = 1 / keyframeDur;

	//Set duration for each keyframe
	for (a3ui32 i = keyframes->firstKeyframe; i < keyframes->lastKeyframe; i++)
	{
		keyframes->keyframePool->keyframe[i].duration = keyframeDur;
		keyframes->keyframePool->keyframe[i].durationInv = keyframeDurInv;
	}

	return 1;
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_KEYFRAMEANIMATION_INL
#endif	// __ANIMAL3D_KEYFRAMEANIMATION_H
