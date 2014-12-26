#include "ParticleKernel.h"
#include "CudaGPUData.h"
#include "ParticleAffectors.h"
#include "ColourFadeParticleAffector.h"
#include "ParticleSystemBase.h"
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

ParticleKernel::ParticleKernelError ParticleKernel::PopulateData(ParticleSystemBase* particleSystem, AffectorMap* affectors)
{
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

	gpuData = new CudaGPUData(particleSystem->GetPhysXCudaManager(), 
		GraphicsResourcePointers::GraphicsResourcePointerCount, DevicePointers::DevicePointerCount);

	bool gpuAllocationResult = RegisterBufferResource(Positions, particleSystem->GetBuffer(Ogre::VES_POSITION));

	size_t maxParticles = particleSystem->GetMaximumParticlesAllowed();

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

	for (AffectorMap::iterator start = affectors->begin(); start != affectors->end(); ++start)
	{
		PrepareAffectorData(particleSystem, start);
	}

	return SUCCESS;
}

void ParticleKernel::PrepareAffectorData(ParticleSystemBase* particleSystem, AffectorMap::iterator affector)
{
	CUresult allocationResult = CUDA_SUCCESS;

	switch (affector->first)
	{
	case ParticleAffectorType::ColourToColour:
		colourMappedData = new MappedGPUData();
		allocationResult = CudaGPUData::AllocateGPUMemory(*colourMappedData, sizeof(GPUColourFaderAffectorParams));
		if(allocationResult == CUDA_SUCCESS)//if memory allocated successfully, add this affector
			colourFadeAffector = static_cast<ColourFadeParticleAffector*>(affector->second);
		break;

	case ParticleAffectorType::Scale:
		scaleMappedData = new MappedGPUData();
		allocationResult = CudaGPUData::AllocateGPUMemory(*scaleMappedData, sizeof(GPUScaleAffectorParams));
		if(allocationResult == CUDA_SUCCESS)//if memory allocated successfully, add this affector
			scaleParticleAffector = static_cast<ScaleParticleAffector*>(affector->second);
		break;

	default:
		break;
	}	

	//Already registered to Positions, so make sure we don't do it again
	if(affector->second->GetDesiredBuffer() != Ogre::VES_POSITION)
		RegisterBufferResource(affector->second->GetDesiredBuffer(), particleSystem->CreateVertexBuffer(affector->second->GetDesiredBuffer()));
}

bool ParticleKernel::RegisterBufferResource(GraphicsResourcePointers resourceIndex, Ogre::HardwareVertexBufferSharedPtr bufferToRegister)
{
	//If count is the index, break out
	if(resourceIndex == GraphicsResourcePointerCount || bufferToRegister.isNull())
		return false;

	return gpuData->RegisterCudaGraphicsResource(resourceIndex, bufferToRegister);
}

bool ParticleKernel::RegisterBufferResource(Ogre::VertexElementSemantic semantic, Ogre::HardwareVertexBufferSharedPtr bufferToRegister)
{
	GraphicsResourcePointers resourceIndex;

	switch (semantic)
	{
	case Ogre::VES_POSITION:
		resourceIndex = Positions;
		break;
	case Ogre::VES_BLEND_WEIGHTS:
		resourceIndex = BlendWeights;
		break;
	case Ogre::VES_BLEND_INDICES:
		resourceIndex = BlendIndices;
		break;
	case Ogre::VES_NORMAL:
		resourceIndex = Normals;
		break;
	case Ogre::VES_DIFFUSE:
		resourceIndex = PrimaryColour;
		break;
	case Ogre::VES_SPECULAR:
		resourceIndex = SecondaryColour;
		break;
	case Ogre::VES_TEXTURE_COORDINATES:
		resourceIndex = UV0;
		break;
	case Ogre::VES_BINORMAL:
		resourceIndex = Binormal;
		break;
	case Ogre::VES_TANGENT:
		resourceIndex = Tangent;
		break;
	default:
		resourceIndex = GraphicsResourcePointerCount;
		break;
	}

	return RegisterBufferResource(resourceIndex, bufferToRegister);
}

GPUResourcePointers ParticleKernel::GetOpenGLPointers()
{
	using namespace physx;

	GPUResourcePointers pointers;

	if (gpuData->GetIsRegistered(Positions))
		pointers.positions = (PxVec4*)gpuData->MapAndGetGPUDataPointer(Positions).devicePointer;
	
	if (gpuData->GetIsRegistered(Normals))
		pointers.normals = (PxVec3*)gpuData->MapAndGetGPUDataPointer(Normals).devicePointer;
	
	if (gpuData->GetIsRegistered(BlendIndices))
		pointers.blendIndices = (PxVec4*)gpuData->MapAndGetGPUDataPointer(BlendIndices).devicePointer;

	if (gpuData->GetIsRegistered(BlendWeights))
		pointers.blendWeights = (PxVec4*)gpuData->MapAndGetGPUDataPointer(BlendWeights).devicePointer;

	if (gpuData->GetIsRegistered(PrimaryColour))
		pointers.primaryColour = (PxVec4*)gpuData->MapAndGetGPUDataPointer(PrimaryColour).devicePointer;

	if (gpuData->GetIsRegistered(SecondaryColour))
		pointers.secondaryColour = (PxVec4*)gpuData->MapAndGetGPUDataPointer(SecondaryColour).devicePointer;

	if (gpuData->GetIsRegistered(Binormal))
		pointers.binormals = (PxVec3*)gpuData->MapAndGetGPUDataPointer(Binormal).devicePointer;

	if (gpuData->GetIsRegistered(Tangent))
		pointers.tangent = (PxVec3*)gpuData->MapAndGetGPUDataPointer(Tangent).devicePointer;

	if (gpuData->GetIsRegistered(UV0))
		pointers.uv0 = (PxVec4*)gpuData->MapAndGetGPUDataPointer(UV0).devicePointer;

	return pointers;

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

	GPUResourcePointers openGLData = GetOpenGLPointers();
	
	MappedGPUData src_bit_data = gpuData->GetNonGraphicsResource(DevicePointers::ValidBitmap);
	MappedGPUData src_lifetimes = gpuData->GetNonGraphicsResource(DevicePointers::Lifetimes);

	MappedGPUData src_affector_data = gpuData->GetNonGraphicsResource(DevicePointers::AffectorParameterCollection);

	//Ensure we have valid data...properly implement this eventually
	if(src_lifetimes.isValid)
	{
		//Construct the parameters for the Kernel
		void* args[7] = {
			&openGLData,
			&src_pos_data,
			&src_bit_data.devicePointer,
			&src_lifetimes,
			&initialLifetime,
			&src_affector_data.devicePointer,
			&particleData->validParticleRange
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
		/*gpuData->UnmapResourceFromCuda(GraphicsResourcePointers::Positions);
		gpuData->UnmapResourceFromCuda(GraphicsResourcePointers::PrimaryColour);*/
		gpuData->UnmapAllResourcesFromCuda();

		UnmapAffectors();
		return res == CUDA_SUCCESS;
		
	}

	return false;
}


void ParticleKernel::UnmapAffectors()
{
	//Use this method to allow affectors a chance for clean up after kernel execution
	/*if(colourFadeAffector)
	colourFadeAffector->UnmapCudaBuffers();

	if(scaleParticleAffector)
	scaleParticleAffector->UnmapCudaBuffers();*/
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