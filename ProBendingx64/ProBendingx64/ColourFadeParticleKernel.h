#pragma once
#include "particlekernel.h"

class ColourFadeParticleAffector;

class ColourFadeParticleKernel :
	public ParticleKernel
{
protected:
	ColourFadeParticleAffector* colourAffector;

	static const std::string KernelFilePath;
	static const std::string KernelFunctionName;

	enum DevicePointers{ValidBitmap, Lifetimes, DevicePointerCount};
	enum GraphicsResourcePointers{Positions, GraphicsResourcePointerCount};

public:
	ColourFadeParticleKernel(void);
	virtual ~ColourFadeParticleKernel(void);

	virtual ParticleKernelError PopulateData(physx::PxCudaContextManager* contextManager, Ogre::HardwareVertexBufferSharedPtr positionBuffer, const unsigned int maxParticles, AffectorMap* affectors);

	virtual bool LaunchKernel(physx::PxParticleReadData* particleData, float* lifetimes, float initialLifetime, const unsigned int maxParticles);

	virtual ColourFadeParticleKernel* Clone();

};

