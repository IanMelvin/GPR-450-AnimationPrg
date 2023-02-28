/*
	Team Existential Crisis
	Robert Christensen & Ian Melvin

	ec_Interpolation.h
	Generic functions that can be used to make polymorphic interpolation
*/

#ifndef EC_INTERPOLATION_H
#define EC_INTERPOLATION_H

#include <animal3D/a3/a3types_real.h>
#include <stddef.h>

typedef struct ec_DataVtable ec_DataVtable;

struct ec_DataVtable
{
	void* (*identity)(void* val_out);
	void* (*copy)(void* dst, const void* src);
	void* (*invert)(void* val_inout);
	void* (*concat)(void* val_out, const void* lhs, const void* rhs);
	void* (*scale)(void* val_inout, const a3real scale);
	size_t size;
};

void* deconcat(void* val_out, const void* lhs, const void* rhs, const ec_DataVtable* funcs);

void* nearest   (void* val_out, const void* x0, const void* x1, const a3real param, const ec_DataVtable* funcs);
void* lerp      (void* val_out, const void* x0, const void* x1, const a3real param, const ec_DataVtable* funcs);
void* cubic     (void* val_out, const void* xneg1, const void* x0, const void* x1, const void* x2, const a3real param, const ec_DataVtable* funcs);
void* triangular(void* val_out, const void* x0, const void* x1, const void* x2, const a3real param0, const a3real param1, const ec_DataVtable* funcs);

/*
	Still need:
	 - Bi-nearest
	 - Bi-linear
	 - Bi-cubic

	** For bi-interpolation, allow the first two interpolations to have separate parameters **

	Not doing:
	 - Construct
*/

extern ec_DataVtable vtable_mat4;
extern ec_DataVtable vtable_SpatialPose;
extern ec_DataVtable vtable_translation;
extern ec_DataVtable vtable_quatRotation;
extern ec_DataVtable vtable_eulerRotation;
extern ec_DataVtable vtable_scale;

#endif