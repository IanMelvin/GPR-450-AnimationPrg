#include "a3_SpatialPose.h"
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


//-----------------------------------------------------------------------------

inline a3i32 a3spatialPoseInit(a3_SpatialPose* spatialPose, a3_SpatialPoseEulerOrder eulerOrder)
{
	spatialPose->transform = a3mat4_identity;
	spatialPose->orientation = a3vec3_zero;
	spatialPose->scale = a3vec3_one;
	spatialPose->translation = a3vec3_zero;

	return 1;
}

// set rotation values for a single node pose
inline a3i32 a3spatialPoseSetRotation(a3_SpatialPose* spatialPose, const a3f32 rx_degrees, const a3f32 ry_degrees, const a3f32 rz_degrees)
{
	if (spatialPose)
	{
		spatialPose->orientation.x = rx_degrees;
		spatialPose->orientation.y = ry_degrees;
		spatialPose->orientation.z = rz_degrees;
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
		spatialPose->transform = a3mat4_identity;
		spatialPose->orientation = a3vec3_one;
		spatialPose->scale = a3vec3_one;
		spatialPose->translation = a3vec3_one;
	}
	return -1;
}

// convert single node pose to matrix
inline a3i32 a3spatialPoseConvert(a3mat4* mat_out, const a3_SpatialPose* spatialPose_in, const a3_SpatialPoseChannel channel, const a3_SpatialPoseEulerOrder order)
{
	if (mat_out && spatialPose_in)
	{
		//Scale matrix
		a3mat4 scaleMatrix = {
			{spatialPose_in->scale.x, 0.0f, 0.0f, 0.0f,
			0.0f, spatialPose_in->scale.y, 0.0f, 0.0f,
			0.0f, 0.0f, spatialPose_in->scale.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f}
		};

		//transform matrix
		a3mat4 transformMatrix = {
			{1.0f, 0.0f, 0.0f, spatialPose_in->translation.x,
			0.0f, 1.0f, 0.0f, spatialPose_in->translation.y,
			0.0f, 0.0f, 1.0f, spatialPose_in->translation.z,
			0.0f, 0.0f, 0.0f, 1.0f}
		};

		//Orientation matrices
		a3mat4 orientationMatrixX = {
			{1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, a3cosd(spatialPose_in->orientation.x), a3sind(spatialPose_in->orientation.x), 0.0f,
			0.0f, -a3sind(spatialPose_in->orientation.x), a3cosd(spatialPose_in->orientation.x), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f}
		};

		a3mat4 orientationMatrixY = {
			{a3cosd(spatialPose_in->orientation.y), 0.0f, -a3sind(spatialPose_in->orientation.y), 0.0f,
			0.0f, 1.0, 0.0f, 0.0f,
			a3sind(spatialPose_in->orientation.y), 0.0f, a3cosd(spatialPose_in->orientation.y), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f}
		};

		a3mat4 orientationMatrixZ = {
			{a3cosd(spatialPose_in->orientation.z), -a3sind(spatialPose_in->orientation.z), 0.0f, 0.0f,
			a3sind(spatialPose_in->orientation.z), a3cosd(spatialPose_in->orientation.z), 0.0f, 0.0f,
			0.0f, 0.0f, 1.0, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f}
		};

		//Combine orientation matricies
		a3real4x4Product(mat_out->m, orientationMatrixX.m, orientationMatrixY.m);
		a3real4x4Product(mat_out->m, mat_out->m, orientationMatrixZ.m);

		//Combine scale and orientation matricices
		a3real4x4Product(mat_out->m, scaleMatrix.m, mat_out->m);

		//Combine transform with result of combining scale and orientation matricies
		a3real4x4Product(mat_out->m, transformMatrix.m, mat_out->m);

		return 1;
	}
	return -1;
}

// copy operation for single node pose
inline a3i32 a3spatialPoseCopy(a3_SpatialPose* spatialPose_out, const a3_SpatialPose* spatialPose_in)
{
	if (spatialPose_out && spatialPose_in)
	{
		spatialPose_out->transform = spatialPose_in->transform;
		spatialPose_out->orientation = spatialPose_in->orientation;
		spatialPose_out->scale = spatialPose_in->scale;
		spatialPose_out->translation = spatialPose_in->translation;
	}
	return -1;
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_SPATIALPOSE_INL
#endif	// __ANIMAL3D_SPATIALPOSE_H