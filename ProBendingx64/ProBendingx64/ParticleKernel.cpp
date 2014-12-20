#include "ParticleKernel.h"
#include "CudaGPUData.h"
#include "ParticleAffectors.h"
#include "ColourFadeParticleAffector.h"
#include "CudaModuleManager.h"

#if _DEBUG
#include "OgreLogManager.h"
#endif

const std::string ParticleKernel::KernelFunctionName = "UpdateParticleSystem";

const std::string ParticleKernel::KernelFilePath = "x64/Debug/KernelTest.ptx";

ParticleKernel::ParticleKernel(void)
{
	gpuData = NULL;
	colourMappedData = NULL;
	scaleMappedData = NULL;
}

ParticleKernel::~ParticleKernel(void)
{
	//we dont release the affectors because we dont claim ownership of the memory

	//Free the Mapped Data instances
	FreeAndDestroyGPUAffectorMemory();

	if(gpuData)
	{
		//Does this work if they arent registered?
		gpuData->UnregisterAllGraphicsResources();
		gpuData->FreeAllGPUMemory();
		delete gpuData;
		gpuData = NULL;
	}
}

ParticleKernel::ParticleKernelError ParticleKernel::PopulateData(physx::PxCudaContextManager* contextManager, Ogre::HardwareVertexBufferSharedPtr positionBuffer, const unsigned int maxParticles, AffectorMap* affectors)
{
	if(gpuData) //If the gpu data already exists, break out early
		return SUCCESS;
	
	for (AffectorMap::iterator start = affectors->begin(); start != affectors->end(); ++start)
	{
		CUresult allocationResult = CUDA_SUCCESS;

		switch (start->first)
		{
		case ParticleAffectorType::ColourToColour:
			colourMappedData = new MappedGPUData();
			allocationResult = CudaGPUData::AllocateGPUMemory(*colourMappedData, sizeof(GPUColourFaderAffectorParams));
			if(allocationResult == CUDA_SUCCESS)//if memory allocated successfully, add this affector
				colourFadeAffector = static_cast<ColourFadeParticleAffector*>(start->second);
			break;

		case ParticleAffectorType::Scale:
			scaleMappedData = new MappedGPUData();
			allocationResult = CudaGPUData::AllocateGPUMemory(*scaleMappedData, sizeof(GPUScaleAffectorParams));
			if(allocationResult == CUDA_SUCCESS)//if memory allocated successfully, add this affector
				scaleParticleAffector = static_cast<ScaleParticleAffector*>(start->second);
			break;

		default:
			break;
		}			
	}

	return InitializeGPUData(contextManager, positionBuffer, maxParticles);
}

ParticleKernel::ParticleKernelError ParticleKernel::InitializeGPUData(physx::PxCudaContextManager* contextManager, 
								Ogre::HardwareVertexBufferSharedPtr positionBuffer, const unsigned int maxParticles)
{
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

	bool gpuAllocationResult = true;// = gpuData->RegisterCudaGraphicsResource(GraphicsResourcePointers::Positions, positionBuffer);

	if(gpuAllocationResult)
		gpuAllocationResult = gpuData->AllocateGPUMemory(DevicePointers::ValidBitmap, sizeof(physx::PxU32) * (maxParticles + 31) >> 5);

	if(gpuAllocationResult)
		gpuAllocationResult = gpuData->AllocateGPUMemory(DevicePointers::Lifetimes, sizeof(float) * maxParticles);

	if(gpuAllocationResult)
		gpuAllocationResult = gpuData->AllocateGPUMemory(DevicePointers::AffectorParameterCollection, sizeof(GPUParamsCollection));

	if(!gpuAllocationResult)//if failed, undo what we've done
	{
		gpuData->FreeAllGPUMemory();
		delete gpuData;
		gpuData = NULL;

		return ALLOCATION_ERROR;
	}

	return SUCCESS;
}

bool ParticleKernel::LaunchKernel(physx::PxParticleReadData* particleData, float* lifetimes, float initialLifetime, const unsigned int maxParticles)
{
	using namespace physx;

	if(!gpuData->CopyHostToDevice(DevicePointers::ValidBitmap, 
		&particleData->validParticleBitmap[0], sizeof(PxU32)*(maxParticles + 31) >> 5))
		printf("Copy Validity to GPU Failed\n");

	if(!gpuData->CopyHostToDevice(DevicePointers::Lifetimes,
		lifetimes, sizeof(float) * maxParticles))
		printf("Copy Scales to GPU Failed\n");

	GPUParamsCollection devicePointerCollection = GetAffectorDevices();

	if(!gpuData->CopyHostToDevice(DevicePointers::AffectorParameterCollection, &devicePointerCollection))
		printf("Copy Affector Parameters to GPU Failed\n");

	CUdeviceptr src_pos_data = reinterpret_cast<CUdeviceptr>(&particleData->positionBuffer[0]);

	MappedGPUData dest_pos_data = gpuData->MapAndGetGPUDataPointer(GraphicsResourcePointers::Positions);
	MappedGPUData src_bit_data = gpuData->GetNonGraphicsResource(DevicePointers::ValidBitmap);
	MappedGPUData src_lifetimes = gpuData->GetNonGraphicsResource(DevicePointers::Lifetimes);

	MappedGPUData src_affector_data = gpuData->GetNonGraphicsResource(DevicePointers::AffectorParameterCollection);

	//Ensure we have valid data
	if(dest_pos_data.isValid)
	{
		//Construct the parameters for the Kernel
		void* args[7] = {
			&dest_pos_data.devicePointer,
			&src_pos_data,
			&src_bit_data.devicePointer,
			&src_lifetimes,
			&initialLifetime,
			&src_affector_data.devicePointer,
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
		
		UnmapAffectors();

		return res == CUDA_SUCCESS;
	}

	return false;
}


void ParticleKernel::UnmapAffectors()
{
	if(colourFadeAffector)
		colourFadeAffector->UnmapCudaBuffers();

	if(scaleParticleAffector)
		scaleParticleAffector->UnmapCudaBuffers();
}

GPUParamsCollection ParticleKernel::GetAffectorDevices()
{
	GPUParamsCollection devPtrCollection = GPUParamsCollection();

	CUresult copyResult = CUDA_SUCCESS;

	if(colourMappedData)
	{
		copyResult = CudaGPUData::CopyHostToDevice(*colourMappedData, colourFadeAffector->GetGPUParamaters());
		if(copyResult == CUDA_SUCCESS)
			devPtrCollection.colourFadeParams = reinterpret_cast<GPUColourFaderAffectorParams*>(colourMappedData->devicePointer);
	}

	if(scaleMappedData)
	{
		copyResult = CudaGPUData::CopyHostToDevice(*scaleMappedData, scaleParticleAffector->GetGPUParamaters());
		if(copyResult == CUDA_SUCCESS)
			devPtrCollection.scaleParameters = reinterpret_cast<GPUScaleAffectorParams*>(scaleMappedData->devicePointer);
	}
	
	return devPtrCollection;
}

ParticleKernel* ParticleKernel::Clone()
{
	ParticleKernel* clone = new ParticleKernel(*this);
	clone->colourFadeAffector = NULL;
	clone->scaleParticleAffector = NULL;
	clone->gpuData = NULL;

	return clone;
}

void ParticleKernel::FreeAndDestroyGPUAffectorMemory()
{
	if(colourMappedData)
	{
		CudaGPUData::FreeGPUMemory(*colourMappedData);
		delete colourMappedData;
	}

	if(scaleMappedData)
	{
		CudaGPUData::FreeGPUMemory(*scaleMappedData);
		delete scaleMappedData;
	}
}
