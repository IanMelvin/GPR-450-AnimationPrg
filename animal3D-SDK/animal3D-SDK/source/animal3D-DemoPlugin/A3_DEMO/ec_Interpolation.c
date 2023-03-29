#include "ec_Interpolation.h"

#include <assert.h>
#include <string.h>

void* ec_interpolate(void* out, const void* handle0, const void* val0, const void* val1, const void* handle1, a3real param, const ec_DataVtable* funcs, ec_InterpolationMode mode)
{
    switch (mode)
	{
	case EC_INTERPOLATE_CONSTANT     : funcs->copy   (out,          val0,                       funcs); break;
	case EC_INTERPOLATE_NEAREST      : funcs->nearest(out,          val0, val1,          param, funcs); break;
	case EC_INTERPOLATE_LINEAR       : funcs->lerp   (out,          val0, val1,          param, funcs); break;
	//case EC_INTERPOLATE_CATMULL_ROM  : break;
	case EC_INTERPOLATE_CUBIC_HERMITE: funcs->cubic  (out, handle0, val0, val1, handle1, param, funcs); break;

	//Catch unhandled options
	default: assert(false); break;
	}

	return out;
}
