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
				ec_checkHeader(&inStream, poseGroup_out, hierarchy_out);
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

a3i32 ec_peakNextInput(a3_FileStream const* inStream)
{
	char c = fgetc(inStream->stream);
	if (c != '#' && c != '[')
	{
		ungetc(c, inStream->stream);
		return 2;
	}
	else if (c == '#')
	{
		ec_skipLine(inStream);
		return 0;
	}
	else if (c == '[')
	{
		ungetc(c, inStream->stream);
		return 1;
	}
	return -1;
}

a3i32 ec_checkHeader(const a3_FileStream* inStream, a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out)
{
	//Define Variables
	a3ui32 numSkipped = 0;
	a3ui32 output = 0;
	
	char header[fileLineMaxLength];
	header[fileLineMaxLength - 1] = '\0';

	//Get header and skip comment line
	output = fscanf(inStream->stream, "%s", header);
	printf("Header: %s \n", header);
	ec_skipLine(inStream);

	if (strncmp(header, "[Header]", strlen("[Header]")) == 0) //Check if header is header header
	{
		//printf("Same");
		char stringValue [fileLineMaxLength];
		stringValue[fileLineMaxLength - 1] = '\0';
		a3ui32 intValue = 0;
		
		while (!feof(inStream->stream)) //Loop while not at end of file
		{
			output = fscanf(inStream->stream, "%s", stringValue); //Get next string value from file
			if (strncmp(stringValue, "NumSegments", strlen("NumSegments")) == 0) //Check if value is number of segments
			{
				//Get data from file and print to console
				output = fscanf(inStream->stream, "%2d", &intValue);
				printf("- NS = %d \n", intValue);

				//Set up hierarchy
				hierarchy_out->numNodes = intValue;
				a3hierarchyCreate(hierarchy_out, hierarchy_out->numNodes, NULL);
			}
			else if (strncmp(stringValue, "NumFrames", strlen("NumFrames")) == 0) //Check if value is number of frames
			{
				//Get data from file and print to console
				output = fscanf(inStream->stream, "%2d", &intValue);
				printf("- NF = %d \n", intValue);
				poseGroup_out->poseCount = intValue;
			}
			else if (strncmp(stringValue, "EulerRotationOrder", strlen("EulerRotationOrder")) == 0) //Check if value is euler rotation order
			{
				//Get data from file and print to console
				output = fscanf(inStream->stream, "%s", &stringValue);
				printf("- ERO: %s \n", stringValue);
				return 0;
			}
		}
	}
	else if (strncmp(header, "[SegmentNames&Hierarchy]", strlen("[SegmentNames&Hierarchy]")) == 0) //Check if header corresponds to segements and hierarchy
	{
		//Define Variables
		char objName[fileLineMaxLength] = { ' ' };
		char objNameParent[fileLineMaxLength];
		objName[fileLineMaxLength - 1] = '\0';
		objNameParent[fileLineMaxLength - 1] = '\0';
		a3ui32 index = 0;

		while (!feof(inStream->stream)) //Loop while not at end of file
		{
			output = ec_peakNextInput(inStream);
			if (output == 2) // Check if line is non header / non comment
			{
				output = fscanf(inStream->stream, "%s", &objName);
				if (objName[0] == '[') //Check for header, if so print name and skip comment on following line
				{
					strcpy(header, objName);
					printf("Header: %s \n", header);
					ec_skipLine(inStream);
					break;
				}
				else //Parse, store, and print data for everyother line
				{
					output = fscanf(inStream->stream, "%s", &objNameParent);
					printf("Object: %s, Parent: %s \n", objName, objNameParent);
					a3hierarchySetNode(hierarchy_out, index, a3hierarchyGetNodeIndex(hierarchy_out, objNameParent), objName);
				}

				//Increment
				index++;
			}
		}

		//Defines Hierarchy values
		poseGroup_out->hierarchy = hierarchy_out;
		poseGroup_out->spatialPoseCount = poseGroup_out->poseCount * hierarchy_out->numNodes;
		const a3ui32 dataSize = sizeof(a3_SpatialPose) * poseGroup_out->spatialPoseCount;
		poseGroup_out->spatialPosePool = (a3_SpatialPose*)malloc(dataSize);
		poseGroup_out->hierarchalPoses = (a3_HierarchyPose*)malloc(sizeof(a3_SpatialPose)* poseGroup_out->poseCount);

	}
	
	if (strncmp(header, "[BasePosition]", strlen("[BasePosition]")) == 0) //Check if header relates to base position
	{
		//Define Variables
		char objName[fileLineMaxLength] = { ' ' };
		objName[fileLineMaxLength - 1] = '\0';
		a3vec3 transform;
		a3vec4 rotation;
		a3real boneLength;
		a3ui32 index = 0;

		while (!feof(inStream->stream)) //Loop while not at end of file
		{
			output = ec_peakNextInput(inStream);
			if (output == 2) // Check if line is non header / non comment
			{
				output = fscanf(inStream->stream, "%s", &objName);
				if (objName[0] == '[') //Check for header
				{
					strcpy(header, objName);
					printf("Header: %s \n", header);
					ec_skipLine(inStream);
					break;
				}

				//Pull values from file
				output = fscanf(inStream->stream, "%f", &transform.x);
				output = fscanf(inStream->stream, "%f", &transform.y);
				output = fscanf(inStream->stream, "%f", &transform.z);
				output = fscanf(inStream->stream, "%f", &rotation.x);
				output = fscanf(inStream->stream, "%f", &rotation.y);
				output = fscanf(inStream->stream, "%f", &rotation.z);
				output = fscanf(inStream->stream, "%f", &boneLength);
				a3mat4 matrix = { transform.x, transform.y, transform.z,0,
								rotation.x, rotation.y, rotation.z,0,
								boneLength, boneLength, boneLength,0,
								0,0,0,0};

				//Set hierarchalPose matrix
				poseGroup_out->hierarchalPoses[index].transform = matrix;

				//Print to console
				printf("Name: %s, Transform: %f %f %f, Rotation: %f %f %f, Bone: %f \n", objName, transform.x, transform.y, transform.z, rotation.x, rotation.y, rotation.z, boneLength);
			}
		}
	}

	if(strchr(header, '[') != NULL) //Check if header exists
	{
		//Skip Line
		ec_skipLine(inStream);

		//Define Variables
		a3ui32 frameNumber = 0;
		a3vec3 transform;
		a3vec3 rotation;
		a3vec3 scale;
		a3real boneScaleFactor;
		a3ui32 index = 0;

		while(!feof(inStream->stream) && frameNumber < poseGroup_out->poseCount - 1) //Loop while not at end of file and frame count is less then the pose count
		{
			//Pull Data
			output = fscanf(inStream->stream, "%2d", &frameNumber);
			output = fscanf(inStream->stream, "%f", &transform.x);
			output = fscanf(inStream->stream, "%f", &transform.y);
			output = fscanf(inStream->stream, "%f", &transform.z);
			output = fscanf(inStream->stream, "%f", &rotation.x);
			output = fscanf(inStream->stream, "%f", &rotation.y);
			output = fscanf(inStream->stream, "%f", &rotation.z);
			output = fscanf(inStream->stream, "%f", &boneScaleFactor);

			//Set scale vector
			scale.x = boneScaleFactor;
			scale.y = boneScaleFactor;
			scale.z = boneScaleFactor;

			//Set spacial pose values
			poseGroup_out->spatialPosePool[index].translation = transform;
			poseGroup_out->spatialPosePool[index].orientation = rotation;
			poseGroup_out->spatialPosePool[index].scale = scale;

			//Increment
			index++;

			//Print to console
			printf("Frame: %d, Transform: %f %f %f, Rotation: %f %f %f, Bone: %f \n", frameNumber, transform.x, transform.y, transform.z, rotation.x, rotation.y, rotation.z, boneScaleFactor);
		}
	}
	return 0;
}

a3i32 ec_skipLine(a3_FileStream const* inStream)
{
	a3ui32 numSkipped = 0;

	//Loop through and get more characters from file i untill end of line symbol is found
	while (!feof(inStream->stream) && fgetc(inStream->stream) != '\n')
	{
		numSkipped++;
	}

	//return number of characters skipped
	return numSkipped;

}


//-----------------------------------------------------------------------------
