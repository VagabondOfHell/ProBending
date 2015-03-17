#include "ParticleMoveToPoint.h"

#include "FluidAndParticleBase.h"

ParticleMoveToPoint::ParticleMoveToPoint(void)
{
}


ParticleMoveToPoint::~ParticleMoveToPoint(void)
{
}

void ParticleMoveToPoint::ApplyToParticle(FluidAndParticleBase* particleBase,
										  physx::PxU32 particleIndex, const physx::PxParticleReadData* readData,
										  float gameTime, float particleLifetime)
{
	physx::PxVec3 dir = TargetPosition -  readData->positionBuffer[particleIndex];
	dir.normalize();

	particleBase->ApplyForce(particleIndex, dir * Speed, physx::PxForceMode::eFORCE);
}
