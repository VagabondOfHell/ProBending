#pragma once
#include "ParticleAffectorEnum.h"
#include "foundation/PxVec4.h"

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
	physx::PxVec4* scales;

	GPUScaleAffectorParams(bool _enlarge = true, float _minScale = 0.0f, float _maxScale = 0.0f, physx::PxVec4* _scales = 0)
		: enlarge(_enlarge), minScale(_minScale), maxScale(_maxScale), scaleDiff(maxScale - minScale), scales(_scales)
	{
	}
};

struct GPUColourFaderAffectorParams: public GPUParticleAffectorParams
{
	physx::PxVec4* colours;//used to hold the CUDevicePtr after H to D copy or to hold the Ogre Array after buffer lock
	physx::PxVec4 startColour;
	physx::PxVec4 endColour;
	physx::PxVec4 colourDifference;

	GPUColourFaderAffectorParams(physx::PxVec4 _startColour = physx::PxVec4(0.0f), 
		physx::PxVec4 _endColour = physx::PxVec4(1.0f), physx::PxVec4* _colours = 0)
		: startColour(_startColour), endColour(_endColour), colours(_colours)
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
