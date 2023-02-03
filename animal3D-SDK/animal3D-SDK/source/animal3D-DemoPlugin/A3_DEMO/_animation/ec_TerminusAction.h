#pragma once

#include "animal3D/a3/a3types_integer.h"

typedef enum ec_terminusActionFlags ec_terminusActionFlags;
enum ec_terminusActionFlags
{
	EC_TERMINUSACTION_REVERSE = (1 << 0),
	EC_TERMINUSACTION_FORWARD = (1 << 1),
	EC_TERMINUSACTION_PAUSE = (1 << 2),
	EC_TERMINUSACTION_SKIP = (1 << 3)
};


typedef struct ec_terminusAction ec_terminusAction;
struct ec_terminusAction
{
	ec_terminusActionFlags flags;
	a3ui32 targetClipID;
};

//Magic number to represent a clip that has not been assigned
#define NULL_CLIP_ID (~(a3ui32)0)
