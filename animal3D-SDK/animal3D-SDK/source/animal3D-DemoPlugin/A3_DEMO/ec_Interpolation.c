#include "ec_Interpolation.h"

#include <assert.h>
#include <string.h>

void* ec_interpolate(void* out, const void* val0, const void* val1, a3real param, const ec_InterpolationFuncFamily* funcs, ec_InterpolationMode mode)
{
    switch (mode)
	{
	//Special cases for non-blending modes
	case EC_INTERPOLATE_CONSTANT:
		//*out = *val0;
		memcpy(out, val0, funcs->valSize);
		return out;

	case EC_INTERPOLATE_NEAREST:
		//*out = param < 0.5f ? val0 : val1;
		memcpy(out, param < 0.5f ? val0 : val1, funcs->valSize);
		return out;

	//Normal blending modes
	default:
		assert(funcs->byMode[mode]);
		funcs->byMode[mode](out, val0, val1, param);
		return out;
	}

	/*
	switch (mode)
	{
	case EC_INTERPOLATE_CONSTANT: *out = val0;
	case EC_INTERPOLATE_NEAREST : *out = param < 0.5f ? val0 : val1;
	case EC_INTERPOLATE_LINEAR  : *out = a3lerp(val0, val1, param);

	default: assert(false); return 0;
	}
	*/
}
