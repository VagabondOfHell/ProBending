#pragma once
#include "OgreSimpleRenderable.h"
#include "ParticleSystemParams.h"

#include <map>

class AbstractParticleEmitter;
class ParticleAffector;
class ParticleKernel;

///Base class shared between Fluid and Particle System to be inherited from

class FluidAndParticleBase
{
protected:
	physx::PxParticleReadDataFlags readableData; ///The readable data of the particle system

	std::shared_ptr<AbstractParticleEmitter> emitter;//The emitter used to create particles

	std::vector<physx::PxU32> availableIndices; //The available indices within the particle system

	std::vector<const physx::PxU32> indicesToRemove;//Vector of indices that should be removed this frame

	physx::PxCudaContextManager* cudaContextManager; ///The physX cuda context manager

	Ogre::HardwareVertexBufferSharedPtr mVertexBufferPosition;//The ogre GL buffer to hold the positions

	size_t maximumParticles; ///The maximum number of particles allowed in the particle system
	bool onGPU; ///Whether or not the particle system is used on the GPU
	ParticleKernel* cudaKernel; //Kernel used to update particles through cuda

	//Map of buffers organized by semantic, holding the buffer reference
	typedef std::map<Ogre::VertexElementSemantic, Ogre::HardwareVertexBufferSharedPtr> BufferMap;
	typedef std::pair<BufferMap::iterator, bool> BufferMapInsertResult;

	BufferMap bufferMap;//list of buffers organized by semantic 

	float initialLifetime;//Starting lifetime for particles
	float* lifetimes;//array of lietimes for particles

	///<summary>Sets the read data flags as specified by the newFlags argument</summary>
	///<param name="newFlags">The new read data flags to use</param>
	void SetParticleReadFlags(physx::PxParticleBase* pxParticleSystem, physx::PxParticleReadDataFlags newFlags);

	///<summary>Sets the base flags as specified by the newFlags argument. Does not handle the GPU flag</summary>
	///<param name="newFlags">The new base flags to use</param>
	void SetParticleBaseFlags(physx::PxParticleBase* pxParticleSystem, physx::PxParticleBaseFlags newFlags);

	void SetSystemData(physx::PxParticleBase* pxParticleSystem, const ParticleSystemParams& params);

public:
	FluidAndParticleBase(std::shared_ptr<AbstractParticleEmitter> _emitter, size_t _maximumParticles, float _initialLifetime,
		physx::PxCudaContextManager* _cudaMan)
		: emitter(_emitter), maximumParticles(_maximumParticles), initialLifetime(_initialLifetime), cudaContextManager(_cudaMan)
	{

	}

	virtual ~FluidAndParticleBase()
	{

	}
};