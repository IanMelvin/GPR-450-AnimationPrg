#include "a3_SpatialPose.h"
#include "..\a3_SpatialPose.h"
#include "A3_DEMO/ec_MathBridge.h"
/*
	Copyright 2011-2020 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	a3_SpatialPose.inl
	Implementation of inline spatial pose operations.
*/


#ifdef __ANIMAL3D_SPATIALPOSE_H
#ifndef __ANIMAL3D_SPATIALPOSE_INL
#define __ANIMAL3D_SPATIALPOSE_INL

#include <math.h>

//-----------------------------------------------------------------------------

inline a3real4x4r ec_eulerToMat4x4(a3real4x4p mat_out, a3vec3 eulerAngles, const a3_SpatialPoseEulerOrder order)
{
	a3real4x4SetIdentity(mat_out);

	eulerAngles.x = (a3real)fmod(eulerAngles.x, 360);
	eulerAngles.y = (a3real)fmod(eulerAngles.y, 360);
	eulerAngles.z = (a3real)fmod(eulerAngles.z, 360);

	// Setup axis matrices

	a3real sinRotX = a3sind(eulerAngles.x);
	a3real cosRotX = a3cosd(eulerAngles.x);
	a3mat4 matRotX = {{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cosRotX, -sinRotX, 0.0f,
		0.0f, sinRotX, cosRotX, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	}};

	a3real sinRotY = a3sind(eulerAngles.y);
	a3real cosRotY = a3cosd(eulerAngles.y);
	a3mat4 matRotY = {{
		cosRotY, 0.0f, sinRotY, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sinRotY, 0.0f, cosRotY, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	}};

	a3real sinRotZ = a3sind(eulerAngles.z);
	a3real cosRotZ = a3cosd(eulerAngles.z);
	a3mat4 matRotZ = {{
		cosRotZ, -sinRotZ, 0.0f, 0.0f,
		sinRotZ, cosRotZ, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	}};


	// Apply in order

	switch (order>>a3poseEulerOrder_order1 & 0xf)
	{
	case a3poseEulerOrder_idx: a3real4x4ConcatL(mat_out, matRotX.m); break;
	case a3poseEulerOrder_idy: a3real4x4ConcatL(mat_out, matRotY.m); break;
	case a3poseEulerOrder_idz: a3real4x4ConcatL(mat_out, matRotZ.m); break;
	}

	switch (order>>a3poseEulerOrder_order2 & 0xf)
	{
	case a3poseEulerOrder_idx: a3real4x4ConcatL(mat_out, matRotX.m); break;
	case a3poseEulerOrder_idy: a3real4x4ConcatL(mat_out, matRotY.m); break;
	case a3poseEulerOrder_idz: a3real4x4ConcatL(mat_out, matRotZ.m); break;
	}

	switch (order>>a3poseEulerOrder_order3 & 0xf)
	{
	case a3poseEulerOrder_idx: a3real4x4ConcatL(mat_out, matRotX.m); break;
	case a3poseEulerOrder_idy: a3real4x4ConcatL(mat_out, matRotY.m); break;
	case a3poseEulerOrder_idz: a3real4x4ConcatL(mat_out, matRotZ.m); break;
	}

	return mat_out;
}

inline a3real4r ec_eulerToQuat(a3real4p quat_out, const a3vec3 eulerAngles, const a3_SpatialPoseEulerOrder order)
{
	a3quatSetIdentity(quat_out);

	// Setup axis quaternions
	
	a3quat rx_quat = { 0 };
	a3quat ry_quat = { 0 };
	a3quat rz_quat = { 0 };
	a3quatSetEulerX(rx_quat.q, eulerAngles.x);
	a3quatSetEulerY(ry_quat.q, eulerAngles.y);
	a3quatSetEulerZ(rz_quat.q, eulerAngles.z);

	// Apply in order
	
	switch (order>>a3poseEulerOrder_order1 & 0xf)
	{
	case a3poseEulerOrder_idx: a3quatConcatL(quat_out, rx_quat.q); break;
	case a3poseEulerOrder_idy: a3quatConcatL(quat_out, ry_quat.q); break;
	case a3poseEulerOrder_idz: a3quatConcatL(quat_out, rz_quat.q); break;
	}

	switch (order>>a3poseEulerOrder_order2 & 0xf)
	{
	case a3poseEulerOrder_idx: a3quatConcatL(quat_out, rx_quat.q); break;
	case a3poseEulerOrder_idy: a3quatConcatL(quat_out, ry_quat.q); break;
	case a3poseEulerOrder_idz: a3quatConcatL(quat_out, rz_quat.q); break;
	}

	switch (order>>a3poseEulerOrder_order3 & 0xf)
	{
	case a3poseEulerOrder_idx: a3quatConcatL(quat_out, rx_quat.q); break;
	case a3poseEulerOrder_idy: a3quatConcatL(quat_out, ry_quat.q); break;
	case a3poseEulerOrder_idz: a3quatConcatL(quat_out, rz_quat.q); break;
	}

	return quat_out;
}

inline a3i32 a3spatialPoseInit(a3_SpatialPose* spatialPose)
{
#ifdef USE_EULER_ANGLES
	spatialPose->orientation = a3vec3_zero;
#else
	spatialPose->orientation = a3quat_identity;
#endif
	spatialPose->scale = a3vec3_one;
	spatialPose->translation = a3vec3_zero;

	return 1;
}

// set rotation values for a single node pose
inline a3i32 a3spatialPoseSetRotation(a3_SpatialPose* spatialPose, const a3f32 rx_degrees, const a3f32 ry_degrees, const a3f32 rz_degrees, const a3_SpatialPoseEulerOrder eulerOrder)
{
	if (spatialPose)
	{
#ifdef USE_EULER_ANGLES
		spatialPose->orientation.x = rx_degrees;
		spatialPose->orientation.y = ry_degrees;
		spatialPose->orientation.z = rz_degrees;
#else
		a3vec3 eulerAngle = { rx_degrees, ry_degrees, rz_degrees };
		ec_eulerToQuat(spatialPose->orientation.q, eulerAngle, eulerOrder);
		
		//DO NOT USE
		//a3quatSetEulerXYZ(spatialPose->orientation.q, rx_degrees, ry_degrees, rz_degrees);
#endif
	}
	return -1;
}

// scale
inline a3i32 a3spatialPoseSetScale(a3_SpatialPose* spatialPose, const a3f32 sx, const a3f32 sy, const a3f32 sz)
{
	if (spatialPose)
	{
		spatialPose->scale.x = sx;
		spatialPose->scale.y = sy;
		spatialPose->scale.z = sz;
		//spatialPose->transform.m00 *= sx;
		//spatialPose->transform.m11 *= sy;
		//spatialPose->transform.m22 *= sz;
	}
	return -1;
}

// translation
inline a3i32 a3spatialPoseSetTranslation(a3_SpatialPose* spatialPose, const a3f32 tx, const a3f32 ty, const a3f32 tz)
{
	if (spatialPose)
	{
		spatialPose->translation.x = tx;
		spatialPose->translation.y = ty;
		spatialPose->translation.z = tz;
		//spatialPose->transform.m30 = tx;
		//spatialPose->transform.m31 = ty;
		//spatialPose->transform.m32 = tz;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// reset single node pose
inline a3i32 a3spatialPoseReset(a3_SpatialPose* spatialPose)
{
	if (spatialPose)
	{
#ifdef USE_EULER_ANGLES
		spatialPose->orientation = a3vec3_zero;
#else
		spatialPose->orientation = a3quat_identity;
#endif
		spatialPose->scale = a3vec3_one;
		spatialPose->translation = a3vec3_zero;
	}
	return -1;
}

// convert single node pose to matrix
inline a3i32 a3spatialPoseConvert(a3mat4* mat_out, const a3_SpatialPose* spatialPose_in, const a3_SpatialPoseChannel channel, const a3_SpatialPoseEulerOrder order)
{
	if (mat_out && spatialPose_in)
	{
		// Build component matrices

		a3mat4 matScale = a3mat4_identity;
		if (channel & a3poseChannel_scale_x) matScale.m00 = spatialPose_in->scale.x;
		if (channel & a3poseChannel_scale_y) matScale.m11 = spatialPose_in->scale.y;
		if (channel & a3poseChannel_scale_z) matScale.m22 = spatialPose_in->scale.z;
		
		a3mat4 matRotate = { 0 };
#ifdef USE_EULER_ANGLES
		a3vec3 eulerAngles = { 0 };
		if (channel & a3poseChannel_orient_x) eulerAngles.x = spatialPose_in->orientation.x;
		if (channel & a3poseChannel_orient_y) eulerAngles.y = spatialPose_in->orientation.y;
		if (channel & a3poseChannel_orient_z) eulerAngles.z = spatialPose_in->orientation.z;
		ec_eulerToMat4x4(matRotate.m, eulerAngles, order);
#else
		/*
		a3vec3 axis = { 0 };
		a3real angle = 0;
		a3real scale = 0;
		a3quatGetAxisAngleScale(spatialPose_in->orientation.q, axis.v, &angle, &scale); //TODO @rsc should this have scale?
		if (!(channel & a3poseChannel_orient_x)) axis.x = 0;
		if (!(channel & a3poseChannel_orient_y)) axis.y = 0;
		if (!(channel & a3poseChannel_orient_z)) axis.z = 0;
		
		a3quat constrainedRotation = { 0 };
		a3quatSetAxisAngleScale(constrainedRotation.q, axis.v, angle, scale);

		a3mat4 matRotate = { 0 };
		a3quatConvertToMat4(matRotate.m, constrainedRotation.q);
		// */
		a3quatConvertToMat4(matRotate.m, spatialPose_in->orientation.q);
#endif

		// Concat

		*mat_out = matScale;
		a3real4x4ConcatL(mat_out->m, matRotate.m);
		if (channel & a3poseChannel_translate_x) mat_out->m30 = spatialPose_in->translation.x;
		if (channel & a3poseChannel_translate_y) mat_out->m31 = spatialPose_in->translation.y;
		if (channel & a3poseChannel_translate_z) mat_out->m32 = spatialPose_in->translation.z;
		
		return 1;
	}
	return -1;
}

inline a3i32 a3spatialPoseConcat(a3_SpatialPose* finalPose, const a3_SpatialPose* basePose, const a3_SpatialPose* deltaPose)
{
	//Scale first
	finalPose->scale.x = basePose->scale.x * deltaPose->scale.x;
	finalPose->scale.y = basePose->scale.y * deltaPose->scale.y;
	finalPose->scale.z = basePose->scale.z * deltaPose->scale.z;

	//Then rotate
#ifdef USE_EULER_ANGLES
	a3real3Sum(finalPose->orientation.v, basePose->orientation.v, deltaPose->orientation.v);
#else
	finalPose->orientation = basePose->orientation;
	a3quatConcatL(finalPose->orientation.q, deltaPose->orientation.q);
#endif
	a3real3Sum(finalPose->translation.v, basePose->translation.v, deltaPose->translation.v); //Then translate

	return 1;
}

// copy operation for single node pose
inline a3i32 a3spatialPoseCopy(a3_SpatialPose* spatialPose_out, const a3_SpatialPose* spatialPose_in)
{
	if (spatialPose_out && spatialPose_in)
	{
		spatialPose_out->orientation = spatialPose_in->orientation;
		spatialPose_out->scale = spatialPose_in->scale;
		spatialPose_out->translation = spatialPose_in->translation;
	}
	return -1;
}

// lerp between spatial poses
inline a3_SpatialPose* a3spatialPoseLerp(a3_SpatialPose* out, const a3_SpatialPose* val0, const a3_SpatialPose* val1, a3real param)
{
	a3real3Lerp(out->translation.v, val0->translation.v, val1->translation.v, param);
#ifdef USE_EULER_ANGLES
	a3real3Lerp(out->orientation.v, val0->orientation.v, val1->orientation.v, param);
#else
	a3quatSlerp(out->orientation.q, val0->orientation.q, val1->orientation.q, param);
#endif
	a3vec3LogLerp(out->scale.v, val0->scale.v, val1->scale.v, param);
	return out;
}

//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_SPATIALPOSE_INL
#endif	// __ANIMAL3D_SPATIALPOSE_H