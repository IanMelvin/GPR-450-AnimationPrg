#include "A3_DEMO/_animation/ec_IK.h"

#include <stddef.h>

a3i32 ec_applyEffector_triangle(ec_IKEffector const* effector, a3_SpatialPose* poses_out, const a3_SpatialPose* posesRef_in, a3_Hierarchy const* hierarchy);

a3i32 ec_applyEffector_lookAt(ec_IKEffector const* effector, a3_SpatialPose* poses_out, const a3_SpatialPose* posesRef_in, a3_Hierarchy const* hierarchy);

a3i32 ec_applyEffector(ec_IKEffector const* effector, a3_SpatialPose* poses_out, const a3_SpatialPose* posesRef_in, a3_Hierarchy const* hierarchy)
{
	if(effector && !poses_out && posesRef_in && hierarchy)
	{
		switch(effector->type)
		{
			case IK_LOOK_AT:
				ec_applyEffector_lookAt(effector, poses_out, posesRef_in, hierarchy);
				return 1;
				break;
			case IK_TRIANGLE:
				ec_applyEffector_triangle(effector, poses_out, posesRef_in, hierarchy);
				return 1;
				break;
			case IK_NO_OP:
				return 1;
				break;
			default:
				return 1;
				break;
		}
	}
	return -1;
}
a3i32 ec_applyEffector_triangle(ec_IKEffector const* effector, a3_SpatialPose* poses_out, const a3_SpatialPose* posesRef_in, a3_Hierarchy const* hierarchy)
{
	//////// Solve positions ////////
	
	a3vec3 constraintDisplacement; //"c" in slides
	a3real3Diff(constraintDisplacement.v, effector->data.triangle.bendHint.v, posesRef_in[effector->workingRoot].translate.v);

	a3vec3 effectorDisplacement; //"d" (vector) in slides
	a3real3Diff(effectorDisplacement.v, effector->data.triangle.endTarget.v, posesRef_in[effector->workingRoot].translate.v);

	a3vec3 planeNormal; //"n_plane" in slides
	a3real3CrossUnit(planeNormal.v, effectorDisplacement.v, constraintDisplacement.v);

	a3vec3 h; //No idea what this actually means, but the slides say it's important
	a3real3CrossUnit(h.v, planeNormal.v, effectorDisplacement.v); //Technically slides say to normalize first, but this is more performant

	a3real L1 = a3real3Length(constraintDisplacement.v);
	a3vec3 L2v; a3real3Diff(L2v.v, effector->data.triangle.bendHint.v, effector->data.triangle.endTarget.v);
	a3real L2 = a3real3Length(L2v.v);
	a3real B = a3real3Length(effectorDisplacement.v);
	a3real s = ( B + L1 + L2 )/2;
	a3real A = a3sqrt( s*(s-B)*(s-L1)*(s-L2) );
	a3real H = 2*A/B;

	a3real D = a3sqrt( L1*L1 - H*H );

	a3vec3 Hh = h; a3real3MulS(Hh.v, H);
	a3vec3 Dd = effectorDisplacement; a3real3MulS(Dd.v, D);
	a3_SpatialPose* elbow_out = &poses_out[effector->data.triangle.elbowID]; //"p" in slides
	a3real3SetReal4(elbow_out->translate.v, posesRef_in[effector->workingRoot].translate.v);
	a3real3Add(elbow_out->translate.v, Dd.v);
	a3real3Add(elbow_out->translate.v, Hh.v);

	//Frenet-serret frames

	a3vec4 t0; a3real4Diff(t0.v, elbow_out->translate.v, posesRef_in[effector->workingRoot].translate.v);
	a3vec4 n0 = posesRef_in[effector->workingRoot].transformMat.v1;
	a3real4x4MakeLookAt(poses_out[effector->workingRoot].transformMat.m, NULL,
		posesRef_in[effector->workingRoot].translate.v, elbow_out->translate.v, n0.v);

	a3vec3 t1; a3real3Diff(t1.v, effector->data.triangle.endTarget.v, elbow_out->translate.v);
	a3vec4 n1 = posesRef_in[effector->data.triangle.elbowID].transformMat.v1;
	a3real4x4MakeLookAt(poses_out[effector->data.triangle.elbowID].transformMat.m, NULL,
		elbow_out->translate.v, effector->data.triangle.endTarget.v, n1.v);

	return 1;
}

a3i32 ec_applyEffector_lookAt(ec_IKEffector const* effector, a3_SpatialPose* poses_out, const a3_SpatialPose* posesRef_in, a3_Hierarchy const* hierarchy) //Check if Robert thinks I need last 2 param in array
{
	a3vec3 targetPosition = effector->data.lookAt.target;

	//Restores the position, rotation, and scale values from the transform matrix
	a3ui32 index = effector->data.lookAt.neckID;
	a3spatialPoseRestore(&poses_out[index], a3poseChannel_none, a3poseEulerOrder_xyz);

	//Define up
	a3vec3 up;
	up.x = 0, up.y = 1, up.z = 0;

	//Define storageVar
	a3mat4 invMat;

	a3real4x4MakeLookAt(poses_out[index].transformMat.m, invMat.m, poses_out[index].translate.v, targetPosition.v, up.v);

	return 1;
	/* 
	* FK(Full), Solution, IK(Partial), FK(Partial)
	* 
	* 1. a3kinematicsSolveForward(hierarchyState);
	* 
	* 2. this function
	* 
	* 3. a3kinematicsSolveInversePartial(hierarchyState, index, hierarchyState->hierarchy->numNodes);
	* 
	* 4. a3kinematicsSolveForwardPartial(hierarchyState, index, hierarchyState->hierarchy->numNodes);
	*/	
}
