#pragma once
#include "OgreSimpleRenderable.h"
#include <vector>
#include "particles\PxParticleSystem.h"
#include "CudaModuleHelper.h"
#include "CudaGPUData.h"

namespace DevicePointers{enum DevicePointers{Lifetimes, ValidBitmap, Count};};
namespace GraphicsResourcePointers{enum GraphicsResourcePointers{Positions, Count};};

class DefaultParticlePolicy: public Ogre::SimpleRenderable
{
protected:
	unsigned int particlesPerSecond;
	float particlesToEmitThisFrame;

	unsigned int maxParticles;

	bool onGPU;
	bool performCopyThisFrame;

	unsigned short framesPassed;
	unsigned short framesTillCopy;

	float initialLifetime;

	physx::PxParticleSystem* particleSystem;

	CUmodule module;
	CUfunction updateParticlesKernel;

	CudaGPUData* gpuBuffers;
	Ogre::HardwareVertexBufferSharedPtr mVertexBufferPosition;

	float* lifetimes;

	std::vector<unsigned int> availableIndices;
	std::vector<unsigned int> indicesToRemove;
	
	std::vector<physx::PxVec3>forces;
	physx::PxVec3 emitterPosition;

	void InitializeVertexBuffers();

public:
	DefaultParticlePolicy(void);
	DefaultParticlePolicy(unsigned int _particlesPerSecond);

	~DefaultParticlePolicy(void);
	
	///<summary>Initializes the Particle Policy</summary>
	///<param name="maxParticles">The maximum number of particles allowed in this particle system</param>
	///<param name="_particleSystem">The physx Particle System that we will store a reference to in order to manipulate particles</param>
	virtual void Initialize(unsigned int maxParticles, physx::PxParticleSystem* _particleSystem);

	///<summary>Provides the policy the opportunity to perform any GPU initializations</summary>
	///<param name="contextManager">The physX context manager that we will store reference to in order to utilize the GPU</param>
	virtual void InitializeGPUData(physx::PxCudaContextManager* contextManager);

	///<summary>Sets the value to indicate how many frames should pass before copying any data
	///from the CPU to the GPU. If using Cuda, this is lifetimes and valid particle bitmaps.
	///If not using Cuda, this is positions
	///The default is 0, so it updates every frame</summary>
	///<param name="frameCount">The frames that should pass before performing copy</param>
	virtual void inline SetFramesUntilCopy(unsigned short frameCount)
	{
		framesTillCopy = frameCount;
	}

	///<summary>Performs any particle emissions required this frame</summary>
	///<param name="gameTime">The gametime that has passed, in order to allow particle per second emission</param>
	///<returns>The physx particle creation data for this frame</returns>
	virtual physx::PxParticleCreationData* Emit(float gameTime);

	///<summary>Callback for the policy to confirm the particles were created</summary>
	///<param name="createdCount>The amount of particles that were created</param>
	virtual void ParticlesCreated(unsigned int createdCount);

	///<summary>Gets the vector of indices to remove from the particle system</summary>
	///<returns>The vector of indices that should be removed</returns>
	virtual std::vector<unsigned int> ReleaseParticles()
	{
		return indicesToRemove;
	}

	///<summary>Updates the policy. Any CPU focused updating should go here</summary>
	///<param name="time">The time that has passed between frames</param>
	///<param name="readData">The particle data as provided by PhysX</param>
	///<param name="readableData">The flags representing readable data provided by PhysX </param>
	virtual void UpdatePolicy(float time, physx::PxParticleReadData* readData, physx::PxParticleReadDataFlags readableData);

	///<summary>Updates the policy. Any GPU focused updating should go here. This is only called if physX 
	///is running on the GPU</summary>
	///<param name="time">The time that has passed between frames</param>
	///<param name="readData">The particle data as provided by PhysX in a GPU format</param>
	///<param name="readableData">The flags representing readable data provided by PhysX </param>
	virtual void UpdatePolicyGPU(float time, physx::PxParticleReadData* readData, physx::PxParticleReadDataFlags readableData);

	virtual Ogre::Real getBoundingRadius(void) const
	{
		return 0;
	}

	virtual Ogre::Real getSquaredViewDepth(const Ogre::Camera*)const
	{
		return 0;
	}
};

