#pragma once
#include "ParticleSystemBase.h"
#include "CudaModuleHelper.h"
#include "CudaGPUData.h"

class BasicParticleSystem :
	public ParticleSystemBase
{
protected:
	enum DevicePointers{ValidBitmap, DevicePointerCount};
	enum GraphicsResourcePointers{Positions, GraphicsResourcePointerCount};

	bool performCopyThisFrame;

	unsigned short framesPassed;
	unsigned short framesTillCopy;

	float initialLifetime;
	float* lifetimes;

	CUmodule cudaModule;
	CUfunction updateParticlesKernel;

	CudaGPUData* gpuBuffers;
	Ogre::HardwareVertexBufferSharedPtr mVertexBufferPosition;
	
	std::vector<physx::PxVec3>forces;

	void InitializeVertexBuffers();

public:
	BasicParticleSystem(AbstractParticleEmitter* _emitter, size_t _maximumParticles, 
		ParticleSystemParams& paramsStruct = ParticleSystemParams(), bool _ownEmitter = true);

	virtual ~BasicParticleSystem(void);

	///<summary>Initializes the Particle System</summary>
	virtual void InitializeParticleSystemData();

	///<summary>Provides the policy the opportunity to perform any GPU initializations</summary>
	virtual void InitializeGPUData();

	///<summary>Sets the value to indicate how many frames should pass before copying any data
	///from the CPU to the GPU. If using Cuda, this is lifetimes and valid particle bitmaps.
	///If not using Cuda, this is positions
	///The default is 0, so it updates every frame</summary>
	///<param name="frameCount">The frames that should pass before performing copy</param>
	virtual void inline SetFramesUntilCopy(const unsigned short frameCount)
	{
		framesTillCopy = frameCount;
	}

	///<summary>Callback for the policy to confirm the particles were created</summary>
	///<param name="createdCount>The amount of particles that were created</param>
	///<param name="emittedIndices>The indices that were used this creation frame</param>
	virtual void ParticlesCreated(const unsigned int createdCount, physx::PxStrideIterator<const physx::PxU32> emittedIndices);

	///<summary>Updates the policy. Any CPU focused updating should go here</summary>
	///<param name="time">The time that has passed between frames</param>
	///<param name="readData">The particle data as provided by PhysX</param>
	virtual std::vector<const physx::PxU32> UpdatePolicy(const float time, const physx::PxParticleReadData* const readData);

	///<summary>Updates the policy. Any GPU focused updating should go here. This is only called if physX 
	///is running on the GPU</summary>
	///<param name="time">The time that has passed between frames</param>
	///<param name="readData">The particle data as provided by PhysX in a GPU format</param>
	virtual void UpdatePolicyGPU(const float time, physx::PxParticleReadData* const readData);

};

