#include "ec_Interpolation.h"

#include <stdlib.h>
#include <string.h>
#include <Math.h>

#include <animal3D-A3DM/animal3D-A3DM.h>

#include "a3_SpatialPose.h"

#pragma region Helpers and defaults

void vtable_setDefaults(ec_DataVtable* out)
{
	out->copy       = defaultCopy      ;
	out->lerp       = defaultLerp      ;
	out->nearest    = defaultNearest   ;
	out->cubic      = defaultCubic     ;
	out->deconcat   = defaultDeconcat  ;
	out->triangular = defaultTriangular;
	out->biLerp     = defaultBiLerp    ;
	out->biNearest  = defaultBiNearest ;
	out->biCubic    = defaultBiCubic   ;
}

void* defaultCopy(void* dst, const void* src, const ec_DataVtable* funcs)
{
	memcpy(dst, src, funcs->size);
	return dst;
}

void* defaultLerp(void* val_out, const void* v0, const void* v1, const a3real param, const ec_DataVtable* funcs)
{
	funcs->copy(val_out, v0, funcs);     //val_out = v0
	funcs->invert(val_out);              //val_out = v0^-1
	funcs->concat(val_out, v1, val_out); //val_out = v1 * v0^-1
	funcs->scale(val_out, param);        //val_out = (v1 * v0^-1) * u
	funcs->concat(val_out, val_out, v0); //val_out = ((v1 * v0^-1) * u) * v0
	return val_out;
}

void* defaultNearest(void* val_out, const void* v0, const void* v1, const a3real param, const ec_DataVtable* funcs)
{
	return funcs->copy(val_out, param>0.5f ? v1 : v0, funcs);
}

void* defaultCubic(void* val_out, const void* v1, const void* v2, const void* v3, const void* v4, const a3real param, const ec_DataVtable* funcs)
{
	if (val_out && v1 && v2 && v3 && v4)
	{
		funcs->copy(val_out, v1, funcs);

		void* val2 = malloc(sizeof(funcs->size));
		funcs->copy(val2, v2, funcs);

		void* val3 = malloc(sizeof(funcs->size));
		funcs->copy(val3, v3, funcs);

		void* val4 = malloc(sizeof(funcs->size));
		funcs->copy(val4, v4, funcs);

		funcs->scale(val_out, (a3real)(-param + 2 * pow(param, 2) - pow(param, 3)));
		funcs->scale(val2, (a3real)(2 - 5 * pow(param, 2) + 3 * pow(param, 3)));
		funcs->scale(val3, (a3real)(param + 4 * pow(param, 2) - 3 * pow(param, 3)));
		funcs->scale(val4, (a3real)(-1 * pow(param, 2) + pow(param, 3)));

		funcs->concat(val_out, val_out, val2);
		funcs->concat(val_out, val_out, val3);
		funcs->concat(val_out, val_out, val4);

		funcs->scale(val_out, 0.5f);
	}
	return val_out;
}

void* defaultDeconcat(void* val_out, const void* lhs, const void* rhs, const ec_DataVtable* funcs)
{
	funcs->copy(val_out, rhs, funcs);     //val_out = rhs
	funcs->invert(val_out);               //val_out = rhs^-1
	funcs->concat(val_out, lhs, val_out); //val_out = lhs * rhs^-1
	return val_out;
}

void* defaultTriangular(void* val_out, const void* v0, const void* v1, const void* v2, const a3real param1, const a3real param2, const ec_DataVtable* funcs)
{
	if (val_out && v0 && v1 && v2)
	{
		a3real param3 = 1 - param1 - param2;

		funcs->copy(val_out, v0, funcs);
		funcs->scale(val_out, param3);

		void* val1 = malloc(sizeof(v0));
		funcs->copy(val1, v1, funcs);
		funcs->scale(val1, param1);

		void* val2 = malloc(sizeof(v0));
		funcs->copy(val2, v2, funcs);
		funcs->scale(val2, param2);

		funcs->concat(val_out, val_out, val1);
		funcs->concat(val_out, val_out, val2);
	}

	return val_out;
}

void* defaultBiLerp(void* val_out, const void* v00, const void* v01, const void* v10, const void* v11, const a3real paramX0, const a3real paramX1, const a3real paramY, const ec_DataVtable* funcs)
{
	void* vx0 = calloc(2, funcs->size);
	void* vx1 = ((char*)vx0) + funcs->size;
	funcs->lerp(vx0, v00, v01, paramX0, funcs);
	funcs->lerp(vx1, v10, v11, paramX1, funcs);
	funcs->lerp(val_out, vx0, vx1, paramY, funcs);
	free(vx0);
	return val_out;
}

void* defaultBiNearest(void* val_out, const void* v00, const void* v01, const void* v10, const void* v11, const a3real paramX0, const a3real paramX1, const a3real paramY, const ec_DataVtable* funcs)
{
	const void* vx0 = paramX0>0.5f ? v00 : v01;
	const void* vx1 = paramX1>0.5f ? v10 : v11;
	return funcs->nearest(val_out, vx0, vx1, paramY, funcs);
}

void* defaultBiCubic(void* val_out, const void* v1, const void* v2, const void* v3, const void* v4, const void* v5, const void* v6, const void* v7, const void* v8, const void* v9, const void* v10, const void* v11, const void* v12, const void* v13, const void* v14, const void* v15, const void* v16, const a3real param0, const a3real param1, const a3real param2, const a3real param3, const a3real param4, const ec_DataVtable* funcs)
{
	if (val_out && v1 && v2 && v3 && v4 && v5 && v6 && v7 && v8 && v9 && v10 && v11 && v12 && v13 && v14 && v15 && v16)
	{
		void* val1 = malloc(sizeof(funcs->size));
		void* val2 = malloc(sizeof(funcs->size));
		void* val3 = malloc(sizeof(funcs->size));
		void* val4 = malloc(sizeof(funcs->size));

		funcs->cubic(val_out,
			funcs->cubic(val1, v1, v2, v3, v4, param0, funcs),
			funcs->cubic(val2, v5, v6, v7, v8, param1, funcs),
			funcs->cubic(val3, v9, v10, v11, v12, param2, funcs),
			funcs->cubic(val4, v13, v14, v15, v16, param3, funcs),
			param4, funcs);
	}

	return val_out;
}

#pragma endregion

#pragma region "Missing" functions

a3real3* a3real3SetZero(a3real3* val_out)
{
	a3real3Set(*val_out, 0, 0, 0);
	return val_out;
}

a3real3* a3real3SetOne (a3real3* val_out)
{
	a3real3Set(*val_out, 1, 1, 1);
	return val_out;
}

a3real3* a3real3InvertLength(a3real3* val_inout)
{
	a3real3MulS(*val_inout, a3real3LengthSquaredInverse(*val_inout));
	return val_inout;
}

a3real3* a3real3PowS(a3real3* val_inout, const a3real power)
{
	a3real invLen;
	a3real3NormalizeGetInvLength(*val_inout, &invLen);
	a3real3MulS(*val_inout, (a3real)pow(1/invLen, power));
	return val_inout;
}

a3quat* a3quatMulS(a3quat* val_inout, const a3real scale)
{
	a3quat tmp = { 0 }; //Same var used for read/write may have unintended consequences
	a3quatSlerpIdentityQ0(tmp.q, val_inout->q, scale);
	a3real4SetReal4(val_inout->q, tmp.q);
	return val_inout;
}

a3_SpatialPose* a3spatialPoseMulS(a3_SpatialPose* val_inout, const a3real scale)
{
	vtable_vec3Additive      .scale(&val_inout->translation, scale);
	vtable_vec3Additive      .scale(&val_inout->angles     , scale);
	vtable_quat              .scale(&val_inout->orientation, scale);
	vtable_vec3Multiplicative.scale(&val_inout->scale      , scale);
	vtable_mat4              .scale(&val_inout->transform  , scale);
	return val_inout;
}

#pragma endregion

#pragma region Vtable implementations

//NOTE: All vec3 functions can also take a vec4, but ignore W
ec_DataVtable vtable_mat4;
ec_DataVtable vtable_SpatialPose;
ec_DataVtable vtable_vec3Additive;       // translation and euler angles
ec_DataVtable vtable_vec3Multiplicative; // scale
ec_DataVtable vtable_quat;               // quat, multiplicative

void setupVtables()
{
#ifdef _DEBUG
	//Safety: Zero all to detect unset values
	memset(&vtable_mat4              , 0, sizeof(ec_DataVtable));
	memset(&vtable_SpatialPose       , 0, sizeof(ec_DataVtable));
	memset(&vtable_vec3Additive      , 0, sizeof(ec_DataVtable));
	memset(&vtable_vec3Multiplicative, 0, sizeof(ec_DataVtable));
	memset(&vtable_quat              , 0, sizeof(ec_DataVtable));
#endif

	//Set all defaults, size
	vtable_setDefaults(&vtable_mat4              );
	vtable_setDefaults(&vtable_SpatialPose       );
	vtable_setDefaults(&vtable_vec3Additive      );
	vtable_setDefaults(&vtable_vec3Multiplicative);
	vtable_setDefaults(&vtable_quat              );
	vtable_mat4              .size = sizeof(a3mat4);
	vtable_SpatialPose       .size = sizeof(a3_SpatialPose);
	vtable_vec3Additive      .size = sizeof(a3vec3);
	vtable_vec3Multiplicative.size = sizeof(a3vec3);
	vtable_quat              .size = sizeof(a3quat);

	//mat4
	vtable_mat4.identity = (fp_identity) a3real4x4SetIdentity;
	vtable_mat4.invert	 = (fp_invert  ) a3real4x4Invert;
	vtable_mat4.concat	 = (fp_concat  ) a3real4x4Product;
	vtable_mat4.scale	 = (fp_scale   ) a3real4x4MulS;

	//spatial pose
	vtable_SpatialPose.identity = (fp_identity) a3spatialPoseReset;
	vtable_SpatialPose.invert   = (fp_invert  ) a3spatialPoseInvert;
	vtable_SpatialPose.concat   = (fp_concat  ) a3spatialPoseConcat;
	vtable_SpatialPose.scale    = (fp_scale   ) a3spatialPoseMulS;
	
	//translation and euler angles
	vtable_vec3Additive.identity = (fp_identity) a3real3SetZero;
	vtable_vec3Additive.invert	= (fp_invert  ) a3real3Negate;
	vtable_vec3Additive.concat	= (fp_concat  ) a3real3Sum;
	vtable_vec3Additive.scale	= (fp_scale   ) a3real3MulS;

	//rotation
	vtable_quat.identity = (fp_identity) a3quatSetIdentity;
	vtable_quat.invert	 = (fp_invert  ) a3quatInvert;
	vtable_quat.concat	 = (fp_concat  ) a3quatProduct;
	vtable_quat.scale	 = (fp_scale   ) a3quatMulS;

	//scale
	vtable_vec3Multiplicative.identity = (fp_identity) a3real3SetOne;
	vtable_vec3Multiplicative.invert	  = (fp_invert  ) a3real3InvertLength;
	vtable_vec3Multiplicative.concat	  = (fp_concat  ) a3real3MulComp;
	vtable_vec3Multiplicative.scale	  = (fp_scale   ) a3real3PowS;
}

#pragma endregion