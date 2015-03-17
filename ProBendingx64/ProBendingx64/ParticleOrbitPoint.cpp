#include "ParticleOrbitPoint.h"

#include "FluidAndParticleBase.h"
#include "HelperFunctions.h"

ParticleOrbitPoint::ParticleOrbitPoint(void)
{
}

ParticleOrbitPoint::ParticleOrbitPoint(float radius, float _rotationPerSecond)
{
	Radius = radius;
	SetParticlePerSecond(_rotationPerSecond);
}


ParticleOrbitPoint::~ParticleOrbitPoint(void)
{
}

void ParticleOrbitPoint::ApplyToParticle(FluidAndParticleBase* base, 
										 physx::PxU32 particleIndex, const physx::PxParticleReadData* readData,
										 float gameTime, float particleLifetime)
{
	//if(perParticleRadians.size() == 0)
	//{
	//	for (int i = 0; i < base->GetMaximumParticlesAllowed(); i++)
	//	{
	//		perParticleRadians.push_back(Ogre::Radian(0.0f));
	//	}
	//}

	//physx::PxVec3 partPosition = readData->positionBuffer[particleIndex];
	//	
	//perParticleRadians[particleIndex] += rotationPerSecond * gameTime;

	//Ogre::Degree fullCircle = Ogre::Degree(360.0f);

	//if(perParticleRadians[particleIndex] >= fullCircle)
	//	perParticleRadians[particleIndex] -= fullCircle;

	////elevation angle as well

	//float cosRot = Ogre::Math::Cos(perParticleRadians[particleIndex].valueRadians());
	//float sinRot = Ogre::Math::Sin(perParticleRadians[particleIndex].valueRadians());

	////Second cos and sin in x and y should be elevation angle
	//physx::PxVec3 dir;
	//dir.x = Radius * sinRot * cosRot;
	//dir.y = Radius * cosRot * sinRot;
	//dir.z = Radius * cosRot;

	//if(particleIndex == 0)
	//	printf("%f, %f, %f\n", dir.x, dir.y, dir.z);
	physx::PxVec3 diff = readData->positionBuffer[particleIndex] - Pivot;
	physx::PxQuat quat = physx::PxQuat(30, diff.getNormalized());
	//HelperFunctions::FromEulerAngles(physx::PxVec3(20.0f, 20.0f, 20.0f));

	physx::PxVec3 forceDir = quat.rotate(diff);


	base->ApplyForce(particleIndex, forceDir.getNormalized() * 3.0f, physx::PxForceMode::eFORCE);
}
