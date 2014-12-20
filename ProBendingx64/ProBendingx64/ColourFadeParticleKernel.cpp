#include "ColourFadeParticleKernel.h"
#include "OgreHardwareVertexBuffer.h"
#include "ParticleAffectors.h"
#include "ColourFadeParticleAffector.h"
#include "CudaModuleManager.h"
#include "CudaGPUData.h"
#include "ParticleAffectors.h"

#if _DEBUG
#include "OgreLogManager.h"
#endif

const std::string ColourFadeParticleKernel::KernelFunctionName = "UpdateColourSystem";

const std::string ColourFadeParticleKernel::KernelFilePath = "x64/Debug/KernelTest.ptx";

ColourFadeParticleKernel::ColourFadeParticleKernel(void)
{
}


ColourFadeParticleKernel::~ColourFadeParticleKernel(void)
{
}

ParticleKernel::ParticleKernelError ColourFadeParticleKernel::PopulateData(physx::PxCudaContextManager* contextManager, Ogre::HardwareVertexBufferSharedPtr positionBuffer, const unsigned int maxParticles, AffectorMap* affectors)
{
	for (AffectorMap::iterator start = affectors->begin(); start != affectors->end(); ++start)
	{
		if(start->first == ParticleAffectorType::ColourToColour)
			colourAffector = static_cast<ColourFadeParticleAffector*>(start->second);
	}

	if(gpuData) //If the gpu data already exists, break out early
		return SUCCESS;

	CudaModule* module = CudaModuleManager::GetSingleton()->LoadModule(KernelFilePath);

	if(!module)
		return MODULE_NOT_FOUND;
	else
	{
		kernelFunction = module->LoadFunction(KernelFunctionName);

		if(!kernelFunction)
			return FUNCTION_NOT_FOUND;
	}

	gpuData = new CudaGPUData(contextManager, GraphicsResourcePointers::GraphicsResourcePointerCount, DevicePointers::DevicePointerCount);

	bool gpuAllocationResult = gpuData->RegisterCudaGraphicsResource(GraphicsResourcePointers::Positions, positionBuffer);

	if(gpuAllocationResult)
		gpuAllocationResult = gpuData->AllocateGPUMemory(DevicePointers::ValidBitmap, sizeof(physx::PxU32) * (maxParticles + 31) >> 5);

	if(gpuAllocationResult)
		gpuAllocationResult = gpuData->AllocateGPUMemory(DevicePointers::Lifetimes, sizeof(float) * maxParticles);

	if(!gpuAllocationResult)//if failed, undo what we've done
	{
		delete gpuData;
		gpuData = NULL;

		return ALLOCATION_ERROR;
	}

	return SUCCESS;
}

bool ColourFadeParticleKernel::LaunchKernel(physx::PxParticleReadData* particleData, float* lifetimes, float initialLifetime, const unsigned int maxParticles)
{
	using namespace physx;

	if(!gpuData->CopyHostToDevice(DevicePointers::ValidBitmap, 
		&particleData->validParticleBitmap[0], sizeof(PxU32)*(maxParticles + 31) >> 5))
		printf("Copy Validity to GPU Failed\n");

	if(!gpuData->CopyHostToDevice(DevicePointers::Lifetimes,
		lifetimes, sizeof(float) * maxParticles))
		printf("Copy Scales to GPU Failed\n");

	CUdeviceptr src_pos_data = reinterpret_cast<CUdeviceptr>(&particleData->positionBuffer[0]);

	MappedGPUData dest_pos_data = gpuData->MapAndGetGPUDataPointer(GraphicsResourcePointers::Positions);
	MappedGPUData src_bit_data = gpuData->GetNonGraphicsResource(DevicePointers::ValidBitmap);
	MappedGPUData src_lifetimes = gpuData->GetNonGraphicsResource(DevicePointers::Lifetimes);
	
	GPUColourFaderAffectorParams params = *colourAffector->GetGPUParamaters();
	//
	//	//Ensure we have valid data
	if(dest_pos_data.isValid)
	{
		//Construct the parameters for the Kernel
		void* args[7] = {
			&dest_pos_data.devicePointer,
			&src_pos_data,
			&src_bit_data.devicePointer,
			&src_lifetimes,
			&initialLifetime,
			&params,
			&particleData->nbValidParticles
		};

		//Launch the kernel with 1 block of 512 threads. Each thread will complete (MaxParticles/512) worth of particles 
		CUresult res = cuLaunchKernel(kernelFunction, 1, 1, 1, 512, 1, 1, 0, 0, args, 0); 
		//CUresult res = CUDA_SUCCESS;
#if _DEBUG
		//If an error, log the problem
		if(res != CUDA_SUCCESS)
			Ogre::LogManager::getSingleton().getDefaultLog()->
			logMessage("Basic Particle System Cuda Kernel Launch failed", Ogre::LogMessageLevel::LML_CRITICAL);
#endif

		//Unmap the graphics resource
		gpuData->UnmapResourceFromCuda(GraphicsResourcePointers::Positions);
		colourAffector->UnmapCudaBuffers();
		return res == CUDA_SUCCESS;
	}

	return false;
}

ColourFadeParticleKernel* ColourFadeParticleKernel::Clone()
{
	ColourFadeParticleKernel* clone = new ColourFadeParticleKernel(*this);
	clone->colourAffector = NULL;
	clone->gpuData = NULL;

	return clone;
}


