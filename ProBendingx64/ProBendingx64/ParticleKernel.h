#pragma once
#include "ParticleAffectorEnum.h"
#include "AffectorParameters.h"
#include <map>

//forward declare
typedef struct CUfunc_st *CUfunction;   

class ParticleAffector;
class ScaleParticleAffector;
class ColourFadeParticleAffector;

class CudaGPUData;
struct MappedGPUData;

namespace Ogre
{
	class HardwareVertexBufferSharedPtr;
};

namespace physx
{
	class PxCudaContextManager;
	class PxParticleReadData;
};

class ParticleKernel
{
public:
	enum ParticleKernelError{SUCCESS, MODULE_NOT_FOUND, FUNCTION_NOT_FOUND, ALLOCATION_ERROR, REQUIRED_AFFECTORS_NOT_FOUND};

protected:
	static const std::string KernelFilePath;
	static const std::string KernelFunctionName;

	enum DevicePointers{ValidBitmap, Lifetimes, AffectorParameterCollection, DevicePointerCount};
	enum GraphicsResourcePointers{Positions, GraphicsResourcePointerCount};
	
	CUfunction kernelFunction;//the function to run

	CudaGPUData* gpuData;//the cuda gpu data

	typedef std::map<ParticleAffectorType::ParticleAffectorType, ParticleAffector*> AffectorMap;

	ScaleParticleAffector* scaleParticleAffector;
	MappedGPUData* scaleMappedData;

	ColourFadeParticleAffector* colourFadeAffector;
	MappedGPUData* colourMappedData;

	virtual ParticleKernelError InitializeGPUData(physx::PxCudaContextManager* contextManager, 
		Ogre::HardwareVertexBufferSharedPtr positionBuffer, const unsigned int maxParticles);

	virtual GPUParamsCollection GetAffectorDevices();

	virtual void UnmapAffectors();

	virtual void FreeAndDestroyGPUAffectorMemory();

public:
	ParticleKernel(void);
	virtual ~ParticleKernel(void);

	///<summary>Populates the kernel with the necessary affector data. Should only be called once upon
	///kernel creation. Also initializes additional data</summary>
	///<param name="contextManager">The physx Cuda context manager</param>
	///<param name="positionBuffer">The Ogre Buffer that will represent positions</param>
	///<param name="maxParticles">The maximum allowed particles</param>
	///<param name="affectors">Pointer to the affectors that represent the data that the kernel should use.
	///Only uses affectors that are indicated to run on the GPU. Does not claim ownership of memory</param>
	///<returns>Kernel Error enum value. If this isn't successful, chances are that launch kernel will fail</returns>
	virtual ParticleKernelError PopulateData(physx::PxCudaContextManager* contextManager, 
		Ogre::HardwareVertexBufferSharedPtr positionBuffer, const unsigned int maxParticles, AffectorMap* affectors);

	///<summary>Launches the CUDA kernel represented by this class</summary>
	///<param name="particleData">The particle data as provided by Physx</param>
	///<param name="lifetimes">The array of lifetimes, or NULL if not using lifetimes</param>
	///<param name="maxParticles">The maximum allowed particles</param>
	///<returns>True if kernel was successfully launched, false if not</returns>
	virtual bool LaunchKernel(physx::PxParticleReadData* particleData, float* lifetimes, float initialLifetime, const unsigned int maxParticles);

	///<summary>Clones the current instance of the kernel</summary>
	///<returns>A new instance that contains the same data as this one, but does not
	///copy GPU or affector data over (basically only Kernel is copied over)</returns>
	virtual ParticleKernel* Clone();
};

