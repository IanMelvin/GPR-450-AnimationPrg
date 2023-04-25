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
#include "A3_DEMO/_a3_demo_utilities/a3_DemoSceneObject.h"

void ec_LookAt(a3_DemoSceneObject characterObject, a3_DemoSceneObject targetObject, a3_Hierarchy* hierarchyCharacter, a3byte name[a3node_nameSize]);


#endif