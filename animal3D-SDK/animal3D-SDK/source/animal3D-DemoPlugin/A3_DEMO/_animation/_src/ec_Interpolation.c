#include "A3_DEMO/_animation/ec_Interpolation.h"

#include <stdlib.h>
#include <string.h>
#include <Math.h>
#include <assert.h>

#include <animal3D-A3DM/animal3D-A3DM.h>

#include "A3_DEMO/_animation/a3_SpatialPose.h"

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

		free(val2);
		free(val3);
		free(val4);
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

		free(val1);
		free(val2);
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
	free(vx0); //Also gets vx1
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

		free(val1);
		free(val2);
		free(val3);
		free(val4);
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
		for (a3ui32 i = 0; i < numNodes; i++)
		{
			vtable_SpatialPose.copy(&hierarchyPose_Out->pose[i], &in->pose[i], &vtable_SpatialPose);
			vtable_SpatialPose.scale(&hierarchyPose_Out->pose[i], scale);
		}
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
			vtable_SpatialPose.scale(&hierarchyPose_Out->pose[i], scalesPerNode[i]);
		}
	}
	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyPoseCopy(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos0, a3ui32 numNodes)
{
	if (hierarchyPose_Out && pos0 && numNodes > 0 )
	{
		for(a3ui32 i = 0; i < numNodes; i++)
		{
			vtable_SpatialPose.copy(&hierarchyPose_Out->pose[i], &pos0->pose[i], &vtable_SpatialPose);
		}
	}
	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyPoseLerp(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos0, const a3_HierarchyPose* pos1, a3ui32 numNodes, const a3real param)
{
	if (hierarchyPose_Out && pos0 && pos1 && numNodes > 0)
	{
		for (a3ui32 i = 0; i < numNodes; i++)
		{
			vtable_SpatialPose.lerp(&hierarchyPose_Out->pose[i], &pos0->pose[i], &pos1->pose[i], param, &vtable_SpatialPose);
		}
	}
	
	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyPoseNearest(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos0, const a3_HierarchyPose* pos1, a3ui32 numNodes, const a3real param)
{
	if (hierarchyPose_Out && pos0 && pos1 && numNodes > 0)
	{
		for (a3ui32 i = 0; i < numNodes; i++)
		{
			vtable_SpatialPose.nearest(&hierarchyPose_Out->pose[i], &pos0->pose[i], &pos1->pose[i], param, &vtable_SpatialPose);
		}
	}

	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyPoseCubic(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos1, const a3_HierarchyPose* pos2, const a3_HierarchyPose* pos3, const a3_HierarchyPose* pos4, a3ui32 numNodes, const a3real param)
{
	if (hierarchyPose_Out && pos1 && pos2 && pos3 && pos4 && numNodes > 0)
	{
		for (a3ui32 i = 0; i < numNodes; i++)
		{
			vtable_SpatialPose.cubic(&hierarchyPose_Out->pose[i], &pos1->pose[i], &pos2->pose[i], &pos3->pose[i], &pos4->pose[i], param, &vtable_SpatialPose);
		}
	}

	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyPoseDeconcat(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* lhs, const a3_HierarchyPose* rhs, a3ui32 numNodes)
{
	if (hierarchyPose_Out && lhs && rhs && numNodes > 0)
	{
		for (a3ui32 i = 0; i < numNodes; i++)
		{
			vtable_SpatialPose.deconcat(&hierarchyPose_Out->pose[i], &lhs->pose[i], &rhs->pose[i], &vtable_SpatialPose);
		}
	}

	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyPoseTriangular(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos0, const a3_HierarchyPose* pos1, const a3_HierarchyPose* pos2, const a3real param1, const a3real param2, a3ui32 numNodes)
{
	if (hierarchyPose_Out && pos0 && pos1 && pos2 && numNodes > 0)
	{
		for (a3ui32 i = 0; i < numNodes; i++)
		{
			vtable_SpatialPose.triangular(&hierarchyPose_Out->pose[i], &pos0->pose[i], &pos1->pose[i], &pos2->pose[i], param1, param2, &vtable_SpatialPose);
		}
	}

	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyPoseBiLerp(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos00, const a3_HierarchyPose* pos01, const a3_HierarchyPose* pos10, const a3_HierarchyPose* pos11, a3ui32 numNodes, const a3real paramX0, const a3real paramX1, const a3real paramY)
{
	if (hierarchyPose_Out && pos00 && pos01 && pos10 && pos11 && numNodes > 0)
	{
		for (a3ui32 i = 0; i < numNodes; i++)
		{
			vtable_SpatialPose.biLerp(&hierarchyPose_Out->pose[i], &pos00->pose[i], &pos01->pose[i], &pos10->pose[i], &pos11->pose[i], paramX0, paramX1, paramY, &vtable_SpatialPose);
		}
	}

	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyPoseBiNearest(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos00, const a3_HierarchyPose* pos01, const a3_HierarchyPose* pos10, const a3_HierarchyPose* pos11, a3ui32 numNodes, const a3real paramX0, const a3real paramX1, const a3real paramY)
{
	if (hierarchyPose_Out && pos00 && pos01 && pos10 && pos11 && numNodes > 0)
	{
		for (a3ui32 i = 0; i < numNodes; i++)
		{
			vtable_SpatialPose.biNearest(&hierarchyPose_Out->pose[i], &pos00->pose[i], &pos01->pose[i], &pos10->pose[i], &pos11->pose[i], paramX0, paramX1, paramY, &vtable_SpatialPose);
		}
	}

	return hierarchyPose_Out;
}

a3_HierarchyPose* hierarchyPoseBiCubic(a3_HierarchyPose* hierarchyPose_Out, const a3_HierarchyPose* pos1, const a3_HierarchyPose* pos2, const a3_HierarchyPose* pos3, const a3_HierarchyPose* pos4, const a3_HierarchyPose* pos5, const a3_HierarchyPose* pos6, const a3_HierarchyPose* pos7, const a3_HierarchyPose* pos8, const a3_HierarchyPose* pos9, const a3_HierarchyPose* pos10, const a3_HierarchyPose* pos11, const a3_HierarchyPose* pos12, const a3_HierarchyPose* pos13, const a3_HierarchyPose* pos14, const a3_HierarchyPose* pos15, const a3_HierarchyPose* pos16, const a3real param0, const a3real param1, const a3real param2, const a3real param3, const a3real param4, const a3ui32 numNodes)
{
	if (hierarchyPose_Out && pos1 && pos2 && pos3 && pos4 && pos5 && pos6 && pos7 && pos8 && pos9 && pos10 && pos11 && pos12 && pos13 && pos14 && pos15 && pos16 && numNodes > 0)
	{
		for (a3ui32 i = 0; i < numNodes; i++)
		{
			vtable_SpatialPose.biCubic(&hierarchyPose_Out->pose[i], &pos1->pose[i], &pos2->pose[i], &pos3->pose[i], &pos4->pose[i], &pos5->pose[i], &pos6->pose[i], &pos7->pose[i], &pos8->pose[i], &pos9->pose[i], &pos10->pose[i], &pos11->pose[i], &pos12->pose[i], &pos13->pose[i], &pos14->pose[i], &pos15->pose[i], &pos16->pose[i], param0, param1, param2, param3, param4, &vtable_SpatialPose);
		}
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
	a3real invLen;
	a3real3NormalizeGetInvLength(*val_inout, &invLen);
	a3real3MulS(*val_inout, (a3real)pow(1/invLen, power));
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
	vtable_vec3Additive      .scale(&val_inout->translate, scale);
	vtable_vec3Additive      .scale(&val_inout->rotate     , scale);
	vtable_quat              .scale(&val_inout->transformDQ, scale);
	vtable_vec3Multiplicative.scale(&val_inout->scale      , scale);
	vtable_mat4              .scale(&val_inout->transformMat  , scale);
	return val_inout;
}

a3_SpatialPose* a3spatialPoseDivS(a3_SpatialPose* val_inout, const a3real scale)
{
	vtable_vec3Additive.descale(&val_inout->translate, scale);
	vtable_vec3Additive.descale(&val_inout->rotate, scale);
	vtable_quat.descale(&val_inout->transformDQ, scale);
	vtable_vec3Multiplicative.descale(&val_inout->scale, scale);
	vtable_mat4.descale(&val_inout->transformMat, scale);
	return val_inout;
}

a3_SpatialPose* a3spatialPoseLerp_adapter(a3_SpatialPose* spatialPose_out, const a3_SpatialPose* spatialPose_0, const a3_SpatialPose* spatialPose_1, const a3real u, const ec_DataVtable* funcs_ignored)
{
	a3spatialPoseLerp(spatialPose_out, spatialPose_0, spatialPose_1, u);
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
	vtable_mat4.descale	 = (fp_descale ) a3real4x4DivS;

	//spatial pose
	vtable_SpatialPose.identity = (fp_identity) a3spatialPoseReset;
	vtable_SpatialPose.invert   = (fp_invert  ) a3spatialPoseInvert;
	vtable_SpatialPose.concat   = (fp_concat  ) a3spatialPoseConcat;
	vtable_SpatialPose.scale    = (fp_scale   ) a3spatialPoseMulS;
	vtable_SpatialPose.descale  = (fp_descale ) a3spatialPoseDivS;
	vtable_SpatialPose.lerp     = (fp_lerp    ) a3spatialPoseLerp_adapter;
	
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
	vtable_quat.descale  = (fp_descale ) a3quatDivS;

	//scale
	vtable_vec3Multiplicative.identity = (fp_identity) a3real3SetOne;
	vtable_vec3Multiplicative.invert   = (fp_invert  ) a3real3InvertLength;
	vtable_vec3Multiplicative.concat   = (fp_concat  ) a3real3MulComp;
	vtable_vec3Multiplicative.scale	   = (fp_scale   ) a3real3PowS;
	vtable_vec3Multiplicative.descale  = (fp_descale ) a3real3DivideS;
}

#pragma endregion