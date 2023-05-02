/*
	Team Evistential Crisis
	Robert Christensen & Ian Melvin

	ec_IK.h
	Inverse kinematics data structures
*/

#include <animal3D-A3DM/animal3D-A3DM.h>

#include "a3_Hierarchy.h"
#include "a3_HierarchyState.h"
#include "a3_SpatialPose.h"

typedef enum ec_IKEffectorType	ec_IKEffectorType;
typedef struct ec_IKEffector	ec_IKEffector;

enum ec_IKEffectorType
{
	IK_NO_OP = 0,
	IK_LOOK_AT,
	IK_TRIANGLE,
};

struct ec_IKEffector
{
	a3i32 workingRoot; //ID on skeleton

	ec_IKEffectorType type;
	union {

		struct {
			a3vec3 target; //World space
			a3i32 neckID; //ID on skeleton
		} lookAt;
			
		struct {
			a3vec3 bendHint; //World space
			a3vec3 endTarget; //World space
			a3i32 elbowID; //ID on skeleton
			a3i32 endID; //ID on skeleton
		} triangle;

	} data;
};


a3i32 ec_applyEffector(ec_IKEffector const* effector, a3_SpatialPose* poses_out, const a3_SpatialPose* posesRef_in, a3_Hierarchy const* hierarchy);
