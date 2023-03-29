/*
	Team Evistential Crisis
	Robert Christensen & Ian Melvin

	ec_PolymorphicData.h
	Generic vtables and functions that can be used to operate on data without knowing its exact type
*/

#ifndef EC_POLYMORPHIC_DATA_H
#define EC_POLYMORPHIC_DATA_H

#include <animal3D/a3/a3types_real.h>
#include "a3_HierarchyState.h"
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
	
	void* (*descale   )(void* val_inout, const a3real scale, const ec_DataVtable* funcs);
	void* (*copy      )(void* dst, const void* src, const ec_DataVtable* funcs);
	void* (*lerp      )(void* val_out, const void* v0, const void* v1, const a3real param, const ec_DataVtable* funcs);
	void* (*nearest   )(void* val_out, const void* v0, const void* v1, const a3real param, const ec_DataVtable* funcs);
	void* (*cubic     )(void* val_out, const void* v1, const void* v2, const void* v3, const void* v4, const a3real param, const ec_DataVtable* funcs);
	void* (*deconcat  )(void* val_out, const void* lhs, const void* rhs, const ec_DataVtable* funcs);
	void* (*triangular)(void* val_out, const void* v0, const void* v1, const void* v2, const a3real param1, const a3real param2, const ec_DataVtable* funcs);
	void* (*biLerp    )(void* val_out, const void* v00, const void* v01, const void* v10, const void* v11, const a3real paramX0, const a3real paramX1, const a3real paramY, const ec_DataVtable* funcs);
	void* (*biNearest )(void* val_out, const void* v00, const void* v01, const void* v10, const void* v11, const a3real paramX0, const a3real paramX1, const a3real paramY, const ec_DataVtable* funcs);
	void* (*biCubic   )(void* val_out, const void* v1, const void* v2, const void* v3, const void* v4, const void* v5, const void* v6, const void* v7, const void* v8, const void* v9, const void* v10, const void* v11, const void* v12, const void* v13, const void* v14, const void* v15, const void* v16, const a3real param0, const a3real param1, const a3real param2, const a3real param3, const a3real param4, const ec_DataVtable* funcs);
	void* (*smoothStep)();
	void* (*convert)   ();
	void* (*revert)	   ();
	void* (*FK)		   ();
	void* (*IK)		   ();
};

//Defaults
void* defaultDescale   (void* val_inout, const a3real control, const ec_DataVtable* funcs);
void* defaultCopy      (void* dst, const void* src, const ec_DataVtable* funcs);
void* defaultLerp      (void* val_out, const void* v0, const void* v1, const a3real param, const ec_DataVtable* funcs);
void* defaultNearest   (void* val_out, const void* v0, const void* v1, const a3real param, const ec_DataVtable* funcs);
void* defaultCubic     (void* val_out, const void* v1, const void* v2, const void* v3, const void* v4, const a3real param, const ec_DataVtable* funcs);
void* defaultDeconcat  (void* val_out, const void* lhs, const void* rhs, const ec_DataVtable* funcs);
void* defaultTriangular(void* val_out, const void* v0, const void* v1, const void* v2, const a3real param1, const a3real param2, const ec_DataVtable* funcs);
void* defaultBiLerp    (void* val_out, const void* v00, const void* v01, const void* v10, const void* v11, const a3real paramX0, const a3real paramX1, const a3real paramY, const ec_DataVtable* funcs);
void* defaultBiNearest (void* val_out, const void* v00, const void* v01, const void* v10, const void* v11, const a3real paramX0, const a3real paramX1, const a3real paramY, const ec_DataVtable* funcs);
void* defaultBiCubic   (void* val_out, const void* v1, const void* v2, const void* v3, const void* v4, const void* v5, const void* v6, const void* v7, const void* v8, const void* v9, const void* v10, const void* v11, const void* v12, const void* v13, const void* v14, const void* v15, const void* v16, const a3real param0, const a3real param1, const a3real param2, const a3real param3, const a3real param4, const ec_DataVtable* funcs);

//HierarchyPose/Bulk Variants
void* bulkCopy      (void* val_out, const void* pos0, a3ui32 count, const ec_DataVtable* funcs);
void* bulkLerp      (void* val_out, const void* pos0, const void* pos1, a3ui32 count, const a3real param, const ec_DataVtable* funcs);
void* bulkNearest   (void* val_out, const void* pos0, const void* pos1, a3ui32 count, const a3real param, const ec_DataVtable* funcs);
void* bulkCubic     (void* val_out, const void* pos1, const void* pos2, const void* pos3, const void* pos4, a3ui32 count, const a3real param, const ec_DataVtable* funcs);
void* bulkDeconcat  (void* val_out, const void* lhs, const void* rhs, a3ui32 count, const ec_DataVtable* funcs);
void* bulkTriangular(void* val_out, const void* pos0, const void* pos1, const void* pos2, const a3real param1, const a3real param2, a3ui32 count, const ec_DataVtable* funcs);
void* bulkBiLerp    (void* val_out, const void* pos00, const void* pos01, const void* pos10, const void* pos11, a3ui32 count, const a3real paramX0, const a3real paramX1, const a3real paramY, const ec_DataVtable* funcs);
void* bulkBiNearest (void* val_out, const void* pos00, const void* pos01, const void* pos10, const void* pos11, a3ui32 count, const a3real paramX0, const a3real paramX1, const a3real paramY, const ec_DataVtable* funcs);
void* bulkBiCubic   (void* val_out, const void* pos1, const void* pos2, const void* pos3, const void* pos4, const void* pos5, const void* pos6, const void* pos7, const void* pos8, const void* pos9, const void* pos10, const void* pos11, const void* pos12, const void* pos13, const void* pos14, const void* pos15, const void* pos16, const a3real param0, const a3real param1, const a3real param2, const a3real param3, const a3real param4, const a3ui32 count, const ec_DataVtable* funcs);

#endif