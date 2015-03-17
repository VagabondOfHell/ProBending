#pragma once
#include "particlebehaviour.h"

#include "OgreMath.h"
#include "foundation/PxVec3.h"
#include <vector>

class ParticleOrbitPoint :
	public ParticleBehaviour
{
private:
	Ogre::Radian rotationPerSecond;

	std::vector<Ogre::Radian> perParticleRadians;

public:
	float Radius;
	physx::PxVec3 Pivot;

	ParticleOrbitPoint(void);
	ParticleOrbitPoint(float Radius, float rotationPerSecond);

	virtual ~ParticleOrbitPoint(void);

	inline void SetParticlePerSecond(float degrees)
	{
		rotationPerSecond = degrees;
	}

	virtual void ApplyToParticle(FluidAndParticleBase* base, physx::PxU32 particleIndex,
		const physx::PxParticleReadData* readData,
		float gameTime, float particleLifetime);

};

