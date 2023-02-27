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
	
	a3_DemoMode0_Starter-idle-update.c
	Demo mode implementations: starter scene.

	********************************************
	*** UPDATE FOR STARTER SCENE MODE        ***
	********************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoMode0_Starter.h"

//typedef struct a3_DemoState a3_DemoState;
#include "../a3_DemoState.h"

#include "../_a3_demo_utilities/a3_DemoMacros.h"


//-----------------------------------------------------------------------------
// UPDATE

void a3demo_update_objects(a3_DemoState* demoState, a3f64 const dt,
	a3_DemoSceneObject* sceneObjectBase, a3ui32 count, a3boolean useZYX, a3boolean applyScale);
void a3demo_update_defaultAnimation(a3_DemoState* demoState, a3f64 const dt,
	a3_DemoSceneObject* sceneObjectBase, a3ui32 count, a3ui32 axis);
void a3demo_update_bindSkybox(a3_DemoSceneObject* obj_camera, a3_DemoSceneObject* obj_skybox);
void a3demo_update_pointLight(a3_DemoSceneObject* obj_camera, a3_DemoPointLight* pointLightBase, a3ui32 count);

void a3demo_applyScale_internal(a3_DemoSceneObject* sceneObject, a3real4x4p s);


//Ian Added This
#include <stdio.h>
#include <malloc.h>
a3real slowModeValue = .1f;
a3boolean runWhenISaySo = a3true;

a3_TextureAtlas testAtlas;

//RSC MOD
ec_InterpolationFuncFamily floatInterpolateFuncs = {
	sizeof(a3real),
	a3realLerpWrapper,
	0,
	0
};

void InitilizeCode(a3_DemoState const* demoState)
{
	printf("test");

	a3_KeyframePool* keyframes = malloc(sizeof(a3_KeyframePool));
	a3keyframePoolCreate(keyframes, 12, &floatInterpolateFuncs);

	a3real* testKeyframeValues = malloc(sizeof(a3real) * 12);
	for (int i = 0; i < 12; ++i)
	{
		testKeyframeValues[i] = (a3real)i;
		a3keyframeInit(&keyframes->keyframe[i], .5f, &testKeyframeValues[i]);
	}

	a3_ClipPool* clips = malloc(sizeof(a3_ClipPool));

	a3clipPoolCreate(clips, 2);
	a3clipInit(&clips->clip[0], "Entry", 1);
	a3keyframeChannelInit(clips->clip[0].channels, "Value", keyframes, 0, 1);
	a3clipCalculateDuration(&clips->clip[0]);
	clips->clip[0].forwardTransition.flags = EC_TERMINUSACTION_FORWARD; clips->clip[0].forwardTransition.targetClipID = 1;
	clips->clip[0].reverseTransition.flags = EC_TERMINUSACTION_REVERSE; clips->clip[0].reverseTransition.targetClipID = 1;

	a3clipInit(&clips->clip[1], "Ping-pong", 1);
	a3keyframeChannelInit(clips->clip[1].channels, "Value", keyframes, 1, 11);
	a3clipCalculateDuration(&clips->clip[1]);
	clips->clip[1].forwardTransition.flags = EC_TERMINUSACTION_REVERSE;
	clips->clip[1].reverseTransition.flags = EC_TERMINUSACTION_FORWARD;
	
	a3clipControllerInit((a3_ClipController*)&demoState->testAnimator, "Test Animator", clips, 0);
	ec_clipController_preparePlayheads((a3_ClipController*)&demoState->testAnimator, &clips->clip[0]);
	runWhenISaySo = a3false;
}

void a3starter_update(a3_DemoState* demoState, a3_DemoMode0_Starter* demoMode, a3f64 const dt)
{
	//TEMP TESTING
	a3real modifiedDT = (a3real)dt;

	if (demoState->initializeCode && runWhenISaySo)
	{
		InitilizeCode(demoState);
	}

	if (demoState->toggleSlowMode)
	{
		modifiedDT = (a3real)dt * slowModeValue;
	}
	else 
	{
		modifiedDT = (a3real)dt;
	}

	if (demoState->togglePlay && !runWhenISaySo)
	{
		//Run Update
		a3clipControllerUpdate(&demoState->testAnimator, modifiedDT);

		//get current index
		a3real v;
		ec_clipController_evaluateValue(&v, &demoState->testAnimator, a3clipGetChannelID(ec_clipController_getClip(&demoState->testAnimator), "Value"));
		demoState->index = (a3ui32)v;
	}

	if (demoState->reset)
	{
		a3clipPoolRelease(demoState->testAnimator.clipPool);
		demoState->reset = a3false;
		demoState->togglePlay = a3true;
		demoState->toggleSlowMode = a3false;
		demoState->initializeCode = a3false;
		runWhenISaySo = a3true;
	}
	

	a3ui32 i;
	a3_DemoModelMatrixStack matrixStack[starterMaxCount_sceneObject];

	// active camera
	a3_DemoProjector const* activeCamera = demoMode->projector + demoMode->activeCamera;
	a3_DemoSceneObject const* activeCameraObject = activeCamera->sceneObject;

	// temp scale mat
	a3mat4 scaleMat = a3mat4_identity;

	a3demo_update_objects(demoState, dt,
		demoMode->object_scene, starterMaxCount_sceneObject, 0, 0);
	a3demo_update_objects(demoState, dt,
		demoMode->object_camera, starterMaxCount_cameraObject, 1, 0);

	a3demo_updateProjectorViewProjectionMat(demoMode->proj_camera_main);

	a3demo_update_defaultAnimation(demoState, dt, demoMode->obj_box, 6, 2);

	// apply scales to objects
	for (i = 0; i < starterMaxCount_sceneObject; ++i)
		a3demo_applyScale_internal(demoMode->object_scene + i, scaleMat.m);

	// update skybox
	a3demo_update_bindSkybox(demoMode->obj_camera_main, demoMode->obj_skybox);

	// update matrix stack data
	for (i = 0; i < starterMaxCount_sceneObject; ++i)
	{
		a3demo_updateModelMatrixStack(matrixStack + i,
			activeCamera->projectionMat.m, activeCameraObject->modelMat.m, activeCameraObject->modelMatInv.m,
			demoMode->object_scene[i].modelMat.m, a3mat4_identity.m);
	}
}


//-----------------------------------------------------------------------------
