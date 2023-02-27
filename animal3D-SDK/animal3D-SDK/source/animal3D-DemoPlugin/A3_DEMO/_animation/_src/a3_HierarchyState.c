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
	
	a3_HierarchyState.c
	Implementation of transform hierarchy state.
*/

#include "../a3_HierarchyState.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define fileLineMaxLength ((size_t)256)

//-----------------------------------------------------------------------------

// initialize pose set given an initialized hierarchy and key pose count
a3i32 a3hierarchyPoseGroupCreate(a3_HierarchyPoseGroup *poseGroup_out, const a3_Hierarchy *hierarchy, const a3ui32 poseCount)
{
	// validate params and initialization states
	//	(output is not yet initialized, hierarchy is initialized)
	if (poseGroup_out && hierarchy && !poseGroup_out->hierarchy && hierarchy->nodes)
	{
		// determine memory requirements

		// allocate everything (one malloc)
		//??? = (...)malloc(sz);

		// set pointers
		poseGroup_out->hierarchy = hierarchy;
		poseGroup_out->poseCount = poseCount;
		poseGroup_out->spatialPoseCount = poseCount * poseGroup_out->hierarchy->numNodes;
		poseGroup_out->spatialPosePool = malloc(sizeof(a3_SpatialPose)*poseGroup_out->spatialPoseCount);

		// reset all data
		for (a3ui32 i = 0; i < poseGroup_out->spatialPoseCount; i++)
		{
			a3spatialPoseInit(&poseGroup_out->spatialPosePool[i]);
		}

		for (a3ui32 i = 0; i < poseCount; i++)
		{
			//FIXME @rsc - I don't remember what this does but the program seems to run fine without it
			//poseGroup_out->hierarchalPoses[i].spatialPose = poseGroup_out->spatialPosePool;
		}

		// done
		return 1;
	}
	return -1;
}

// release pose set
a3i32 a3hierarchyPoseGroupRelease(a3_HierarchyPoseGroup *poseGroup)
{
	// validate param exists and is initialized
	if (poseGroup && poseGroup->hierarchy)
	{
		// release everything (one free)
		//free(???);
		free(poseGroup->hierarchy->nodes);
		free(poseGroup->spatialPosePool);

		// reset pointers
		poseGroup->hierarchy = 0;

		// done
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// initialize hierarchy state given an initialized hierarchy
a3i32 a3hierarchyStateCreate(a3_HierarchyState *state_out, const a3_Hierarchy *hierarchy)
{
	// validate params and initialization states
	//	(output is not yet initialized, hierarchy is initialized)
	if (state_out && hierarchy && !state_out->hierarchy && hierarchy->nodes)
	{
		// determine memory requirements

		// allocate everything (one malloc)
		//??? = (...)malloc(sz);
		//state_out->hierarchy = (a3_Hierarchy*)malloc(sizeof(a3_Hierarchy));

		// set pointers
		state_out->hierarchy = hierarchy;
		state_out->eulerOrder = a3poseEulerOrder_zxy;

		state_out->sampledDeltaPose = NULL; //Set externally
		state_out->bindPose         = NULL; //Set externally
		state_out->samplePose       = calloc(hierarchy->numNodes, sizeof(a3_SpatialPose       ));
		state_out->localPose        = calloc(hierarchy->numNodes, sizeof(a3_HierarchyPose     ));
		state_out->objectPose       = calloc(hierarchy->numNodes, sizeof(a3_HierarchyPose     ));
		state_out->channels         = calloc(hierarchy->numNodes, sizeof(a3_SpatialPoseChannel));

		// reset all data to defaults
		for (a3ui32 i = 0; i < hierarchy->numNodes; i++)
		{
			a3spatialPoseInit  (&state_out->samplePose[i]);
			state_out->channels[i] = ~0;
			//state_out->hierarchy->nodes[i] = ;
		}
		a3hierarchyPoseInit(state_out-> localPose, hierarchy->numNodes);
		a3hierarchyPoseInit(state_out->objectPose, hierarchy->numNodes);

		// done
		return 1;
	}
	return -1;
}

// release hierarchy state
a3i32 a3hierarchyStateRelease(a3_HierarchyState *state)
{
	// validate param exists and is initialized
	if (state && state->hierarchy)
	{
		// release everything (one free)
		//free(???);
		free(state->hierarchy->nodes);
		free(state->samplePose);
		free(state->localPose);
		free(state->objectPose);
		free(state->channels);

		// reset pointers
		state->hierarchy = 0;

		// done
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// load HTR file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadHTR(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath)
{
	if (poseGroup_out && !poseGroup_out->poseCount && hierarchy_out && !hierarchy_out->numNodes && resourceFilePath && *resourceFilePath)
	{
		//Open File
		a3_FileStream inStream = {0};
		a3ret isValid = a3fileStreamOpenRead(&inStream, resourceFilePath);
		assert(isValid == a3true);
		
		a3ui16 readIndex = 0;
		
		while (!feof(inStream.stream))
		{
			isValid = ec_specialCaseChecker(&inStream);
			if (isValid == 1)
			{
				ec_checkHeader(&inStream);
			}
			else {
				//ec_skipLine(&inStream);
			}
		}
		
		a3fileStreamClose(&inStream);
	}
	return -1;
}

// load BVH file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadBVH(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath)
{
	if (poseGroup_out && !poseGroup_out->poseCount && hierarchy_out && !hierarchy_out->numNodes && resourceFilePath && *resourceFilePath)
	{
		printf("Why is this being called???? (Source: a3HierarchyState.c, Function: a3hierarchyPoseGroupLoadBVH)");
	}
	return -1;
}

a3i32 ec_specialCaseChecker(a3_FileStream const* inStream)
{
	char c = fgetc(inStream->stream);
	if (c != '#' && c != '[')
	{
		//ec_skipLine(inStream);
		return 2;
	}
	else if (c == '#')
	{
		ec_skipLine(inStream);
		//printf("Comment\n");
		return 0;
	}
	else if (c == '[')
	{
		ungetc(c, inStream->stream);
		return 1;
	}
	return -1;
}

a3i32 ec_checkHeader(const a3_FileStream* inStream)
{
	a3ui32 numSkipped = 0;
	a3ui32 output = 0;
	
	char header[fileLineMaxLength];
	header[fileLineMaxLength - 1] = '\0';

	output = fscanf(inStream->stream, "%s", header);
	printf("Header: %s \n", header);
	ec_skipLine(inStream);

	if (strncmp(header, "[Header]", strlen("[Header]")) == 0)
	{
		//printf("Same");
		char stringValue [fileLineMaxLength];
		stringValue[fileLineMaxLength - 1] = '\0';
		a3ui32 intValue = 0;
		
		while (!feof(inStream->stream)) 
		{
			output = fscanf(inStream->stream, "%s", stringValue);
			if (strncmp(stringValue, "NumSegments", strlen("NumSegments")) == 0)
			{
				output = fscanf(inStream->stream, "%2d", &intValue);
				printf("- NS = %d \n", intValue);
			}
			else if (strncmp(stringValue, "NumFrames", strlen("NumFrames")) == 0)
			{
				output = fscanf(inStream->stream, "%2d", &intValue);
				printf("- NF = %d \n", intValue);
			}
			else if (strncmp(stringValue, "EulerRotationOrder", strlen("EulerRotationOrder")) == 0)
			{
				output = fscanf(inStream->stream, "%s", &stringValue);
				printf("- ERO: %s \n", stringValue);
				break;
			}
		}
	}
	else if (strncmp(header, "[SegmentNames&Hierarchy]", strlen("[SegmentNames&Hierarchy]")) == 0)
	{
		
		char objName[fileLineMaxLength] = { ' ' };
		char objNameParent[fileLineMaxLength];
		objName[fileLineMaxLength - 1] = '\0';
		objNameParent[fileLineMaxLength - 1] = '\0';

		while (!feof(inStream->stream))
		{
			output = ec_specialCaseChecker(inStream);
			if (output == 2)
			{
				output = fscanf(inStream->stream, "%s", &objName);
				if (objName[0] == '[')
				{
					strcpy(header, objName);
					printf("Header: %s \n", header);
					ec_skipLine(inStream);
					break;
				}
				else if (strncmp(objName, "ain", strlen("ain")) == 0)
				{
					strcpy(objName, "main");
				}
				output = fscanf(inStream->stream, "%s", &objNameParent);
				printf("Object: %s, Parent: %s \n", objName, objNameParent);
			}
		}
	}
	
	if (strncmp(header, "[BasePosition]", strlen("[BasePosition]")) == 0)
	{
		char objName[fileLineMaxLength] = { ' ' };
		objName[fileLineMaxLength - 1] = '\0';
		a3vec3 transform;
		a3vec4 rotation;
		a3real boneLength;

		while (!feof(inStream->stream))
		{
			output = ec_specialCaseChecker(inStream);
			if (output == 2)
			{
				output = fscanf(inStream->stream, "%s", &objName);
				if (objName[0] == '[')
				{
					strcpy(header, objName);
					printf("Header: %s \n", header);
					break;
				}
				else if (strncmp(objName, "ain", strlen("ain")) == 0)
				{
					strcpy(objName, "main");
				}
				output = fscanf(inStream->stream, "%f", &transform.x);
				output = fscanf(inStream->stream, "%f", &transform.y);
				output = fscanf(inStream->stream, "%f", &transform.z);
				output = fscanf(inStream->stream, "%f", &rotation.x);
				output = fscanf(inStream->stream, "%f", &rotation.y);
				output = fscanf(inStream->stream, "%f", &rotation.z);
				output = fscanf(inStream->stream, "%f", &boneLength);

				printf("Name: %s, Transform: %f %f %f, Rotation: %f %f %f, Bone: %f \n", objName, transform.x, transform.y, transform.z, rotation.x, rotation.y, rotation.z, boneLength);
			}
		}
	}

	return 0;
}

a3i32 ec_skipLine(a3_FileStream const* inStream)
{
	a3ui32 numSkipped = 0;

	while (!feof(inStream->stream) && fgetc(inStream->stream) != '\n')
	{
		numSkipped++;
	}

	return numSkipped;

}

a3i32 ec_parceFile(char* bufferOut[fileLineMaxLength], const a3_FileStream* inStream)
{

	return -1;
}


//-----------------------------------------------------------------------------
