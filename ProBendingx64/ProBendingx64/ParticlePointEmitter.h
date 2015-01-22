#pragma once
#include "AbstractParticleEmitter.h"
#include <vector>
#include <random>

class ParticlePointEmitter :
	public AbstractParticleEmitter
{
protected:
	std::vector<physx::PxVec3>forces;//The forces to apply to newly created particles

	std::random_device rd;
	std::mt19937_64 eng;
public:
	///<summary>Constructor of the abstract class to fill common values</summary>
	///<param name="emitterPosition">The position of the emitter</param>
	///<param name="minEmissionDirection">The minimum values on the X, Y, Z dimensions for random velocity </param>
	///<param name="maxEmissionDirection">The maximum values on the X, Y, Z dimensions for random velocity </param>
	///<param name="_loop">True to loop indefinetely, false if not</param>
	///<param name="_duration">How long the emitter should emit if not looping</param>
	///<param name="minParticleSpeed">The minimum value for creating random particle speed </param>
	///<param name="maxParticleSpeed">The maximum value for creating random particle speed </param>
	ParticlePointEmitter(float particlesPerSecond = 1.0f, physx::PxVec3 emitterPosition = physx::PxVec3(0.0f), 
		physx::PxVec3 minEmissionDirection = physx::PxVec3(0.0f), physx::PxVec3 maxEmissionDirection  = physx::PxVec3(0.0f),
		bool _loop = true, float _duration = 1.0f, float minParticleSpeed = 1.0f, float maxParticleSpeed = 2.0f);

	ParticlePointEmitter(const ParticlePointEmitter& emitter);

	virtual ~ParticlePointEmitter(void);

	virtual ParticleEmitterType GetEmitterType(){return AbstractParticleEmitter::POINT_EMITTER;}
	
	///<summary>Emission method called by a particle system</summary>
	///<param name="gameTime">The game time that has passed </param>
	///<param name="availableIndiceCount">The count of currently available indices </summary>
	///<param name="creationData">The creation data to fill. This is given from the particle system</summary>
	virtual void Emit(const float gameTime, const unsigned int availableIndiceCount, physx::PxParticleCreationData& creationData);

};

