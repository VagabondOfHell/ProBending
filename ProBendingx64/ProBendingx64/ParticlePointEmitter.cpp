#include "ParticlePointEmitter.h"
#include "RandomNumberGenerator.h"

using namespace physx;

ParticlePointEmitter::ParticlePointEmitter(float _particlesPerSecond, physx::PxVec3 emitterPosition, 
		 physx::PxVec3 minEmissionDirection, physx::PxVec3 maxEmissionDirection, float _duration, 
		 float minParticleSpeed, float maxParticleSpeed )
		: AbstractParticleEmitter(emitterPosition, _particlesPerSecond, minEmissionDirection, 
		maxEmissionDirection, _duration, minParticleSpeed, maxParticleSpeed)
{
}

ParticlePointEmitter::ParticlePointEmitter(const ParticlePointEmitter& emitter)
	: AbstractParticleEmitter(emitter)
{
}

ParticlePointEmitter::~ParticlePointEmitter(void)
{
}

void ParticlePointEmitter::Emit(const float gameTime, const unsigned int availableIndiceCount, physx::PxParticleCreationData& creationData)
{
	unsigned int emissionCount(0);

	bool infiniteEmission = duration <= 0.0f;

	if(!infiniteEmission)
		timePassed += gameTime;
	
	//if time passed has not exceeded system duration or there should be infinite emission
	if((timePassed <= duration && !infiniteEmission) || (infiniteEmission))
	{
		//Check amount of particles available
		if(availableIndiceCount > 0)
		{
			//Calculate the number of particles to emit this frame
			particlesToEmitThisFrame += particlesPerSecond * gameTime;
			emissionCount = (unsigned int)PxFloor(particlesToEmitThisFrame);
			particlesToEmitThisFrame -= emissionCount;

			//Gather available indices
			if(availableIndiceCount < emissionCount)
				emissionCount = availableIndiceCount;
		}
	}

	//If we have creation data and indices can be used
	if(emissionCount > 0)
	{
		//reset forces
		forces.clear();

		//Generate initial force and update available indices
		for (unsigned int i = 0; i < emissionCount; i++)
		{
			RandomNumberGenerator* random = RandomNumberGenerator::GetInstance();

			float ranSpeed = random->GenerateRandom(minSpeed, maxSpeed);

			forces.push_back(PxVec3(
				random->GenerateRandom(minimumDirection.x, maximumDirection.x),
				random->GenerateRandom(minimumDirection.y, maximumDirection.y),
				random->GenerateRandom(minimumDirection.z, maximumDirection.z)) * ranSpeed);
		}
		
		//set creation data parameters
		creationData.numParticles = emissionCount;
		creationData.positionBuffer = physx::PxStrideIterator<PxVec3>(&position, 0);
		creationData.velocityBuffer = PxStrideIterator<PxVec3>(&forces[0]);
	}
}

ParticlePointEmitter* ParticlePointEmitter::Clone()
{
	ParticlePointEmitter* clone = new ParticlePointEmitter(particlesPerSecond, position, 
		minimumDirection, maximumDirection, duration, minSpeed, maxSpeed);

	return clone;
}
