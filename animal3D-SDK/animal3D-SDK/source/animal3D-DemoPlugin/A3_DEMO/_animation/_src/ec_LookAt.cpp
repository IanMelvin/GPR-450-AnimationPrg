#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "A3_DEMO/_animation/ec_LookAt.h"


void ec_LookAt(a3_HierarchyState* hierarchyState, a3vec3 targetLocation, a3byte name[a3node_nameSize]) // name should equal "Neck" for xbot
{
	a3kinematicsSolveForward(hierarchyState);

	//Getting target position in rig space
	a3vec3 targetPosition = targetLocation;
	//a3real4Real4x4Mul(characterObject.modelMatInv.m, targetPosition.v);

	//Getting character position in object space
	//a3vec3 charPosition = characterObject.position;
	//a3real4Real4x4Mul(characterObject.modelMatInv.m, charPosition.v);

	//Need 
	a3ui32 index = a3hierarchyGetNodeIndex(hierarchyState->hierarchy, name);
	a3spatialPoseRestore(&hierarchyState->objectSpace->pose[index], a3poseChannel_none, a3poseEulerOrder_xyz);
	hierarchyState->objectSpace->pose[index].translate;

	a3vec3 up;
	up.x = 0, up.y = 1, up.z = 0;

	a3mat4 invMat;

	a3real4x4MakeLookAt(hierarchyState->objectSpace->pose[index].transformMat.m, invMat.m, hierarchyState->objectSpace->pose[index].translate.v, targetPosition.v, up.v);

	a3kinematicsSolveInversePartial(hierarchyState, index, hierarchyState->hierarchy->numNodes);

	a3kinematicsSolveForwardPartial(hierarchyState, index, hierarchyState->hierarchy->numNodes);

	/* By hand way of doing the implementation
	a3vec3 z = targetPosition;
	a3real3Sub(z.v, hierarchyState->objectSpace->pose[index].translate.v);
	a3real3Normalize(z.v);

	a3vec3 x;
	a3real3CrossUnit(x.v, up.v, z.v);

	a3vec3 y;
	a3real3Cross(y.v, z.v, x.v);

	a3vec3 rotation = (x, y, z); //???*/

	//FK(Full), Solution, IK (Partial), FK (Partial)


	/*
	a3mat3 matrixTest = {
		(x.x, x.y, x.z),
		(y.x, y.y, y.z),
		(z.x, z.y, z.z)
	};

	a3mat4 matrix = {
		(x.x, x.y, x.z, -a3real3Dot(x.v, charPosition.v)),
		(y.x, y.y, y.z, -a3real3Dot(y.v, charPosition.v)),
		(z.x, z.y, z.z, -a3real3Dot(z.v, charPosition.v)),
		(0,		0,	 0,								   1)
	};*/
}
