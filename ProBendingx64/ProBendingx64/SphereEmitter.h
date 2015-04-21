#pragma once
#include "AbstractParticleEmitter.h"

class SphereEmitter :
	public AbstractParticleEmitter
{
protected:
	std::vector<physx::PxVec3> positions;

public:
	bool EmitFromShell;
	float Radius;
	bool TowardsSphereCenter;

	SphereEmitter(float particlesPerSecond = 1.0f, physx::PxVec3 emitterPosition = physx::PxVec3(0.0f), 
		float _duration = 1.0f, float minPartSpeed = 0.0f, float maxPartSpeed = 1.0f,
		bool emitFromShell = false, float radius = 1.0f, bool towardsSphereCenter = false);

	virtual ~SphereEmitter(void);

	virtual inline ParticleEmitterType GetEmitterType(){return AbstractParticleEmitter::SPHERE_EMITTER;}

	virtual void Emit(const float gameTime, 
		const unsigned int availableIndiceCount, physx::PxParticleCreationData& creationData);

	virtual SphereEmitter* Clone();

};

