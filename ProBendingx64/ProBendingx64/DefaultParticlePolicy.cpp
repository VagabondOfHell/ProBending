#include "DefaultParticlePolicy.h"
#include "pxtask\PxCudaContextManager.h"
#include "OgreHardwareBufferManager.h"
#include "OgreMaterial.h"
#include "OgreLogManager.h"
#include "pxtask\PxGpuCopyDesc.h"
#include "pxtask\PxGpuDispatcher.h"
#include "foundation/PxMemory.h"
#include "PsBitUtils.h"
#include <math.h>

using namespace physx;

DefaultParticlePolicy::DefaultParticlePolicy(void)
{
	//The box around the Ogre::SimpleRenderer
	mBox.setInfinite();
	
	framesPassed = 0;
	framesTillCopy = 0;

	onGPU = false;
}

DefaultParticlePolicy::~DefaultParticlePolicy(void)
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

	cuModuleUnload(module);
}

void DefaultParticlePolicy::Initialize(unsigned int _maxParticles, PxParticleSystem* _particleSystem)
{
	//Assign the variables
	maxParticles = _maxParticles;
	particleSystem = _particleSystem;
	performCopyThisFrame = false;
	initialLifetime = 2.5f;

	lifetimes = new float[maxParticles];

	//loop through, indicate available indices, and initialize all lifetimes to 0
	for (int i = maxParticles - 1; i >= 0; --i)
	{
		lifetimes[i] = 0.0f;
	}

	_particleSystem->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

	// our vertices are just points
	mRenderOp.operationType = Ogre::RenderOperation::OT_POINT_LIST;
	mRenderOp.useIndexes = false;//EBO
	
	mRenderOp.vertexData = new Ogre::VertexData();
	mRenderOp.vertexData->vertexCount = maxParticles;
	mRenderOp.vertexData->vertexBufferBinding->unsetAllBindings();

	InitializeVertexBuffers();
}

void DefaultParticlePolicy::InitializeVertexBuffers()
{
	size_t currOffset = 0;

	mRenderOp.vertexData->vertexDeclaration->addElement(0, currOffset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
	
	// allocate the vertex buffer
	mVertexBufferPosition = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3),
			maxParticles,
			Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE,///Add dynamicWriteOnly as well?
			false);

	PxVec3* positions = static_cast<PxVec3*>(mVertexBufferPosition->lock(Ogre::HardwareBuffer::LockOptions::HBL_WRITE_ONLY));

	for (unsigned int i = 0; i < maxParticles; i++)
	{
		positions[i] = PxVec3(std::numeric_limits<float>::quiet_NaN());
	}

	mVertexBufferPosition->unlock();

	// bind positions to location 0
	mRenderOp.vertexData->vertexBufferBinding->setBinding(0, mVertexBufferPosition);

	mRenderOp.vertexData->vertexCount = maxParticles;
}

void DefaultParticlePolicy::InitializeGPUData(physx::PxCudaContextManager* contextManager)
{
	if(!CudaModuleHelper::LoadCUDAModule(&module, "x64/Debug/KernelTest.ptx"))
	{
		printf("Load CUDA Module failed! Error: %i\n", CudaModuleHelper::GetLastCudaError());
		onGPU = false;
		return;
	}

	if(!CudaModuleHelper::LoadCUDAFunction(&updateParticlesKernel, module, "updateBillboardVB"))
	{
		printf("Load CUDA Function failed! Error: %i\n", CudaModuleHelper::GetLastCudaError());
		onGPU = false;
		return;
	}
	
	gpuBuffers = new CudaGPUData(contextManager, GraphicsResourcePointers::Count, DevicePointers::Count);
	
	bool gpuAllocationResult = false;
	
	//Register the resource to CUDA
	gpuAllocationResult = gpuBuffers->RegisterCudaGraphicsResource(GraphicsResourcePointers::Positions, mVertexBufferPosition);

	//Allocate room on the GPU once for the validity bitmaps
	if(gpuAllocationResult)
		gpuAllocationResult = gpuBuffers->AllocateGPUMemory(DevicePointers::ValidBitmap, sizeof(PxU32) * (maxParticles + 31) >> 5);

	//If we failed gpu allocation, indicate it in our onGPU result
	onGPU = gpuAllocationResult;
}

std::vector<const physx::PxU32> DefaultParticlePolicy::UpdatePolicy(float time, physx::PxParticleReadData* readData, 
							physx::PxParticleReadDataFlags readableData)
{
	std::vector<const physx::PxU32> indicesToRemove;

	//PxMemCopy(mParticleValidity, readData->validParticleBitmap, ((readData->validParticleRange + 31) >> 5) << 2);

	//Reserve 5% of the max particles for removal
	indicesToRemove.reserve(maxParticles * 0.05f);
	PxU32 newValidRange = 0;

	//Check if there is any updating to do
	if (readData->validParticleRange > 0)
	{
		PxVec3* positions = NULL;

		//Store as a boolean to prevent doing the modulus in the loop below
		//Done as a ternary operation to avoid divide by 0 when frames passed is 0
		//This checks if frames till copy is 0. If it is, set frames passed to 0 and return true
		//If it isn't, modulus the frames passed with the number of frames until copy and check if remainder is 0
		performCopyThisFrame = framesTillCopy == 0 ? (framesPassed = 0) == 0 : (framesPassed %= framesTillCopy) == 0;

		int numParticles = 0;
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
						++numParticles;
						newValidRange = index;
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
		//mValidParticleRange = newValidRange;
	}//end if valid range > 0

	//increment frames passed here, because the GPU version may not get called, but this one is guaranteed to be called
	framesPassed++;

	return indicesToRemove;
}

void DefaultParticlePolicy::UpdatePolicyGPU(float time, physx::PxParticleReadData* readData, physx::PxParticleReadDataFlags readableData)
{
	if(readData->nbValidParticles > 0)
	{
		//CPU Update calculates this bool for us. If true, copy Host to Device
		if(performCopyThisFrame)
		{
			if(!gpuBuffers->CopyHostToDevice(DevicePointers::ValidBitmap, 
				&readData->validParticleBitmap[0], sizeof(PxU32)*(maxParticles + 31) >> 5))
				printf("Copy Validity to GPU Failed");
		}

		CUdeviceptr src_pos_data = reinterpret_cast<CUdeviceptr>(&readData->positionBuffer[0]);

		MappedGPUData dest_pos_data = gpuBuffers->MapAndGetGPUDataPointer(GraphicsResourcePointers::Positions);
		MappedGPUData src_bit_data = gpuBuffers->GetNonGraphicsResource(DevicePointers::ValidBitmap);

		//Ensure we have valid data
		if(dest_pos_data.isValid)
		{
			//Construct the parameters for the Kernel
			void* args[5] = {
				&dest_pos_data.devicePointer,
				&src_pos_data,
				&src_bit_data.devicePointer,
				&readData->nbValidParticles,
				&maxParticles
			};

			//Launch the kernel with 1 block of 512 threads. Each thread will complete (MaxParticles/512) worth of particles 
			CUresult res = cuLaunchKernel(updateParticlesKernel, 1, 1, 1, 512, 1, 1, 0, 0, args, 0); 
		
			//If an error, log the problem
			if(res != CUDA_SUCCESS)
				Ogre::LogManager::getSingleton().getDefaultLog()->
					logMessage("Default Particle Policy Cuda Kernel Launch failed", Ogre::LogMessageLevel::LML_CRITICAL);
		}

		//Unmap the graphics resource
		gpuBuffers->UnmapResourceFromCuda(GraphicsResourcePointers::Positions);
	}
}

void DefaultParticlePolicy::ParticlesCreated(unsigned int createdCount, physx::PxStrideIterator<const PxU32> emittedIndices)
{
	for (int i = 0; i < createdCount; i++)
	{
		lifetimes[emittedIndices[i]] = 3;// (rand() % 8) + 2;
	}
}