#pragma once
#include "ParticleAffectorEnum.h"
#include "foundation/PxVec4.h"
#include "foundation/PxVec3.h"

struct GPUResourcePointers
{
	physx::PxVec4* positions;
	physx::PxVec4* blendWeights;
	physx::PxVec4* blendIndices;
	physx::PxVec4* primaryColour;
	physx::PxVec4* secondaryColour;
	physx::PxVec4* uv0;

	physx::PxVec3* normals;
	physx::PxVec3* binormals;
	physx::PxVec3* tangent;

	GPUResourcePointers()
	{
		positions = NULL;
		blendWeights = NULL;
		blendIndices = NULL;
		primaryColour = NULL;
		secondaryColour = NULL;
		uv0 = NULL;

		normals = NULL;
		binormals = NULL;
		tangent = NULL;
	}
};

struct GPUParticleAffectorParams
{
	bool onGPU;
};

struct GPUScaleAffectorParams:public GPUParticleAffectorParams
{
	bool enlarge;
	float minScale;
	float maxScale;
	float scaleDiff;

	GPUScaleAffectorParams(bool _enlarge = true, float _minScale = 0.0f, float _maxScale = 0.0f)
		: enlarge(_enlarge), minScale(_minScale), maxScale(_maxScale), scaleDiff(maxScale - minScale)
	{
	}
};

struct GPUColourFaderAffectorParams: public GPUParticleAffectorParams
{
	physx::PxVec4 startColour;
	physx::PxVec4 endColour;
	physx::PxVec4 colourDifference;

	GPUColourFaderAffectorParams(physx::PxVec4 _startColour = physx::PxVec4(0.0f), 
		physx::PxVec4 _endColour = physx::PxVec4(1.0f))
		: startColour(_startColour), endColour(_endColour)
	{
		colourDifference = _startColour - _endColour;
		colourDifference.x = physx::PxAbs(colourDifference.x);
		colourDifference.y = physx::PxAbs(colourDifference.y);
		colourDifference.z = physx::PxAbs(colourDifference.z);
		colourDifference.w = physx::PxAbs(colourDifference.w);

		if(_startColour.x > _endColour.x)
			colourDifference.x = -colourDifference.x;
		if(_startColour.y > _endColour.y)
			colourDifference.y = -colourDifference.y;
		if(_startColour.z > _endColour.z)
			colourDifference.z = -colourDifference.z;
		if(_startColour.w > _endColour.w)
			colourDifference.w = -colourDifference.w;
	}
};

struct GPUParamsCollection
{
	GPUScaleAffectorParams* scaleParameters;
	GPUColourFaderAffectorParams* colourFadeParams;

	GPUParamsCollection(){
		scaleParameters = NULL;
		colourFadeParams = NULL;
	}

	~GPUParamsCollection(){}
};
