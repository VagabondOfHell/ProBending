#pragma once
#include "FluidAndParticleBase.h"

class FluidBase: public FluidAndParticleBase
{
	friend class SceneSerializer;

protected:
	physx::PxParticleFluid* pxFluidBase;

public:
	//Serializer Constructor
	FluidBase(physx::PxParticleFluid* physxFluidSystem, 
		std::shared_ptr<AbstractParticleEmitter> _emitter, size_t _maximumParticles, float _initialLifetime);

	FluidBase(std::shared_ptr<AbstractParticleEmitter> _emitter, size_t _maximumParticles, float _initialLifetime,
		ParticleSystemParams& paramsStruct = ParticleSystemParams());

	virtual ~FluidBase(void);

	virtual FluidBase* Clone();

};

