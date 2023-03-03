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

void vtable_setDefaults(ec_DataVtable* out); // Sets defaults for order 2+
void setupVtables();
extern ec_DataVtable vtable_mat4;
extern ec_DataVtable vtable_SpatialPose;
extern ec_DataVtable vtable_translation; // vec3, additive
extern ec_DataVtable vtable_quatRotation; // quat, multiplicative
extern ec_DataVtable vtable_eulerRotation; // vec3, additive
extern ec_DataVtable vtable_scale; // vec3, multiplicative

struct ec_DataVtable
{
	size_t size;

	//1st order essential functions
	void* (*identity)(void* val_out);
	void* (*copy)(void* dst, const void* src);
	void* (*invert)(void* val_inout);
	void* (*concat)(void* val_out, const void* lhs, const void* rhs);
	void* (*scale)(void* val_inout, const a3real scale);

	//Optional to override, vtable_setDefaults sets these to the functions below
	
	void* (*lerp      )(void* val_out, const void* v0, const void* v1, const a3real param, const ec_DataVtable* funcs);
	void* (*nearest   )(void* val_out, const void* v0, const void* v1, const a3real param, const ec_DataVtable* funcs);
	void* (*cubic     )(void* val_out, const void* vals, const a3real param, const ec_DataVtable* funcs); //Assume vals is void[4]
	void* (*deconcat  )(void* val_out, const void* lhs, const void* rhs, const ec_DataVtable* funcs);
	void* (*triangular)(void* val_out, const void* v0, const void* v1, const void* v2, const a3real param1, const a3real param2, const ec_DataVtable* funcs);
	void* (*biLerp    )(void* val_out, const void* v00, const void* v01, const void* v10, const void* v11, const a3real paramX0, const a3real paramX1, const a3real paramY, const ec_DataVtable* funcs);
	void* (*biNearest )(void* val_out, const void* v00, const void* v01, const void* v10, const void* v11, const a3real paramX0, const a3real paramX1, const a3real paramY, const ec_DataVtable* funcs);
	void* (*biCubic   )(void* val_out, const void* vals[4], const a3real param0, const a3real param1, const ec_DataVtable* funcs); //Assume vals is 4x4
};

//Defaults
void* defaultLerp      (void* val_out, const void* v0, const void* v1, const a3real param, const ec_DataVtable* funcs);
void* defaultNearest   (void* val_out, const void* v0, const void* v1, const a3real param, const ec_DataVtable* funcs);
void* defaultCubic     (void* val_out, const void* vals, const a3real param, const ec_DataVtable* funcs); //Assume vals is void[4]
void* defaultDeconcat  (void* val_out, const void* lhs, const void* rhs, const ec_DataVtable* funcs);
void* defaultTriangular(void* val_out, const void* v0, const void* v1, const void* v2, const a3real param1, const a3real param2, const ec_DataVtable* funcs);
void* defaultBiLerp    (void* val_out, const void* v00, const void* v01, const void* v10, const void* v11, const a3real paramX0, const a3real paramX1, const a3real paramY, const ec_DataVtable* funcs);
void* defaultBiNearest (void* val_out, const void* v00, const void* v01, const void* v10, const void* v11, const a3real paramX0, const a3real paramX1, const a3real paramY, const ec_DataVtable* funcs);
void* defaultBiCubic   (void* val_out, const void* vals[4], const a3real param0, const a3real param1, const ec_DataVtable* funcs); //Assume vals is 4x4

/*
	Not doing:
	 - Construct
*/

#endif