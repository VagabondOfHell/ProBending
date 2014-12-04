#pragma once
#include "AbstractParticleEmitter.h"
#include <vector>
#include <random>

class ParticlePointEmitter :
	public AbstractParticleEmitter
{
private:
	float particlesPerSecond; //Amount of particles to launch per second. Can be a fractional number to take longer than a second
	float particlesToEmitThisFrame;//Number of particles to emit this frame

	std::vector<physx::PxVec3>forces;//The forces to apply to newly created particles

	std::random_device rd;
	std::mt19937_64 eng;
public:
	///<summary>Constructor of the particle point emitter, which emits particles from a central point</summary>
	///<param name="particlesPerSecond">The number of particles to emit per second. If below 0, it is flipped to positive</param>
	///<param name="emitterPosition">Position of the emitter</param>
	///<param name="minEmissionDirection">The minimum values on the X, Y, Z dimensions for random velocity </param>
	///<param name="maxEmissionDirection">The maximum values on the X, Y, Z dimensions for random velocity </param>
	///<param name="minParticleSpeed">The minimum value for creating random particle speed </param>
	///<param name="maxParticleSpeed">The maximum value for creating random particle speed </param>
	ParticlePointEmitter(float particlesPerSecond = 1.0f, physx::PxVec3 emitterPosition = physx::PxVec3(0.0f), 
		physx::PxVec3 minEmissionDirection = physx::PxVec3(0.0f), physx::PxVec3 maxEmissionDirection  = physx::PxVec3(0.0f),
		float minParticleSpeed = 1.0f, float maxParticleSpeed = 2.0f);

	virtual ~ParticlePointEmitter(void);

	///<summary>Emission method called by a particle system</summary>
	///<param name="gameTime">The game time that has passed </param>
	///<param name="availableIndiceCount">The count of currently available indices </summary>
	///<param name="creationData">The creation data to fill. This is given from the particle system</summary>
	virtual void Emit(const float gameTime, const unsigned int availableIndiceCount, physx::PxParticleCreationData& creationData);
};

