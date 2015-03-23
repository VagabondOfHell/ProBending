#include "SphereEmitter.h"

#include "RandomNumberGenerator.h"

SphereEmitter::SphereEmitter(float particlesPerSecond /*= 1.0f*/, physx::PxVec3 emitterPosition 
	/*= physx::PxVec3(0.0f)*/, float _duration /*= 1.0f*/, float minPartSpeed /*= 0.0f*/, 
	float maxPartSpeed /*= 1.0f*/, bool emitFromShell /*= false*/, float radius /*= 1.0f*/, 
	bool towardsSphereCenter /*= false*/)
	: AbstractParticleEmitter(emitterPosition, particlesPerSecond, physx::PxVec3(-1.0f, -1.0f, -1.0f).getNormalized(),
	physx::PxVec3(1.0f, 1.0f, 1.0f).getNormalized(), _duration, minPartSpeed, maxPartSpeed), 
	EmitFromShell(emitFromShell), Radius(radius), TowardsSphereCenter(towardsSphereCenter)
{

}


SphereEmitter::~SphereEmitter(void)
{
}

void SphereEmitter::Emit(const float gameTime, 
		const unsigned int availableIndiceCount, physx::PxParticleCreationData& creationData)
{
	unsigned int emissionCount(GetEmissionCount(gameTime, availableIndiceCount));

	//If we have creation data and indices can be used
	if(emissionCount > 0)
	{
		//reset forces
		forces.clear();
		positions.clear();

		RandomNumberGenerator* random = RandomNumberGenerator::GetInstance();
		
		//Generate initial force and update available indices
		for (unsigned int i = 0; i < emissionCount; i++)
		{
			float ranSpeed = random->GenerateRandom(minSpeed, maxSpeed);

			physx::PxVec3 ranDirection = physx::PxVec3(
				random->GenerateRandom(minimumDirection.x, maximumDirection.x),
				random->GenerateRandom(minimumDirection.y, maximumDirection.y),
				random->GenerateRandom(minimumDirection.z, maximumDirection.z));

			if(EmitFromShell)
				positions.push_back(position + (ranDirection * Radius));
			else
				positions.push_back(position + (ranDirection * random->GenerateRandom(0.0f, Radius)));

			if(TowardsSphereCenter)
				forces.push_back(-ranDirection * ranSpeed);
			else
				forces.push_back(ranDirection * ranSpeed);
		}

		//set creation data parameters
		creationData.numParticles = emissionCount;
		creationData.positionBuffer = physx::PxStrideIterator<physx::PxVec3>(&positions[0]);
		creationData.velocityBuffer = physx::PxStrideIterator<physx::PxVec3>(&forces[0]);
	}
}

SphereEmitter* SphereEmitter::Clone()
{
	SphereEmitter* clone = new SphereEmitter(particlesPerSecond, position, duration,
		minSpeed, maxSpeed, EmitFromShell, Radius, TowardsSphereCenter);

	return clone;
}
