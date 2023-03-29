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
	
a3_SpatialPose.h
Description of a spatial pose with rotation, translation and scale.
*/

#ifndef __ANIMAL3D_SPATIALPOSE_H
#define __ANIMAL3D_SPATIALPOSE_H


// A3 math library
#include "animal3D-A3DM/animal3D-A3DM.h"
#include "../ec_Interpolation.h"

//-----------------------------------------------------------------------------

// Compile-time switch
// Define to use Euler angles, don't define to use quaternions
#define USE_EULER_ANGLES

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef enum a3_SpatialPoseEulerOrder	a3_SpatialPoseEulerOrder;
typedef enum a3_SpatialPoseChannel		a3_SpatialPoseChannel;
typedef struct a3_SpatialPose			a3_SpatialPose;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

// list of Euler angle product orders
enum a3_SpatialPoseEulerOrder
{
	//Raw IDs of each axis
	a3poseEulerOrder_idx = 0x1,
	a3poseEulerOrder_idy = 0x2,
	a3poseEulerOrder_idz = 0x3,

	//Masks: Filter order flags
	a3poseEulerOrder_order1 = 4*0,
	a3poseEulerOrder_order2 = 4*1,
	a3poseEulerOrder_order3 = 4*2,

	// Construct order flag like: id << offset
	// Select by index: id>>offset & 0xf

	//Main euler orders, aliases built using flags
	a3poseEulerOrder_xyz = a3poseEulerOrder_idx<<a3poseEulerOrder_order1 | a3poseEulerOrder_idy<<a3poseEulerOrder_order2 | a3poseEulerOrder_idz<<a3poseEulerOrder_order3,
	a3poseEulerOrder_yzx = a3poseEulerOrder_idy<<a3poseEulerOrder_order1 | a3poseEulerOrder_idz<<a3poseEulerOrder_order2 | a3poseEulerOrder_idx<<a3poseEulerOrder_order3,
	a3poseEulerOrder_zxy = a3poseEulerOrder_idz<<a3poseEulerOrder_order1 | a3poseEulerOrder_idx<<a3poseEulerOrder_order2 | a3poseEulerOrder_idy<<a3poseEulerOrder_order3,
	a3poseEulerOrder_yxz = a3poseEulerOrder_idy<<a3poseEulerOrder_order1 | a3poseEulerOrder_idx<<a3poseEulerOrder_order2 | a3poseEulerOrder_idz<<a3poseEulerOrder_order3,
	a3poseEulerOrder_xzy = a3poseEulerOrder_idx<<a3poseEulerOrder_order1 | a3poseEulerOrder_idz<<a3poseEulerOrder_order2 | a3poseEulerOrder_idy<<a3poseEulerOrder_order3,
	a3poseEulerOrder_zyx = a3poseEulerOrder_idz<<a3poseEulerOrder_order1 | a3poseEulerOrder_idy<<a3poseEulerOrder_order2 | a3poseEulerOrder_idx<<a3poseEulerOrder_order3,

	a3poseEulerOrder_default = a3poseEulerOrder_zxy
};

a3real4x4r ec_eulerToMat4x4(a3real4x4p mat_out, a3vec3 eulerAngles, const a3_SpatialPoseEulerOrder order);
a3real4r ec_eulerToQuat(a3real4p quat_out, const a3vec3 eulerAngles, const a3_SpatialPoseEulerOrder order);

//-----------------------------------------------------------------------------

// flags to describe transformation components in use
// useful for constraining motion and kinematics
enum a3_SpatialPoseChannel
{
	// identity (no channels)
	a3poseChannel_none,

	// orientation
	a3poseChannel_orient_x = 0x0001,
	a3poseChannel_orient_y = 0x0002,
	a3poseChannel_orient_z = 0x0004,
	a3poseChannel_orient_xy = a3poseChannel_orient_x | a3poseChannel_orient_y,
	a3poseChannel_orient_yz = a3poseChannel_orient_y | a3poseChannel_orient_z,
	a3poseChannel_orient_zx = a3poseChannel_orient_z | a3poseChannel_orient_x,
	a3poseChannel_orient_xyz = a3poseChannel_orient_xy | a3poseChannel_orient_z,

	// scale
	a3poseChannel_scale_x = 0x0010,
	a3poseChannel_scale_y = 0x0020,
	a3poseChannel_scale_z = 0x0040,
	a3poseChannel_scale_xy = a3poseChannel_scale_x | a3poseChannel_scale_y,
	a3poseChannel_scale_yz = a3poseChannel_scale_y | a3poseChannel_scale_z,
	a3poseChannel_scale_zx = a3poseChannel_scale_z | a3poseChannel_scale_x,
	a3poseChannel_scale_xyz = a3poseChannel_scale_xy | a3poseChannel_scale_z,

	// translation
	a3poseChannel_translate_x = 0x0100,
	a3poseChannel_translate_y = 0x0200,
	a3poseChannel_translate_z = 0x0400,
	a3poseChannel_translate_xy = a3poseChannel_translate_x | a3poseChannel_translate_y,
	a3poseChannel_translate_yz = a3poseChannel_translate_y | a3poseChannel_translate_z,
	a3poseChannel_translate_zx = a3poseChannel_translate_z | a3poseChannel_translate_x,
	a3poseChannel_translate_xyz = a3poseChannel_translate_xy | a3poseChannel_translate_z,
};

	
//-----------------------------------------------------------------------------

// single pose for a single node
struct a3_SpatialPose
{
	// orientation
	a3vec3 eulerAngles;
	a3quat quatOrientation; //TODO rename back to 'orientation' once all references are fixed

	// translation vector
	a3vec3 translation;

	// scale vector
	a3vec3 scale;
};


//-----------------------------------------------------------------------------

a3i32 a3spatialPoseInit(a3_SpatialPose* spatialPose);

// set rotation values for a single node pose
a3i32 a3spatialPoseSetRotation(a3_SpatialPose* spatialPose, const a3f32 rx_degrees, const a3f32 ry_degrees, const a3f32 rz_degrees, const a3_SpatialPoseEulerOrder eulerOrder);

// scale
a3i32 a3spatialPoseSetScale(a3_SpatialPose* spatialPose, const a3f32 sx, const a3f32 sy, const a3f32 sz);

// translation
a3i32 a3spatialPoseSetTranslation(a3_SpatialPose* spatialPose, const a3f32 tx, const a3f32 ty, const a3f32 tz);


//-----------------------------------------------------------------------------

// reset single node pose
a3i32 a3spatialPoseReset(a3_SpatialPose* spatialPose);

// invert single node pose
a3_SpatialPose* a3spatialPoseInvert(a3_SpatialPose* spatialPose_inout);

// convert single node pose to matrix
a3i32 a3spatialPoseConvert(a3mat4* mat_out, const a3_SpatialPose* spatialPose_in, const a3_SpatialPoseChannel channel, const a3_SpatialPoseEulerOrder order);

// convert single matrix back to node pose
a3i32 a3spatialPoseRestore(a3_SpatialPose* spatialPose_out, const a3mat4* mat_in, const a3_SpatialPoseChannel channel, const a3_SpatialPoseEulerOrder order);

// concat two spatial poses
a3i32 a3spatialPoseConcat(a3_SpatialPose* finalPose, const a3_SpatialPose* basePose, const a3_SpatialPose* deltaPose);

// copy operation for single node pose
a3i32 a3spatialPoseCopy(a3_SpatialPose* spatialPose_out, const a3_SpatialPose* spatialPose_in);

// lerp between spatial poses
a3_SpatialPose* a3spatialPoseLerp(a3_SpatialPose* out, const a3_SpatialPose* val0, const a3_SpatialPose* val1, a3real param);

extern ec_InterpolationFuncFamily a3_SpatialPoseInterpolationInfo;

//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_SpatialPose.inl"


#endif	// !__ANIMAL3D_SPATIALPOSE_H