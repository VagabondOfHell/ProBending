#pragma once
#include "FluidBase.h"

#include "PxPhysics.h"

using namespace physx;

FluidBase::FluidBase(std::shared_ptr<AbstractParticleEmitter> _emitter, size_t _maximumParticles, float _initialLifetime,
		  ParticleSystemParams& paramsStruct /*= ParticleSystemParams()*/)
		  :FluidAndParticleBase(_emitter, _maximumParticles, _initialLifetime, paramsStruct.cudaContext)
{
	pxFluidBase = PxGetPhysics().createParticleFluid(_maximumParticles, false);

	//place the system on the GPU if it should be
	pxFluidBase->setParticleBaseFlag(physx::PxParticleBaseFlag::eGPU, onGPU);

	//Check that PhysX didn't overwrite our GPU selection. If they did, reset our information
	if(!(pxFluidBase->getParticleBaseFlags() & physx::PxParticleBaseFlag::eGPU))
	{
		onGPU = false;
		cudaContextManager = NULL;
	}

	SetParticleBaseFlags(pxFluidBase, paramsStruct.baseFlags);

	//Set the gravity flag
	pxFluidBase->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !paramsStruct.useGravity);

	SetSystemData(pxFluidBase, paramsStruct);

	particleBase = pxFluidBase;
}

//Serializer Constructor
FluidBase::FluidBase(physx::PxParticleFluid* physxFluidSystem, 
		  std::shared_ptr<AbstractParticleEmitter> _emitter, size_t _maximumParticles, float _initialLifetime)
		 : FluidAndParticleBase(_emitter, _maximumParticles, _initialLifetime, NULL)
{
	//Set parent copy and this copy to the specified system
	particleBase = pxFluidBase = physxFluidSystem;

	readableData = pxFluidBase->getParticleReadDataFlags();
}

FluidBase::~FluidBase(void)
{
}
