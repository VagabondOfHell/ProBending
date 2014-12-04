#include "ColourParticleSystem.h"
#include "BasicParticleSystem.h"
#include "OgreHardwareBufferManager.h"
#include "OgreLogManager.h"
#include "PsBitUtils.h"

ColourParticleSystem::ColourParticleSystem(AbstractParticleEmitter* _emitter, size_t _maximumParticles, float _initialLifetime,
										   physx::PxVec4 _startColor, physx::PxVec4 _endColor, physx::PxVec4 _fadeRate,
										   ParticleSystemParams& paramsStruct, bool _ownEmitter)
		 : ParticleSystemBase(_emitter, _maximumParticles, paramsStruct, _ownEmitter), initialLifetime(_initialLifetime),
			startColor(_startColor), endColor(_endColor), fadeRate(_fadeRate),  framesPassed(0), framesTillCopy(0)
{

}


ColourParticleSystem::~ColourParticleSystem(void)
{
	if(gpuBuffers)
	{
		//Does this work if they arent registered?
		gpuBuffers->UnregisterAllGraphicsResources();
		gpuBuffers->FreeAllGPUMemory();
		delete gpuBuffers;
		gpuBuffers = NULL;
	}

	if(lifetimes)
	{
		delete[] lifetimes;
		lifetimes = NULL;
	}

	cuModuleUnload(cudaModule);
}

void ColourParticleSystem::InitializeVertexBuffers()
{
	using namespace physx;

	size_t currOffset = 0;

	mRenderOp.vertexData->vertexDeclaration->addElement(0, currOffset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

	// allocate the vertex buffer
	mVertexBufferPosition = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
		Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3),
		maximumParticles,
		Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE,
		false);

	PxVec3* positions = static_cast<PxVec3*>(mVertexBufferPosition->lock(Ogre::HardwareBuffer::LockOptions::HBL_WRITE_ONLY));

	for (unsigned int i = 0; i < maximumParticles; i++)
	{
		positions[i] = PxVec3(std::numeric_limits<float>::quiet_NaN());
	}

	mVertexBufferPosition->unlock();

	// bind positions to location 0
	mRenderOp.vertexData->vertexBufferBinding->setBinding(0, mVertexBufferPosition);

	currOffset = 0;

	mRenderOp.vertexData->vertexDeclaration->addElement(1, currOffset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
	currOffset+= Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);

	mVertexBufferColour = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
		Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT4), maximumParticles, 
		Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE, false);

	Ogre::Vector4* colours = static_cast<Ogre::Vector4*>(mVertexBufferColour->lock(Ogre::HardwareBuffer::LockOptions::HBL_WRITE_ONLY));

	for (unsigned int i = 0; i <maximumParticles; ++i)
	{
		colours[i] = Ogre::Vector4(startColor.x, startColor.y, startColor.z, startColor.w);
	}

	mVertexBufferColour->unlock();

	mRenderOp.vertexData->vertexBufferBinding->setBinding(1, mVertexBufferColour);
	

	//Set the vertex count to the maximum allowed particles in case we are running on the CPU
	//Otherwise GPU update will override
	mRenderOp.vertexData->vertexCount = maximumParticles;
}

void ColourParticleSystem::InitializeParticleSystemData()
{
	//Assign the variables
	performCopyThisFrame = false;

	lifetimes = new float[maximumParticles];

	//loop through, indicate available indices, and initialize all lifetimes to 0
	for (int i = maximumParticles - 1; i >= 0; --i)
	{
		lifetimes[i] = 0.0f;
	}

	// our vertices are just points
	mRenderOp.operationType = Ogre::RenderOperation::OT_POINT_LIST;
	mRenderOp.useIndexes = false;//EBO
	
	mRenderOp.vertexData = new Ogre::VertexData();
	mRenderOp.vertexData->vertexCount = maximumParticles;
	mRenderOp.vertexData->vertexBufferBinding->unsetAllBindings();

	InitializeVertexBuffers();
}

void ColourParticleSystem::InitializeGPUData()
{
	//Load the module the kernel belongs to (compiled file)
	if(!CudaModuleHelper::LoadCUDAModule(&cudaModule, "x64/Debug/KernelTest.ptx"))
	{
		printf("Load CUDA Module failed! Error: %i\n", CudaModuleHelper::GetLastCudaError());
		onGPU = false;
		return;
	}

	//Load the cuda kernel
	if(!CudaModuleHelper::LoadCUDAFunction(&updateParticlesKernel, cudaModule, "UpdateColourSystem"))
	{
		printf("Load CUDA Function failed! Error: %i\n", CudaModuleHelper::GetLastCudaError());
		onGPU = false;
		return;
	}

	//Initialize GPU Data
	gpuBuffers = new CudaGPUData(cudaContextManager, 
		GraphicsResourcePointers::GraphicsResourcePointerCount, DevicePointers::DevicePointerCount);

	bool gpuAllocationResult;

	//Register the resource to CUDA
	gpuAllocationResult = gpuBuffers->RegisterCudaGraphicsResource(GraphicsResourcePointers::Positions, mVertexBufferPosition);

	if(gpuAllocationResult)
		gpuAllocationResult = gpuBuffers->RegisterCudaGraphicsResource(GraphicsResourcePointers::Colour, mVertexBufferColour);

	if(gpuAllocationResult)
		gpuAllocationResult = gpuBuffers->AllocateGPUMemory(DevicePointers::Lifetimes, sizeof(float) * maximumParticles);

	//Allocate room on the GPU once for the validity bitmaps
	if(gpuAllocationResult)
		gpuAllocationResult = gpuBuffers->AllocateGPUMemory(DevicePointers::ValidBitmap, 
		sizeof(physx::PxU32) * (maximumParticles + 31) >> 5); //This was taken from physx particle website

	//If we failed gpu allocation, indicate it in our onGPU result
	onGPU = gpuAllocationResult;
}

void ColourParticleSystem::ParticlesCreated(const unsigned int createdCount, physx::PxStrideIterator<const physx::PxU32> emittedIndices)
{
	//Loop through all the created particles and set their lifetimes
	for (unsigned int i = 0; i < createdCount; i++)
	{
		lifetimes[emittedIndices[i]] = initialLifetime;
	}
}

std::vector<const physx::PxU32> ColourParticleSystem::UpdatePolicy(const float time, const physx::PxParticleReadData* const readData)
{
	using namespace physx;
	//Add CPU Updating to colours

	std::vector<const PxU32> indicesToRemove;

	//PxMemCopy(mParticleValidity, readData->validParticleBitmap, ((readData->validParticleRange + 31) >> 5) << 2);

	//Reserve 5% of the max particles for removal
	indicesToRemove.reserve(maximumParticles * 0.05f);

	//Check if there is any updating to do
	if (readData->validParticleRange > 0)
	{
		PxVec3* positions = NULL;

		//Store as a boolean to prevent doing the modulus in the loop below
		//Done as a ternary operation to avoid divide by 0 when frames passed is 0
		//This checks if frames till copy is 0. If it is, set frames passed to 0 and return true
		//If it isn't, modulus the frames passed with the number of frames until copy and check if remainder is 0
		performCopyThisFrame = framesTillCopy == 0 ? (framesPassed = 0) == 0 : (framesPassed %= framesTillCopy) == 0;

		int numParticles(0);

		//Lock Ogre GL Buffers if not using CUDA so we can update them
		if(!onGPU)
			if(performCopyThisFrame)
				positions = static_cast<PxVec3*>(mVertexBufferPosition->lock(Ogre::HardwareBuffer::LockOptions::HBL_WRITE_ONLY));

		for (PxU32 w = 0; w <= (readData->validParticleRange-1) >> 5; w++)
		{
			//For each bit of the bitmap
			for (PxU32 b = readData->validParticleBitmap[w]; b; b &= b-1)
			{
				PxU32 index = (w << 5 | shdfnd::lowestSetBit(b));

				//Check particle validity
				if(readData->flagsBuffer[index] & PxParticleFlag::eSPATIAL_DATA_STRUCTURE_OVERFLOW ||
					!(readData->flagsBuffer[index] & PxParticleFlag::eVALID) || lifetimes[index] <= 0.0f )
				{
					//If lifetime is equal or below zero
					indicesToRemove.push_back(index); //indicate removal

					lifetimes[index] = 0.0f;//set lifetime to 0

					if(!onGPU)
						positions[index] = PxVec3(std::numeric_limits<float>::quiet_NaN());

					continue;
				}
				//If the particle is valid and lifetimes are above 0, subtract this frame
				else
				{
					lifetimes[index] -= time;

					//printf("Particle Position: %f, %f, %f\n", positions[index].x,positions[index].y, positions[index].z);
					++numParticles;
				}

				if(!onGPU)
				{
					if(performCopyThisFrame)
					{
						// copy particle positions over
						const PxVec3& position = readData->positionBuffer[index];

						positions[index] = position;
					}
				}
			}//end of bitmap for loop (b)
		}//end of particle range for loop

		//If this system is on the CPU, unlock the vertex buffer
		if(!onGPU)
			if(performCopyThisFrame)
				mVertexBufferPosition->unlock();

		if(onGPU)
			mRenderOp.vertexData->vertexCount = numParticles;

	}//end if valid range > 0

	//increment frames passed here, because the GPU version may not get called, (if CUDA not enabled)
	//but this one is guaranteed to be called
	framesPassed++;

	return indicesToRemove;
}

void ColourParticleSystem::UpdatePolicyGPU(const float time, physx::PxParticleReadData* const readData)
{
	using namespace physx;

	//CPU Update calculates this bool for us. If true, copy Host to Device
	if(performCopyThisFrame)
	{
		if(!gpuBuffers->CopyHostToDevice(DevicePointers::ValidBitmap, 
			&readData->validParticleBitmap[0], sizeof(PxU32)*(maximumParticles + 31) >> 5))
			printf("Copy Validity to GPU Failed");

		if(!gpuBuffers->CopyHostToDevice(DevicePointers::Lifetimes,
			&lifetimes[0], sizeof(float) * maximumParticles))
			printf("Copy Lifetimes to GPU Failed\n");
	}

	CUdeviceptr src_pos_data = reinterpret_cast<CUdeviceptr>(&readData->positionBuffer[0]);

	MappedGPUData dest_pos_data = gpuBuffers->MapAndGetGPUDataPointer(GraphicsResourcePointers::Positions);
	MappedGPUData dest_col_data = gpuBuffers->MapAndGetGPUDataPointer(GraphicsResourcePointers::Colour);
	MappedGPUData src_bit_data = gpuBuffers->GetNonGraphicsResource(DevicePointers::ValidBitmap);
	MappedGPUData src_life_data = gpuBuffers->GetNonGraphicsResource(DevicePointers::Lifetimes);

	//Ensure we have valid data
	if(dest_pos_data.isValid)
	{
		//Construct the parameters for the Kernel
		void* args[10] = {
			&dest_pos_data.devicePointer,
			&src_pos_data,
			&src_bit_data.devicePointer,
			&src_life_data.devicePointer,
			&initialLifetime,
			&dest_col_data,
			&startColor,
			&endColor,
			&fadeRate,
			&readData->nbValidParticles
		};

		//Launch the kernel with 1 block of 512 threads. Each thread will complete (MaxParticles/512) worth of particles 
		CUresult res = cuLaunchKernel(updateParticlesKernel, 1, 1, 1, 512, 1, 1, 0, 0, args, 0); 

#if _DEBUG
		//If an error, log the problem
		if(res != CUDA_SUCCESS)
			Ogre::LogManager::getSingleton().getDefaultLog()->
			logMessage("Basic Particle System Cuda Kernel Launch failed", Ogre::LogMessageLevel::LML_CRITICAL);
#endif

		//Unmap the graphics resource
		gpuBuffers->UnmapResourceFromCuda(GraphicsResourcePointers::Positions);
		gpuBuffers->UnmapResourceFromCuda(GraphicsResourcePointers::Colour);
	}
}
