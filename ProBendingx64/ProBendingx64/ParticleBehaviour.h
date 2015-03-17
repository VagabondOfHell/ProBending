#pragma once
#include "particles/PxParticleReadData.h"

class FluidAndParticleBase;

class ParticleBehaviour
{
public:

	ParticleBehaviour(void)
	{
	}

	virtual ~ParticleBehaviour(void)
	{
	}

	virtual void ApplyToParticle(FluidAndParticleBase* base, 
		physx::PxU32 particleIndex, const physx::PxParticleReadData* readData,
		float gameTime, float particleLifetime) = 0;
};

