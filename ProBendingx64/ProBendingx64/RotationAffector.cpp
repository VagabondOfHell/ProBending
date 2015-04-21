#include "RotationAffector.h"
#include "ParticleSystemBase.h"

RotationAffector::RotationAffector()
	:ParticleAffector(onGPU)
{

}

RotationAffector::RotationAffector(float degreeMinRot, float degreeMaxRot, bool onGPU)
	:ParticleAffector(onGPU), affectorAttributes(degreeMinRot, degreeMaxRot)
{
}

RotationAffector::~RotationAffector(void)
{
}

bool RotationAffector::Initialize(ParticleSystemBase* owningSystem)
{
	owningSystem->CreateVertexBuffer(GetDesiredBuffer());
	return true;
}

void RotationAffector::Update(const float gameTime, GPUResourcePointers& pointers, const float percentile, const unsigned int particleIndex)
{
	if(!onGPU)//if we are on the GPU, there is no need to update from CPU
	{
		pointers.normals[particleIndex].x = affectorAttributes.maxRotation -
			(affectorAttributes.difference * percentile);
	}
}

GPUParticleAffectorParams* const RotationAffector::GetGPUParamaters()
{
	if(!onGPU)
		return NULL;

	return &affectorAttributes;
}

GPUParticleAffectorParams* const RotationAffector::GetParameters()
{
	return &affectorAttributes;
}

ParticleAffector* RotationAffector::Clone()
{
	return new RotationAffector(affectorAttributes.minRotation, affectorAttributes.maxRotation, onGPU);
}
