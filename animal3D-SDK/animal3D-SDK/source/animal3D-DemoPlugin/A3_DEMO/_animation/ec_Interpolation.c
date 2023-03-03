#include "ec_Interpolation.h"

#include <stdlib.h>
#include <string.h>

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

void* defaultDeconcat(void* val_out, const void* lhs, const void* rhs, const ec_DataVtable* funcs)
{
	funcs->copy(val_out, rhs, funcs);     //val_out = rhs
	funcs->invert(val_out);               //val_out = rhs^-1
	funcs->concat(val_out, lhs, val_out); //val_out = lhs * rhs^-1
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

#pragma endregion

#pragma region Vtable implementations

#include <animal3D-A3DM/animal3D-A3DM.h>
#include "a3_SpatialPose.h"

ec_DataVtable vtable_mat4;
ec_DataVtable vtable_SpatialPose;
ec_DataVtable vtable_translation; // vec3, additive
ec_DataVtable vtable_quatRotation; // quat, multiplicative
ec_DataVtable vtable_eulerRotation; // vec3, additive
ec_DataVtable vtable_scale; // vec3, multiplicative

void setupVtables()
{
#ifdef _DEBUG
	//Safety: Zero all to detect unset values
	memset(&vtable_mat4         , 0, sizeof(ec_DataVtable));
	memset(&vtable_SpatialPose  , 0, sizeof(ec_DataVtable));
	memset(&vtable_translation  , 0, sizeof(ec_DataVtable));
	memset(&vtable_quatRotation , 0, sizeof(ec_DataVtable));
	memset(&vtable_eulerRotation, 0, sizeof(ec_DataVtable));
	memset(&vtable_scale        , 0, sizeof(ec_DataVtable));
#endif

	//Set all defaults, size
	vtable_setDefaults(&vtable_mat4         );
	vtable_setDefaults(&vtable_SpatialPose  );
	vtable_setDefaults(&vtable_translation  );
	vtable_setDefaults(&vtable_quatRotation );
	vtable_setDefaults(&vtable_eulerRotation);
	vtable_setDefaults(&vtable_scale        );
	vtable_mat4         .size = sizeof(a3mat4);
	vtable_SpatialPose  .size = sizeof(a3_SpatialPose);
	vtable_translation  .size = sizeof(a3vec3);
	vtable_quatRotation .size = sizeof(a3quat);
	vtable_eulerRotation.size = sizeof(a3vec3);
	vtable_scale        .size = sizeof(a3vec3);

	//mat4
	vtable_mat4.identity = (fp_identity) a3real4x4SetIdentity;
	vtable_mat4.invert	 = (fp_invert  ) a3real4x4Invert;
	vtable_mat4.concat	 = (fp_concat  ) a3real4x4Product;
	vtable_mat4.scale	 = (fp_scale   ) a3real4x4MulS;

	//spatial pose
	vtable_SpatialPose.identity = (fp_identity) a3spatialPoseReset;
	//vtable_SpatialPose.invert   = (fp_invert  ) ;
	vtable_SpatialPose.concat   = (fp_concat  ) a3spatialPoseConcat;
	//vtable_SpatialPose.scale    = (fp_scale   ) ;
	
	//translation
	//vtable_translation.identity = (fp_identity) ;
	vtable_translation.invert	= (fp_invert  ) a3real3Negate;
	vtable_translation.concat	= (fp_concat  ) a3real3Sum;
	vtable_translation.scale	= (fp_scale   ) a3real3MulS;

	//quat rotation
	vtable_quatRotation.identity = (fp_identity) a3quatSetIdentity;
	vtable_quatRotation.invert	 = (fp_invert  ) a3quatInvert;
	//vtable_quatRotation.concat	 = (fp_concat  ) ;
	//vtable_quatRotation.scale	 = (fp_scale   ) ;

	//euler rotation
	//vtable_eulerRotation.identity = (fp_identity) ;
	vtable_eulerRotation.invert	  = (fp_invert  ) a3real3Negate;
	vtable_eulerRotation.concat	  = (fp_concat  ) a3real3Sum;
	vtable_eulerRotation.scale	  = (fp_scale   ) a3real3MulS;

	//scale
	//vtable_scale.identity = (fp_identity) ;
	//vtable_scale.invert	  = (fp_invert  ) ;
	vtable_scale.concat	  = (fp_concat  ) a3real3MulComp;
	//vtable_scale.scale	  = (fp_scale   ) ;
}

#pragma endregion