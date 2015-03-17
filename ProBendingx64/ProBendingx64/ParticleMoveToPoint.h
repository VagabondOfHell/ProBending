#pragma once
#include "ParticleBehaviour.h"

#include "foundation/PxVec3.h"

class ParticleMoveToPoint : public ParticleBehaviour
{
public:
	float Speed;
	physx::PxVec3 TargetPosition;

	ParticleMoveToPoint(void);
	virtual ~ParticleMoveToPoint(void);

	virtual void ApplyToParticle(FluidAndParticleBase* particleBase,
		physx::PxU32 particleIndex, const physx::PxParticleReadData* readData,
		float gameTime, float particleLifetime);

};

