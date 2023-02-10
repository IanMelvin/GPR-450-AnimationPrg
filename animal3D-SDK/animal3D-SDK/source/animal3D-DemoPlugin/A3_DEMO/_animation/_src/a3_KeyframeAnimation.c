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
#include <cassert>

// macros to help with names
#define A3_CLIP_DEFAULTNAME		("unnamed clip")
#define A3_CLIP_SEARCHNAME		((clipName && *clipName) ? clipName : A3_CLIP_DEFAULTNAME)

// file io
#include <stdio.h>
#include "animal3D/a3utility/a3_Stream.h"
#define fileLineMaxLength ((size_t)256)


//-----------------------------------------------------------------------------

// allocate keyframe pool initializing all to default values.
a3i32 a3keyframePoolCreate(a3_KeyframePool* keyframePool_out, const a3ui32 count)
{
	keyframePool_out->keyframe = (a3_Keyframe*)malloc(count * sizeof(a3_Keyframe));
	keyframePool_out->count = count;

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

char fpeek(FILE* file)
{
	char c = fgetc(file);
	ungetc(c, file);
	return c;
}

a3boolean fgetline(FILE* file, char* buf_out)
{
	char* end = buf_out + fileLineMaxLength;

	char c;
	do {
		//Read stream
		c = fgetc(file);
		
		//Write value
		*buf_out = c;
		++buf_out;
		assert(buf_out != end);
	}
	while (c != '\n' && c != '\r');

	//Undo overscan
	ungetc(c, file);
	*buf_out = 0;

	return a3true;
}

a3i32 __skipComments(a3_FileStream const* in)
{
	char c = fgetc(in->stream);
	if (c != '#')
	{
		ungetc(c, in->stream); //Not a comment, don't skip anything
		return 0;
	}

	a3i32 nSkipped = 0; //Includes '#', but we will always overscan by 1, so it evens out

	while (!feof(in->stream) && (c = fgetc(in->stream)) && c != '\n') ++nSkipped; //Skip until we see a newline
	ungetc(c, in->stream); //Overscanned, put one back
	
	return nSkipped;
}

//Can be used both to skip whitespace and to capture fields
//All params except in can be left as NULL to ignore
a3i32 __capture(a3_FileStream const* in, char* bytesOut, const char* include, const char* exclude)
{
	char c;
	a3i32 bytesRead = 0;
	while (!feof(in->stream))
	{
		c = fgetc(in->stream);

		//Check against characters we should skip
		if ((!include || strchr(include, c)) && (!exclude || !strchr(exclude, c)))
		{
			if (bytesOut) bytesOut[bytesRead] = c;
			++bytesRead; //Skip until we see a newline
		}
		else
		{
			ungetc(c, in->stream); //Overscanned, put one back
			return bytesRead;
		}
	}

	//We reached EOF
	return bytesRead;
}

a3ret __decodeKeyframe(a3_Keyframe* keyframe_out, size_t channelIndex, char* buf)
{
	char* end = buf + fileLineMaxLength;

	//Remove leading whitespace
	while (strchr(" \t", *buf)) ++buf;
	assert(buf < end);

	//"@ " prefix
	assert(buf[0] == '@');

	//Remove leading whitespace
	while (strchr(" \t", *buf)) ++buf;
	assert(buf < end);

	//Read name
	//char name[fileLineMaxLength] = { 0 };
	//memcpy(name, buf, strchr(buf, '\t')-buf);

	//Seek to data point
	for (size_t i = 0; i < channelIndex; ++i)
	{
		while (*buf == '\t') ++buf; //Skip all whitespace
		while (*buf != '\t') ++buf; //Skip all non-whitespace
	}

	//Find end of value field
	char* valEnd = strchr(buf, '\t');
	valEnd = min(valEnd, end);

	//Copy value field (still in string form)
	char valStr[fileLineMaxLength] = { 0 };
	memcpy(valStr, buf, valEnd-buf);
	
	a3_Keyframe_data_t value = (a3_Keyframe_data_t) atof(valStr); //FIXME data type abstraction
	a3keyframeInit(keyframe_out, -1, value); //Duration will have to be initialized later

	return true;
}

a3i32 ec_keyframePool_loadFile(a3_KeyframePool* keyframePool_out, char const* filepath)
{
	//Open file
	a3_FileStream in = { 0 };
	a3ret good = a3fileStreamOpenRead(&in, filepath);
	assert(good == a3true);

	char buf[fileLineMaxLength] = { 0 }; //256 is hard-set as max length
	a3ui16 readIndex = 0;

	//Seek to data line
	while (__capture(&in, NULL, " \t\n\r", "@") || __skipComments(&in));

	//Capture cell count
	__capture(&in, buf, NULL, "\t\n\r");
	int cellCount = atoi(buf);

	//Seek to data line
	while (__capture(&in, NULL, " \t\n\r", "@") || __skipComments(&in));

	for (int i = 0; i < cellCount; ++i)
	{
		memset(buf, 0, fileLineMaxLength);
		fgetline(in.stream, buf);

		//TODO @rsc
		//__decodeKeyframe()
	}

	a3fileStreamClose(&in);
	return a3true;
}

// initialize keyframe
a3i32 a3keyframeInit(a3_Keyframe* keyframe_out, const a3real duration, const a3_Keyframe_data_t value_x)
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
	clipPool_out->clip = (a3_Clip*)malloc(count * sizeof(a3_Clip));
	clipPool_out->count = count;

	//Define Default values
	for (a3ui32 i = 0; i < count; i++)
	{
		a3clipInit(&clipPool_out->clip[i], "", NULL, 0, 0);
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
	strcpy_s(clip_out->name, a3keyframeAnimation_nameLenMax, clipName);
	clip_out->keyframePool = (a3_KeyframePool*)keyframePool;
	clip_out->firstKeyframe = firstKeyframeIndex;
	clip_out->lastKeyframe = finalKeyframeIndex;

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
a3_Keyframe* ec_clip_getKeyframe(a3_Clip const* clip, a3ui32 id)
{
	assert(0 <= id && id < clip->keyframeCount);
	return &(clip->keyframePool->keyframe[clip->firstKeyframe + id]);
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
