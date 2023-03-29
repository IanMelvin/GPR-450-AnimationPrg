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
	
	a3_SpatialPose.c
	Implementation of spatial pose.
*/

#include "../a3_SpatialPose.h"
#include "../ec_PolymorphicData.h"

//-----------------------------------------------------------------------------

ec_InterpolationFuncFamily a3_SpatialPoseInterpolationInfo = { //FIXME this is order-dependent
	sizeof(a3_SpatialPose),
	a3spatialPoseLerp,
	0,
	0
};

a3_SpatialPose* a3spatialPoseInvert(a3_SpatialPose* spatialPose_inout)
{
	if (spatialPose_inout)
	{
		vtable_vec3Additive      .invert(&spatialPose_inout->translation    );
		vtable_vec3Additive      .invert(&spatialPose_inout->eulerAngles    );
		//vtable_quat              .invert(&spatialPose_inout->quatOrientation);
		vtable_vec3Multiplicative.invert(&spatialPose_inout->scale          );
		//vtable_mat4              .invert(&spatialPose_inout->transform      );
	}
	return spatialPose_inout;
}

//-----------------------------------------------------------------------------
