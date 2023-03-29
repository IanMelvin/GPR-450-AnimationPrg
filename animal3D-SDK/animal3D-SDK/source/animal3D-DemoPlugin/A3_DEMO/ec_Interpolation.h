/*
	Team Existential Crisis
	Robert Christensen & Ian Melvin

	ec_Interpolation.h
	Provides a common interface to interpolate values of arbitrary format, such as those provided by animal3D.
*/

#ifndef EC_INTERPOLATION_H
#define EC_INTERPOLATION_H

#include "_animation/ec_PolymorphicData.h"

typedef enum ec_InterpolationMode ec_InterpolationMode;

// how to blend between data
// when adding new values here, add to ec_InterpolationFuncFamily
enum ec_InterpolationMode
{
	//Special values - non-blending
	EC_INTERPOLATE_CONSTANT = -0b01,
	EC_INTERPOLATE_NEAREST  = -0b11,

	//Normal blending
	EC_INTERPOLATE_LINEAR = 0,
	EC_INTERPOLATE_CATMULL_ROM,
	EC_INTERPOLATE_CUBIC_HERMITE,

	//Special values
	EC_INTERPOLATE_MODE_COUNT, //How many *blending* interpolation modes are there? (not counting constant or nearest)
	EC_INTERPOLATE_DEFAULT = EC_INTERPOLATE_LINEAR //What is the default interpolation mode?
};

// standardized interpolation function
void* ec_interpolate(void* out, const void* handle0, const void* val0, const void* val1, const void* handle1, a3real param, const ec_DataVtable* funcs, ec_InterpolationMode mode);

#endif