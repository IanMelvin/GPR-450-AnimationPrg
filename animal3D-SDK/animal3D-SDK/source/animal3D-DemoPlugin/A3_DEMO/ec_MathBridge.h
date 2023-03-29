/*
	Team Existential Crisis
	Robert Christensen & Ian Melvin
	
	ec_MathBridge.h
	Fills in the gaps in animal3D, particularly with interpolation, and standardizes their form
*/

#ifndef EC_MATHBRIDGE_H
#define EC_MATHBRIDGE_H

#include <animal3D/a3/a3types_real.h>
#include <animal3D-A3DM/a3math/a3vector.h>

//Helper for loglerp
a3real3r a3real3PowS(a3real3p v_inout, const a3real power);

//Just wraps the lerp function to make it the standard form
a3real* a3realLerpWrapper(a3real* out, const a3real* val0, const a3real* val1, a3real param);

//Logarithmic lerp
a3real3r a3vec3LogLerp(a3real3p v_out, const a3real3p v0, const a3real3p v1, const a3real param);

#endif