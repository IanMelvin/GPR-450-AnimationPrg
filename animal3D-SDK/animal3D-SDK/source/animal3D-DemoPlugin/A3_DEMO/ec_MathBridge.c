#include "ec_MathBridge.h"

#include <math.h>
#include <animal3D-A3DM/a3math/a3interpolation.h>

//Helper for loglerp
a3real3r a3real3PowS(a3real3p v_inout, const a3real power)
{
	a3real invLen;
	a3real3NormalizeGetInvLength(v_inout, &invLen);
	a3real len = 1 / invLen;
	a3real3MulS(v_inout, (a3real)pow(len, power));
	return v_inout;
}

//Just wraps the lerp function to make it the standard form
a3real* a3realLerpWrapper(a3real* out, const a3real* val0, const a3real* val1, a3real param)
{
	*out = a3lerpFunc(*val0, *val1, param); return out;
}

a3real3r a3vec3LogLerp(a3real3p v_out, const a3real3p v0, const a3real3p v1, const a3real param)
{
	a3real3Set(v_out, v1[0], v1[1], v1[2]); // b
	a3real3DivComp(v_out, v0);              // b*a^-1
	a3real3PowS(v_out, param);              // (b*a^-1)^x
	a3real3MulComp(v_out, v0);              // a * (b*a^-1)^x
	return v_out;
}