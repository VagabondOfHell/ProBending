#include "ParticlePointEmitter.h"

using namespace physx;

ParticlePointEmitter::ParticlePointEmitter(float _particlesPerSecond, physx::PxVec3 emitterPosition, 
		physx::PxVec3 minEmissionDirection, physx::PxVec3 maxEmissionDirection, float minParticleSpeed, float maxParticleSpeed )
		: AbstractParticleEmitter(emitterPosition, minEmissionDirection, maxEmissionDirection, minParticleSpeed, maxParticleSpeed)
{
	particlesPerSecond = _particlesPerSecond;
	particlesToEmitThisFrame = 0.0f;

	eng = std::mt19937_64(rd());
}

ParticlePointEmitter::~ParticlePointEmitter(void)
{
}

void ParticlePointEmitter::Emit(float gameTime, unsigned int availableIndiceCount, physx::PxParticleCreationData& creationData)
{
	unsigned int emissionCount(0);

	//Check amount of particles available
	if(availableIndiceCount > 0)
	{
		//Calculate the number of particles to emit this frame
		particlesToEmitThisFrame += particlesPerSecond * gameTime;
		emissionCount = PxFloor(particlesToEmitThisFrame);
		particlesToEmitThisFrame -= emissionCount;

		//Gather available indices
		if(availableIndiceCount < emissionCount)
			emissionCount = availableIndiceCount;
	}

	//If we have creation data and indices can be used
	if(emissionCount > 0)
	{
		//reset forces
		forces.clear();

		//Generate initial force and update available indices
		for (unsigned int i = 0; i < emissionCount; i++)
		{
			std::uniform_real_distribution<float> x(minimumDirection.x, maximumDirection.x); 
			std::uniform_real_distribution<float> y(minimumDirection.y, maximumDirection.y); 
			std::uniform_real_distribution<float> z(minimumDirection.z, maximumDirection.z); 
			
			std::uniform_real_distribution<float> speed(minSpeed, maxSpeed);

			forces.push_back(PxVec3(x(eng), y(eng), z(eng)) * speed(eng));
		}
		
		//set creation data parameters
		creationData.numParticles = emissionCount;
		creationData.positionBuffer = physx::PxStrideIterator<PxVec3>(&position, 0);
		creationData.velocityBuffer = PxStrideIterator<PxVec3>(&forces[0]);
	}
}