#pragma once
#include "cuda.h"
#include "particles\PxParticleSystem.h"

///This outlines the blueprint for the Particle Data policy
///It is meant to hold additional particle data, as PhysX holds the other data for us
class DefaultParticleData
{
private:

	size_t validParticleCount;

public:

	DefaultParticleData(void)
	{
		validParticleCount = 0;
	}

	~DefaultParticleData(void)
	{
	}

	inline size_t GetValidParticleCount()
	{
		return validParticleCount;
	}

	void UpdateGPUAttributes(float time, physx::PxParticleReadData* readData, physx::PxParticleReadDataFlags readableData)
	{
		
	}

	void UpdateAttributes(float time, physx::PxParticleReadData* readData, physx::PxParticleReadDataFlags readableData, bool onGPU = false)
	{
		if(readData)
		{
			validParticleCount = readData->nbValidParticles;
		}
	}

};

