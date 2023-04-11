#include "A3_DEMO/_animation/ec_Interpolation.h"

#include <stdlib.h>
#include <string.h>
#include <Math.h>
#include <assert.h>

#include <animal3D-A3DM/animal3D-A3DM.h>

#include "A3_DEMO/_animation/a3_SpatialPose.h"

#pragma region Helpers and defaults

void* allocTemp(const ec_DataVtable* vtable)
{
	//TODO use stack allocator
	return calloc(vtable->arrayCount, vtable->unitSize);
}

a3ret releaseTemp(void* memory, const ec_DataVtable* vtable)
{
	free(memory);
	return 1;
}

void vtable_setDefaults(ec_DataVtable* out)
{
	out->arrayCount = 1;

	out->arrayIdentity = defaultArrayIdentity;
	out->arrayInvert   = defaultArrayInvert  ;
	out->arrayConcat   = defaultArrayConcat  ;
	out->arrayScale    = defaultArrayScale   ;
	out->arrayDescale  = defaultArrayDescale ;

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

#pragma endregion

#pragma region Default array operations

void* defaultArrayIdentity(void* val_out, const ec_DataVtable* funcs)
{
	for (a3index i = 0; i < funcs->arrayCount; ++i) funcs->arrayIdentity( ((char*)val_out)+funcs->unitSize, funcs );
	return val_out;
}

void* defaultArrayInvert(void* val_inout, const ec_DataVtable* funcs)
{
	for (a3index i = 0; i < funcs->arrayCount; ++i) funcs->arrayInvert( ((char*)val_inout)+funcs->unitSize, funcs);
	return val_inout;
}

void* defaultArrayConcat(void* val_out, const void* lhs, const void* rhs, const ec_DataVtable* funcs)
{
	for (a3index i = 0; i < funcs->arrayCount; ++i) funcs->arrayConcat( ((char*)val_out)+funcs->unitSize, ((char*)lhs)+funcs->unitSize, ((char*)rhs)+funcs->unitSize, funcs);
	return val_out;
}

void* defaultArrayScale(void* val_inout, const a3real scale, const ec_DataVtable* funcs)
{
	for (a3index i = 0; i < funcs->arrayCount; ++i) funcs->arrayScale( ((char*)val_inout)+funcs->unitSize, scale, funcs);
	return val_inout;
}

void* defaultArrayDescale(void* val_inout, const a3real scale, const ec_DataVtable* funcs)
{
	for (a3index i = 0; i < funcs->arrayCount; ++i) funcs->arrayDescale( ((char*)val_inout)+funcs->unitSize, scale, funcs);
	return val_inout;
}

void* defaultCopy(void* dst, const void* src, const ec_DataVtable* funcs)
{
	memcpy(dst, src, funcs->unitSize * funcs->arrayCount);
	return dst;
}

#pragma endregion

#pragma region Default advanced operations

void* defaultLerp(void* val_out, const void* v0, const void* v1, const a3real param, const ec_DataVtable* funcs)
{
	funcs->copy       (val_out, v0,          funcs); //val_out = v0
	funcs->arrayInvert(val_out,              funcs); //val_out = v0^-1
	funcs->arrayConcat(val_out, v1, val_out, funcs); //val_out = v1 * v0^-1
	funcs->arrayScale (val_out, param,       funcs); //val_out = (v1 * v0^-1) * u
	funcs->arrayConcat(val_out, val_out, v0, funcs); //val_out = ((v1 * v0^-1) * u) * v0
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

		void* val2 = allocTemp(funcs);
		funcs->copy(val2, v2, funcs);

		void* val3 = allocTemp(funcs);
		funcs->copy(val3, v3, funcs);

		void* val4 = allocTemp(funcs);
		funcs->copy(val4, v4, funcs);

		funcs->arrayScale(val_out, (a3real)(-param + 2 * pow(param, 2) - pow(param, 3)), funcs);
		funcs->arrayScale(val2, (a3real)(2 - 5 * pow(param, 2) + 3 * pow(param, 3)), funcs);
		funcs->arrayScale(val3, (a3real)(param + 4 * pow(param, 2) - 3 * pow(param, 3)), funcs);
		funcs->arrayScale(val4, (a3real)(-1 * pow(param, 2) + pow(param, 3)), funcs);

		funcs->arrayConcat(val_out, val_out, val2, funcs);
		funcs->arrayConcat(val_out, val_out, val3, funcs);
		funcs->arrayConcat(val_out, val_out, val4, funcs);

		funcs->arrayScale(val_out, 0.5f, funcs);

		releaseTemp(val2, funcs);
		releaseTemp(val3, funcs);
		releaseTemp(val4, funcs);
	}
	return val_out;
}

void* defaultDeconcat(void* val_out, const void* lhs, const void* rhs, const ec_DataVtable* funcs)
{
	funcs->copy       (val_out, rhs,          funcs); //val_out = rhs
	funcs->arrayInvert(val_out,               funcs); //val_out = rhs^-1
	funcs->arrayConcat(val_out, lhs, val_out, funcs); //val_out = lhs * rhs^-1
	return val_out;
}

void* defaultTriangular(void* val_out, const void* v0, const void* v1, const void* v2, const a3real param1, const a3real param2, const ec_DataVtable* funcs)
{
	if (val_out && v0 && v1 && v2)
	{
		a3real param3 = 1 - param1 - param2;

		funcs->copy(val_out, v0, funcs);
		funcs->arrayScale(val_out, param3, funcs);

		void* val1 = allocTemp(funcs);
		funcs->copy(val1, v1, funcs);
		funcs->arrayScale(val1, param1, funcs);

		void* val2 = allocTemp(funcs);
		funcs->copy(val2, v2, funcs);
		funcs->arrayScale(val2, param2, funcs);

		funcs->arrayConcat(val_out, val_out, val1, funcs);
		funcs->arrayConcat(val_out, val_out, val2, funcs);

		releaseTemp(val1, funcs);
		releaseTemp(val2, funcs);
	}

	return val_out;
}

void* defaultBiLerp(void* val_out, const void* v00, const void* v01, const void* v10, const void* v11, const a3real paramX0, const a3real paramX1, const a3real paramY, const ec_DataVtable* funcs)
{
	void* vx0 = allocTemp(funcs);
	void* vx1 = allocTemp(funcs);
	funcs->lerp(vx0, v00, v01, paramX0, funcs);
	funcs->lerp(vx1, v10, v11, paramX1, funcs);
	funcs->lerp(val_out, vx0, vx1, paramY, funcs);
	releaseTemp(vx0, funcs);
	releaseTemp(vx1, funcs);
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
		void* val1 = allocTemp(funcs);
		void* val2 = allocTemp(funcs);
		void* val3 = allocTemp(funcs);
		void* val4 = allocTemp(funcs);

		funcs->cubic(val_out,
			funcs->cubic(val1, v1, v2, v3, v4, param0, funcs),
			funcs->cubic(val2, v5, v6, v7, v8, param1, funcs),
			funcs->cubic(val3, v9, v10, v11, v12, param2, funcs),
			funcs->cubic(val4, v13, v14, v15, v16, param3, funcs),
			param4, funcs);

		releaseTemp(val1, funcs);
		releaseTemp(val2, funcs);
		releaseTemp(val3, funcs);
		releaseTemp(val4, funcs);
	}

	return val_out;
}

a3real smoothStep(a3real x, a3real sharpness)
{
	assert(sharpness > 0);
	assert(0 <= x && x <= 1);
	// slope will be 0 at x=0 and x=1, therefore must have derivative (x)(x-1)
	// integrated derivative: -x^3/3 + x^2/2
	// rescaled to pass through (0,0) and (1,1): -2x^3 + 3x^2
	return -2*x*x*x + 3*x*x;
}

#pragma endregion

#pragma region SpacialPose Specific Functions

a3_SpatialPose* spacialPoseConvert(a3_SpatialPose* spacialPose_Out)
{
	a3spatialPoseConvert(spacialPose_Out, a3poseChannel_none, a3poseEulerOrder_zyx);
	return spacialPose_Out;
}
a3_SpatialPose* spacialPoseRevert(a3_SpatialPose* spacialPose_Out)
{
	a3spatialPoseRestore(spacialPose_Out, a3poseChannel_none, a3poseEulerOrder_zyx);
	return spacialPose_Out;
}

#pragma endregion

#pragma region HierarchyPose Variants

a3_HierarchyPose* hierarchyPoseScaleUniform(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* in, const a3real scale, a3ui32 numNodes)
{
	if (hierarchyPose_Out && in && numNodes > 0)
	{
		ec_DataVtable vtable = vtable_SpatialPose;
		vtable.arrayCount = numNodes;
		vtable.copy(hierarchyPose_Out->pose, in->pose, &vtable);
		vtable.arrayScale(hierarchyPose_Out->pose, scale, &vtable);
	}
	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyPoseScalePerNode(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* in, const a3real* scalesPerNode, a3ui32 numNodes)
{
	if (hierarchyPose_Out && in && numNodes > 0 )
	{
		for(a3ui32 i = 0; i < numNodes; i++)
		{
			vtable_SpatialPose.copy(&hierarchyPose_Out->pose[i], &in->pose[i], &vtable_SpatialPose);
			vtable_SpatialPose.arrayScale(&hierarchyPose_Out->pose[i], scalesPerNode[i], &vtable_SpatialPose);
		}
	}
	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyPoseCopy(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos0, a3ui32 numNodes)
{
	if (hierarchyPose_Out && pos0 && numNodes > 0 )
	{
		ec_DataVtable vtable = vtable_SpatialPose;
		vtable.arrayCount = numNodes;
		vtable.copy(hierarchyPose_Out->pose, pos0->pose, &vtable);
	}
	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyPoseLerp(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos0, const a3_HierarchyPose* pos1, a3ui32 numNodes, const a3real param)
{
	if (hierarchyPose_Out && pos0 && pos1 && numNodes > 0)
	{
		ec_DataVtable vtable = vtable_SpatialPose;
		vtable.arrayCount = numNodes;
		vtable.lerp(hierarchyPose_Out->pose, pos0->pose, pos1->pose, param, &vtable);
	}
	
	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyPoseNearest(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos0, const a3_HierarchyPose* pos1, a3ui32 numNodes, const a3real param)
{
	if (hierarchyPose_Out && pos0 && pos1 && numNodes > 0)
	{
		ec_DataVtable vtable = vtable_SpatialPose;
		vtable.arrayCount = numNodes;
		vtable.nearest(hierarchyPose_Out->pose, pos0->pose, pos1->pose, param, &vtable);
	}

	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyPoseCubic(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos1, const a3_HierarchyPose* pos2, const a3_HierarchyPose* pos3, const a3_HierarchyPose* pos4, a3ui32 numNodes, const a3real param)
{
	if (hierarchyPose_Out && pos1 && pos2 && pos3 && pos4 && numNodes > 0)
	{
		ec_DataVtable vtable = vtable_SpatialPose;
		vtable.arrayCount = numNodes;
		vtable.cubic(hierarchyPose_Out->pose, pos1->pose, pos2->pose, pos3->pose, pos4->pose, param, &vtable);
	}

	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyPoseDeconcat(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* lhs, const a3_HierarchyPose* rhs, a3ui32 numNodes)
{
	if (hierarchyPose_Out && lhs && rhs && numNodes > 0)
	{
		ec_DataVtable vtable = vtable_SpatialPose;
		vtable.arrayCount = numNodes;
		vtable.deconcat(hierarchyPose_Out->pose, lhs->pose, rhs->pose, &vtable);
	}

	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyPoseTriangular(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos0, const a3_HierarchyPose* pos1, const a3_HierarchyPose* pos2, const a3real param1, const a3real param2, a3ui32 numNodes)
{
	if (hierarchyPose_Out && pos0 && pos1 && pos2 && numNodes > 0)
	{
		ec_DataVtable vtable = vtable_SpatialPose;
		vtable.arrayCount = numNodes;
		vtable.triangular(hierarchyPose_Out->pose, pos0->pose, pos1->pose, pos2->pose, param1, param2, &vtable);
	}

	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyPoseBiLerp(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos00, const a3_HierarchyPose* pos01, const a3_HierarchyPose* pos10, const a3_HierarchyPose* pos11, a3ui32 numNodes, const a3real paramX0, const a3real paramX1, const a3real paramY)
{
	if (hierarchyPose_Out && pos00 && pos01 && pos10 && pos11 && numNodes > 0)
	{
		ec_DataVtable vtable = vtable_SpatialPose;
		vtable.arrayCount = numNodes;
		vtable.biLerp(hierarchyPose_Out->pose, pos00->pose, pos01->pose, pos10->pose, pos11->pose, paramX0, paramX1, paramY, &vtable);
	}

	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyPoseBiNearest(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos00, const a3_HierarchyPose* pos01, const a3_HierarchyPose* pos10, const a3_HierarchyPose* pos11, a3ui32 numNodes, const a3real paramX0, const a3real paramX1, const a3real paramY)
{
	if (hierarchyPose_Out && pos00 && pos01 && pos10 && pos11 && numNodes > 0)
	{
		ec_DataVtable vtable = vtable_SpatialPose;
		vtable.arrayCount = numNodes;
		vtable.biNearest(hierarchyPose_Out->pose, pos00->pose, pos01->pose, pos10->pose, pos11->pose, paramX0, paramX1, paramY, &vtable);
	}

	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyPoseBiCubic(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos1, const a3_HierarchyPose* pos2, const a3_HierarchyPose* pos3, const a3_HierarchyPose* pos4, const a3_HierarchyPose* pos5, const a3_HierarchyPose* pos6, const a3_HierarchyPose* pos7, const a3_HierarchyPose* pos8, const a3_HierarchyPose* pos9, const a3_HierarchyPose* pos10, const a3_HierarchyPose* pos11, const a3_HierarchyPose* pos12, const a3_HierarchyPose* pos13, const a3_HierarchyPose* pos14, const a3_HierarchyPose* pos15, const a3_HierarchyPose* pos16, const a3real param0, const a3real param1, const a3real param2, const a3real param3, const a3real param4, const a3ui32 numNodes)
{
	if (hierarchyPose_Out && pos1 && pos2 && pos3 && pos4 && pos5 && pos6 && pos7 && pos8 && pos9 && pos10 && pos11 && pos12 && pos13 && pos14 && pos15 && pos16 && numNodes > 0)
	{
		ec_DataVtable vtable = vtable_SpatialPose;
		vtable.arrayCount = numNodes;
		vtable.biCubic(hierarchyPose_Out->pose, pos1->pose, pos2->pose, pos3->pose, pos4->pose, pos5->pose, pos6->pose, pos7->pose, pos8->pose, pos9->pose, pos10->pose, pos11->pose, pos12->pose, pos13->pose, pos14->pose, pos15->pose, pos16->pose, param0, param1, param2, param3, param4, &vtable);
	}

	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyConvert(a3_HierarchyPose* hierarchyPose_Out, const a3ui32 numNodes)
{
	if (hierarchyPose_Out)
	{
		for (a3ui32 i = 0; i < numNodes; i++)
		{
			hierarchyPose_Out->pose[i] = *spacialPoseConvert(&hierarchyPose_Out->pose[i]);
		}
	}

	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyRevert(a3_HierarchyPose* hierarchyPose_Out, const a3ui32 numNodes)
{
	if (hierarchyPose_Out)
	{
		for (a3ui32 i = 0; i < numNodes; i++)
		{
			hierarchyPose_Out->pose[i] = *spacialPoseRevert(&hierarchyPose_Out->pose[i]);
		}
	}

	return hierarchyPose_Out;
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
	(*val_inout)[0] = (a3real)pow((*val_inout)[0], power);
	(*val_inout)[1] = (a3real)pow((*val_inout)[1], power);
	(*val_inout)[2] = (a3real)pow((*val_inout)[2], power);

	/*
	a3real invLen = a3real3LengthInverse(*val_inout);
	a3real len = 1/invLen;
	a3real3MulS(*val_inout, invLen);
	
	a3real targetLen = (a3real)pow(len, power);
	a3real3MulS(*val_inout, targetLen);
	*/
	return val_inout;
}

a3real3* a3real3DivideS(a3real3* val_inout, const a3real power)
{
	a3real invLen;
	a3real3NormalizeGetInvLength(*val_inout, &invLen);
	a3real3DivS(*val_inout, (a3real)pow(1 / invLen, power));
	return val_inout;
}

a3quat* a3quatMulS(a3quat* val_inout, const a3real scale)
{
	a3quat tmp = { 0 }; //Same var used for read/write may have unintended consequences
	a3quatSlerpIdentityQ0(tmp.q, val_inout->q, scale);
	a3real4SetReal4(val_inout->q, tmp.q);
	return val_inout;
}

//Have Robert Look Over
a3quat* a3quatDivS(a3quat* val_inout, const a3real scale) 
{
	a3quat tmp = { 0 }; //Same var used for read/write may have unintended consequences
	a3quatSlerpIdentityQ1(tmp.q, val_inout->q, scale);
	a3real4SetReal4(val_inout->q, tmp.q);
	return val_inout;
}

a3_SpatialPose* a3spatialPoseMulS(a3_SpatialPose* val_inout, const a3real scale)
{
	vtable_vec3Additive      .unitScale(&val_inout->translate, scale);
	vtable_vec3Additive      .unitScale(&val_inout->rotate     , scale);
	//vtable_quat              .scale(&val_inout->transformDQ, scale);
	vtable_vec3Multiplicative.unitScale(&val_inout->scale      , scale);
	a3spatialPoseConvert(val_inout, ~0, a3poseEulerOrder_zxy);
	//vtable_mat4              .scale(&val_inout->transformMat  , scale);
	return val_inout;
}

a3_SpatialPose* a3spatialPoseDivS(a3_SpatialPose* val_inout, const a3real scale)
{
	vtable_vec3Additive.unitDescale(&val_inout->translate, scale);
	vtable_vec3Additive.unitDescale(&val_inout->rotate, scale);
	vtable_quat.unitDescale(&val_inout->transformDQ, scale);
	vtable_vec3Multiplicative.unitDescale(&val_inout->scale, scale);
	vtable_mat4.unitDescale(&val_inout->transformMat, scale);
	return val_inout;
}

a3_SpatialPose* a3spatialPoseLerp_adapter(a3_SpatialPose* spatialPose_out, const a3_SpatialPose* spatialPose_0, const a3_SpatialPose* spatialPose_1, const a3real u, const ec_DataVtable* funcs)
{
	for (a3index i = 0; i < funcs->arrayCount; ++i) a3spatialPoseLerp(spatialPose_out+i, spatialPose_0+i, spatialPose_1+i, u);
	return spatialPose_out;
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
	vtable_mat4              .unitSize = sizeof(a3mat4);
	vtable_SpatialPose       .unitSize = sizeof(a3_SpatialPose);
	vtable_vec3Additive      .unitSize = sizeof(a3vec3);
	vtable_vec3Multiplicative.unitSize = sizeof(a3vec3);
	vtable_quat              .unitSize = sizeof(a3quat);

	//mat4
	vtable_mat4.unitIdentity = (fp_identity) a3real4x4SetIdentity;
	vtable_mat4.unitInvert   = (fp_invert  ) a3real4x4Invert;
	vtable_mat4.unitConcat   = (fp_concat  ) a3real4x4Product;
	vtable_mat4.unitScale    = (fp_scale   ) a3real4x4MulS;
	vtable_mat4.unitDescale  = (fp_descale ) a3real4x4DivS;

	//spatial pose
	vtable_SpatialPose.unitIdentity = (fp_identity) a3spatialPoseReset;
	vtable_SpatialPose.unitInvert   = (fp_invert  ) a3spatialPoseInvert;
	vtable_SpatialPose.unitConcat   = (fp_concat  ) a3spatialPoseConcat;
	vtable_SpatialPose.unitScale    = (fp_scale   ) a3spatialPoseMulS;
	vtable_SpatialPose.unitDescale  = (fp_descale ) a3spatialPoseDivS;
	vtable_SpatialPose.lerp         = (fp_lerp    ) a3spatialPoseLerp_adapter;
	
	//translation and euler angles
	vtable_vec3Additive.unitIdentity = (fp_identity) a3real3SetZero;
	vtable_vec3Additive.unitInvert   = (fp_invert  ) a3real3Negate;
	vtable_vec3Additive.unitConcat   = (fp_concat  ) a3real3Sum;
	vtable_vec3Additive.unitScale    = (fp_scale   ) a3real3MulS;

	//rotation
	vtable_quat.unitIdentity = (fp_identity) a3quatSetIdentity;
	vtable_quat.unitInvert   = (fp_invert  ) a3quatInvert;
	vtable_quat.unitConcat   = (fp_concat  ) a3quatProduct;
	vtable_quat.unitScale    = (fp_scale   ) a3quatMulS;
	vtable_quat.unitDescale  = (fp_descale ) a3quatDivS;

	//scale
	vtable_vec3Multiplicative.unitIdentity = (fp_identity) a3real3SetOne;
	vtable_vec3Multiplicative.unitInvert   = (fp_invert  ) a3real3InvertLength;
	vtable_vec3Multiplicative.unitConcat   = (fp_concat  ) a3real3MulComp;
	vtable_vec3Multiplicative.unitScale	   = (fp_scale   ) a3real3PowS;
	vtable_vec3Multiplicative.unitDescale  = (fp_descale ) a3real3DivideS;
}

#pragma endregion