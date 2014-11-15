#pragma once
#include "OgreSimpleRenderable.h"
#include <vector>
#include "particles\PxParticleSystem.h"
#include "CudaModuleHelper.h"
#include "CudaGPUData.h"

namespace DevicePointers{enum DevicePointers{Lifetimes, Bitmaps, Count};};
namespace GraphicsResourcePointers{enum GraphicsResourcePointers{Positions, Count};};

class DefaultParticlePolicy: public Ogre::SimpleRenderable
{
	
private:
	unsigned int particlesPerFrame;
	unsigned int maxParticles;
	int frames;
	physx::PxParticleSystem* particleSystem;

	CUmodule module;
	CUfunction updateParticlesKernel;

	CudaGPUData* gpuBuffers;

	float* lifetimes;

	std::vector<unsigned int> availableIndices;
	std::vector<unsigned int> indicesToRemove;
	std::vector<unsigned int> indicesEmitted;
	
	std::vector<physx::PxVec3>forces;
	physx::PxVec3 emitterPosition;

public:
	DefaultParticlePolicy(void);
	DefaultParticlePolicy(unsigned int _particlesPerFrame);

	~DefaultParticlePolicy(void);
	
	void Initialize(unsigned int maxParticles, physx::PxParticleSystem* _particleSystem);
	void InitializeGPUData(physx::PxCudaContextManager* contextManager);

	physx::PxParticleCreationData* Emit();

	///<summary>Gets the vector of indices to remove from the particle system</summary>
	///<returns>The vector of indices that should be removed</returns>
	std::vector<unsigned int> ReleaseParticles()
	{
		return indicesToRemove;
	}

	void UpdatePolicy(float time, physx::PxParticleReadData* readData, physx::PxParticleReadDataFlags readableData);

	void UpdatePolicyGPU(float time, physx::PxParticleReadData* readData, physx::PxParticleReadDataFlags readableData);

	Ogre::Real getBoundingRadius(void) const
	{
		return 0;
	}

	Ogre::Real getSquaredViewDepth(const Ogre::Camera*)const
	{
		return 0;
	}
};

