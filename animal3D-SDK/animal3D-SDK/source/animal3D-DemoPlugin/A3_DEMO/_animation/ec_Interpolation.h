/*
	Team Evistential Crisis
	Robert Christensen & Ian Melvin

	ec_Interpolation.h
	Generic vtables and functions that can be used to make polymorphic interpolation
*/

#ifndef EC_INTERPOLATION_H
#define EC_INTERPOLATION_H

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
typedef void* (*fp_descale )(void* val_inout, const a3real scale);
typedef void* (*fp_lerp    )(void* val_out, const void* val_0, const void* val_1, const a3real u, const ec_DataVtable* funcs);


struct ec_DataVtable
{
	size_t size;

	//1st order essential functions
	fp_identity identity;
	fp_invert   invert;
	fp_concat   concat;
	fp_scale    scale;
	fp_descale	descale;

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
	void* (*smoothStep)(void* val_out, const void* v1, a3real param);
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

a3real smoothStep(a3real param, a3real sharpness);

//SpacialPoseSpecific
a3_SpatialPose* spacialPoseConvert		 (a3_SpatialPose* spacialPose_Out);
a3_SpatialPose* spacialPoseRevert		 (a3_SpatialPose* spacialPose_Out);

//HierarchyPose
a3_HierarchyPose* hierarchyPoseScaleUniform(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* in, const a3real scale, a3ui32 numNodes);
a3_HierarchyPose* hierarchyPoseScalePerNode(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* in, const a3real* scalesPerNode, a3ui32 numNodes);
a3_HierarchyPose* hierarchyPoseCopy      (a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos0, a3ui32 numNodes);
a3_HierarchyPose* hierarchyPoseLerp      (a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos0, const a3_HierarchyPose* pos1, a3ui32 numNodes, const a3real param);
a3_HierarchyPose* hierarchyPoseNearest   (a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos0, const a3_HierarchyPose* pos1, a3ui32 numNodes, const a3real param);
a3_HierarchyPose* hierarchyPoseCubic     (a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos1, const a3_HierarchyPose* pos2, const a3_HierarchyPose* pos3, const a3_HierarchyPose* pos4, a3ui32 numNodes, const a3real param);
a3_HierarchyPose* hierarchyPoseDeconcat  (a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* lhs, const a3_HierarchyPose* rhs, a3ui32 numNodes);
a3_HierarchyPose* hierarchyPoseTriangular(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos0, const a3_HierarchyPose* pos1, const a3_HierarchyPose* pos2, const a3real param1, const a3real param2, a3ui32 numNodes);
a3_HierarchyPose* hierarchyPoseBiLerp    (a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos00, const a3_HierarchyPose* pos01, const a3_HierarchyPose* pos10, const a3_HierarchyPose* pos11, a3ui32 numNodes, const a3real paramX0, const a3real paramX1, const a3real paramY);
a3_HierarchyPose* hierarchyPoseBiNearest (a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos00, const a3_HierarchyPose* pos01, const a3_HierarchyPose* pos10, const a3_HierarchyPose* pos11, a3ui32 numNodes, const a3real paramX0, const a3real paramX1, const a3real paramY);
a3_HierarchyPose* hierarchyPoseBiCubic   (a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos1, const a3_HierarchyPose* pos2, const a3_HierarchyPose* pos3, const a3_HierarchyPose* pos4, const a3_HierarchyPose* pos5, const a3_HierarchyPose* pos6, const a3_HierarchyPose* pos7, const a3_HierarchyPose* pos8, const a3_HierarchyPose* pos9, const a3_HierarchyPose* pos10, const a3_HierarchyPose* pos11, const a3_HierarchyPose* pos12, const a3_HierarchyPose* pos13, const a3_HierarchyPose* pos14, const a3_HierarchyPose* pos15, const a3_HierarchyPose* pos16, const a3real param0, const a3real param1, const a3real param2, const a3real param3, const a3real param4, const a3ui32 numNodes);
a3_HierarchyPose* hierarchyPoseSmoothStep(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pose1, a3real param, const a3ui32 numNodes);
a3_HierarchyPose* hierarchyConvert		 (a3_HierarchyPose* hierarchyPose_Out, const a3ui32 numNodes);
a3_HierarchyPose* hierarchyRevert		 (a3_HierarchyPose* hierarchyPose_Out, const a3ui32 numNodes);


#endif