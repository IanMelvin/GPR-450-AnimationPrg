/*
	Team Evistential Crisis
	Robert Christensen & Ian Melvin

	ec_Interpolation.h
	Generic vtables and functions that can be used to make polymorphic interpolation
*/

#ifndef EC_INTERPOLATION_H
#define EC_INTERPOLATION_H

#include <animal3D/a3/a3types_real.h>
#include <stddef.h>

typedef struct ec_DataVtable ec_DataVtable;

void vtable_setDefaults(ec_DataVtable* out); // Sets defaults for order 2+ (any that take a vtable ptr)
void setupVtables();
extern ec_DataVtable vtable_mat4;
extern ec_DataVtable vtable_SpatialPose;
extern ec_DataVtable vtable_vec3Additive;       // translation and euler angles
extern ec_DataVtable vtable_vec3Multiplicative; // scale
extern ec_DataVtable vtable_quat;               // quat, multiplicative

typedef void* (*fp_identity)(void* val_out);
typedef void* (*fp_invert  )(void* val_inout);
typedef void* (*fp_concat  )(void* val_out, const void* lhs, const void* rhs);
typedef void* (*fp_scale   )(void* val_inout, const a3real scale);

struct ec_DataVtable
{
	size_t size;

	//1st order essential functions
	fp_identity identity;
	fp_invert   invert;
	fp_concat   concat;
	fp_scale    scale;

	//Optional to override, vtable_setDefaults sets these to the functions below
	
	void* (*copy      )(void* dst, const void* src, const ec_DataVtable* funcs);
	void* (*lerp      )(void* val_out, const void* v0, const void* v1, const a3real param, const ec_DataVtable* funcs);
	void* (*nearest   )(void* val_out, const void* v0, const void* v1, const a3real param, const ec_DataVtable* funcs);
	void* (*cubic     )(void* val_out, const void* v1, const void* v2, const void* v3, const void* v4, const a3real param, const ec_DataVtable* funcs);
	void* (*deconcat  )(void* val_out, const void* lhs, const void* rhs, const ec_DataVtable* funcs);
	void* (*triangular)(void* val_out, const void* v0, const void* v1, const void* v2, const a3real param1, const a3real param2, const ec_DataVtable* funcs);
	void* (*biLerp    )(void* val_out, const void* v00, const void* v01, const void* v10, const void* v11, const a3real paramX0, const a3real paramX1, const a3real paramY, const ec_DataVtable* funcs);
	void* (*biNearest )(void* val_out, const void* v00, const void* v01, const void* v10, const void* v11, const a3real paramX0, const a3real paramX1, const a3real paramY, const ec_DataVtable* funcs);
	void* (*biCubic   )(void* val_out, const void* v1, const void* v2, const void* v3, const void* v4, const void* v5, const void* v6, const void* v7, const void* v8, const void* v9, const void* v10, const void* v11, const void* v12, const void* v13, const void* v14, const void* v15, const void* v16, const a3real param0, const a3real param1, const a3real param2, const a3real param3, const a3real param4, const ec_DataVtable* funcs);
};

//Defaults
void* defaultCopy      (void* dst, const void* src, const ec_DataVtable* funcs);
void* defaultLerp      (void* val_out, const void* v0, const void* v1, const a3real param, const ec_DataVtable* funcs);
void* defaultNearest   (void* val_out, const void* v0, const void* v1, const a3real param, const ec_DataVtable* funcs);
void* defaultCubic     (void* val_out, const void* v1, const void* v2, const void* v3, const void* v4, const a3real param, const ec_DataVtable* funcs);
void* defaultDeconcat  (void* val_out, const void* lhs, const void* rhs, const ec_DataVtable* funcs);
void* defaultTriangular(void* val_out, const void* v0, const void* v1, const void* v2, const a3real param1, const a3real param2, const ec_DataVtable* funcs);
void* defaultBiLerp    (void* val_out, const void* v00, const void* v01, const void* v10, const void* v11, const a3real paramX0, const a3real paramX1, const a3real paramY, const ec_DataVtable* funcs);
void* defaultBiNearest (void* val_out, const void* v00, const void* v01, const void* v10, const void* v11, const a3real paramX0, const a3real paramX1, const a3real paramY, const ec_DataVtable* funcs);
void* defaultBiCubic   (void* val_out, const void* v1, const void* v2, const void* v3, const void* v4, const void* v5, const void* v6, const void* v7, const void* v8, const void* v9, const void* v10, const void* v11, const void* v12, const void* v13, const void* v14, const void* v15, const void* v16, const a3real param0, const a3real param1, const a3real param2, const a3real param3, const a3real param4, const ec_DataVtable* funcs);

#endif