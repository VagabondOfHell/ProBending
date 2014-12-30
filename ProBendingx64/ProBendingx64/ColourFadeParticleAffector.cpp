#include "ColourFadeParticleAffector.h"
#include "OgreHardwareBufferManager.h"
#include "CudaGPUData.h"
#include "ParticleSystemBase.h"

ColourFadeParticleAffector::ColourFadeParticleAffector()
	:ParticleAffector()
{

}

ColourFadeParticleAffector::ColourFadeParticleAffector(physx::PxVec4 startColour, physx::PxVec4 endColour, bool onGPU)
	:ParticleAffector(onGPU), affectorAttributes(startColour, endColour)
{
}

ColourFadeParticleAffector::~ColourFadeParticleAffector(void)
{
}

ParticleAffectorType::ParticleAffectorType ColourFadeParticleAffector::GetAffectorType()
{
	return ParticleAffectorType::ColourToColour;
}

bool ColourFadeParticleAffector::Initialize(ParticleSystemBase* owningSystem)
{
	owningSystem->CreateVertexBuffer(GetDesiredBuffer());
	return true;
}

void ColourFadeParticleAffector::Update(const float gameTime, GPUResourcePointers& pointers, const float percentile, const unsigned int particleIndex)
{
	if(!onGPU)//if we are on the GPU, there is no need to update from CPU
	{
		physx::PxVec4 newColour =  affectorAttributes.endColour - (affectorAttributes.colourDifference * percentile);
		pointers.primaryColour[particleIndex].x = newColour.x;
		pointers.primaryColour[particleIndex].y = newColour.y;
		pointers.primaryColour[particleIndex].z = newColour.z;
	}
}

GPUColourFaderAffectorParams* const ColourFadeParticleAffector::GetGPUParamaters()
{
	if(!onGPU)
		return NULL;

	return &affectorAttributes;
}

