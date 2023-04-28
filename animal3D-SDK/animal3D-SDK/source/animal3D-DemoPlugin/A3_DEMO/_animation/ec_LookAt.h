/*
	Team Evistential Crisis
	Robert Christensen & Ian Melvin

	ec_LookAt.h
	LookAtCode
*/

#ifndef EC_LOOK_AT_H
#define EC_LOOK_AT_H

#include "A3_DEMO/_animation/a3_HierarchyState.h"
#include "A3_DEMO/_animation/ec_Interpolation.h"
#include "A3_DEMO/_animation/a3_Kinematics.h"
#include "A3_DEMO/_a3_demo_utilities/a3_DemoSceneObject.h"

void ec_LookAt(a3_HierarchyState* hierarchyState, a3vec3 targetObject, a3byte name[a3node_nameSize]);


#endif