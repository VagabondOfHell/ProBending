#include "ParticleAffectors.h"
#include "OgreHardwareBufferManager.h"
#include "CudaGPUData.h"
#include "foundation/PxVec2.h"
#include "ParticleSystemBase.h"

ParticleAffector::~ParticleAffector()
{
}

#pragma region Lifetime Scale Affector
ScaleParticleAffector::ScaleParticleAffector()
	: ParticleAffector(), affectorAttributes()
{
}

ScaleParticleAffector::ScaleParticleAffector(const bool _enlarge, const float _minScale, const float _maxScale,  bool onGPU)
	:ParticleAffector(onGPU), affectorAttributes(_enlarge, _minScale, _maxScale)
{
	//Verify scales
	if(_minScale > _maxScale)
	{
		affectorAttributes.maxScale = _minScale;
		affectorAttributes.minScale = _maxScale;

		affectorAttributes.scaleDiff = affectorAttributes.maxScale - affectorAttributes.minScale;
	}
}

ScaleParticleAffector::~ScaleParticleAffector()
{
}

ParticleAffectorType::ParticleAffectorType ScaleParticleAffector::GetAffectorType()
{
	return ParticleAffectorType::Scale;
}

bool ScaleParticleAffector::Initialize(ParticleSystemBase* owningSystem)
{
	return true;
}

void ScaleParticleAffector::Update(const float gameTime, GPUResourcePointers& pointers, const float percentile, const unsigned int particleIndex)
{
	if(!onGPU)//if we are on the GPU, there is no need to update from CPU
	{
		if(affectorAttributes.enlarge)//Adjust for enlargement
			pointers.positions[particleIndex].w = affectorAttributes.maxScale - (affectorAttributes.scaleDiff * percentile);
		else//Adjust for shrinkage
			pointers.positions[particleIndex].w = affectorAttributes.minScale + (affectorAttributes.scaleDiff * percentile);
	}
}

GPUScaleAffectorParams* const ScaleParticleAffector::GetGPUParamaters()
{
	if(!onGPU)
		return NULL;

	return &affectorAttributes;
}

#pragma endregion

