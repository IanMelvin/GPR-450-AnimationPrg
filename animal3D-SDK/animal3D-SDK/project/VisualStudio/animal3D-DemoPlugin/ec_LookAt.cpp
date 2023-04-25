#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "ec_LookAt.h"


void ec_LookAt(a3_HierarchyState* hierarchyState, a3_DemoSceneObject characterObject, a3_DemoSceneObject targetObject, a3byte name[a3node_nameSize]) // name should equal "Neck" for xbot
{
	a3kinematicsSolveForward(hierarchyState);

	//Getting target position in rig space
	a3vec3 targetPosition = targetObject.position;
	a3real4Real4x4Mul(characterObject.modelMatInv.m, targetPosition.v);

	//Getting character position in object space
	a3vec3 charPosition = characterObject.position;
	a3real4Real4x4Mul(characterObject.modelMatInv.m, charPosition.v);

	//Need 
	a3ui32 index = a3hierarchyGetNodeIndex(hierarchyState->hierarchy, name);
	

	a3vec3 z = targetPosition;
	a3real3Sub(z.v, charPosition.v);
	a3real3Normalize(z.v);

	a3vec3 x;
	a3real3CrossUnit(x.v, a3vec3_y.v, z.v);

	a3vec3 y;
	a3real3Cross(y.v, z.v, x.v);

	a3vec3 rotation = (x, y, z); //???


	a3kinematicsSolveInverse(hierarchyState);
	//Need to do IK for blend tree to get local space info
	//	Local = World Space * parent Inv World Space

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
