#include "DefaultParticlePolicy.h"
#include "pxtask\PxCudaContextManager.h"
#include "OgreHardwareBufferManager.h"
#include "OgreMaterial.h"
#include "OgreLogManager.h"
#include "pxtask\PxGpuCopyDesc.h"
#include "pxtask\PxGpuDispatcher.h"
#include "PsBitUtils.h"

using namespace physx;

DefaultParticlePolicy::DefaultParticlePolicy(void)
{
	//The box around the Ogre::SimpleRenderer
	mBox.setInfinite();
	particlesPerSecond = 0;
	particlesToEmitThisFrame = 0;
	emitterPosition = PxVec3(0.0f);
	
	framesPassed = 0;
	framesTillCopy = 0;

	onGPU = false;

	setMaterial("DefaultParticleShader");
}

DefaultParticlePolicy::DefaultParticlePolicy(unsigned int _particlesPerSecond)
{
	particlesPerSecond = _particlesPerSecond;
	particlesToEmitThisFrame  = 0;
	particlesEmitted = 0;

	emitterPosition = PxVec3(0.0f, 0.0f, 0.0f);	
	
	framesPassed = 0;
	framesTillCopy = 0;

	onGPU = false;

	//The box around the Ogre::SimpleRenderer
	mBox.setInfinite();

	setMaterial("DefaultParticleShader");
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
		delete lifetimes;
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
	initialLifetime = 5.5f;

	lifetimes = new float[maxParticles];

	//Try to prevent excessive resizing by reserving space
	availableIndices.reserve(maxParticles);
	//indicesToRemove.reserve(particlesPerSecond * 0.01f);

	//loop through, indicate available indices, and initialize all lifetimes to 0
	for (int i = maxParticles - 1; i >= 0; --i)
	{
		availableIndices.push_back(i);
		lifetimes[i] = 0.0f;
	}

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
			//mRenderOp.vertexData->vertexCount,
			maxParticles,
			Ogre::HardwareBuffer::HBU_DISCARDABLE,///Add dynamicWriteOnly as well?
			false);

	PxVec3* positions = static_cast<PxVec3*>(mVertexBufferPosition->lock(Ogre::HardwareBuffer::LockOptions::HBL_WRITE_ONLY));

	for (unsigned int i = 0; i < maxParticles; i++)
	{
		positions[i] = PxVec3(99999.0f);
	}

	mVertexBufferPosition->unlock();

	// bind positions to location 0
	mRenderOp.vertexData->vertexBufferBinding->setBinding(0, mVertexBufferPosition);

	mRenderOp.vertexData->vertexCount = 0;
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

	//Allocate room on the GPU once for the lifetimes and bitmaps
	if(gpuAllocationResult)
		gpuAllocationResult = gpuBuffers->AllocateGPUMemory(DevicePointers::Lifetimes, sizeof(float) * maxParticles);
	
	//If we failed gpu allocation, indicate it in our onGPU result
	onGPU = gpuAllocationResult;
}

void DefaultParticlePolicy::UpdatePolicy(float time, physx::PxParticleReadData* readData, 
							physx::PxParticleReadDataFlags readableData)
{
	indicesToRemove.clear();

	physx::PxStrideIterator<const physx::PxParticleFlags> flagsIter(readData->flagsBuffer);
	physx::PxStrideIterator<const physx::PxVec3> positionIter (readData->positionBuffer);
	physx::PxStrideIterator<const physx::PxU32> validBitmapIter (readData->validParticleBitmap);

	//Indicate we should only draw up to the valid particle range
	mRenderOp.vertexData->vertexCount = readData->validParticleRange;

	//Check if there is any updating to do
	if (readData->validParticleRange > 0)
	{
		PxVec3* positions = NULL;

		//Store as a boolean to prevent doing the modulus in the loop below
		//Done as a ternary operation to avoid divide by 0 when frames passed is 0
		//This checks if frames till copy is 0. If it is, set frames passed to 0 and return true
		//If it isn't, modulus the frames passed with the number of frames until copy and check if remainder is 0
		performCopyThisFrame = framesTillCopy == 0 ? (framesPassed = 0) == 0 : (framesPassed %= framesTillCopy) == 0;

		if(!onGPU)
			if(performCopyThisFrame)
				positions = static_cast<PxVec3*>(mVertexBufferPosition->lock(Ogre::HardwareBuffer::LockOptions::HBL_WRITE_ONLY));

		for (PxU32 w = 0; w <= (readData->validParticleRange-1) >> 5; w++)
		{
			//For each bit of the bitmap
				for (PxU32 b = readData->validParticleBitmap[w]; b; b &= b-1)
				{
					PxU32 index = (w << 5 | shdfnd::lowestSetBit(b));

					if(!onGPU)
					{
						if(performCopyThisFrame)
						{
							// copy particle positions over
							const PxVec3& position = readData->positionBuffer[index];
							positions[index] = position;
						}
					}

					//Check particle validity
					if(flagsIter[index] & PxParticleFlag::eSPATIAL_DATA_STRUCTURE_OVERFLOW ||
						!(flagsIter[index] & PxParticleFlag::eVALID) || lifetimes[index] <= 0.0f )
					{
						//If lifetime is equal or below zero
						indicesToRemove.push_back(index); //indicate removal
						availableIndices.push_back(index); //add index to list of available indices
						lifetimes[index] = 0.0f;//set lifetime to 0

						if(!onGPU)
						{
							positions[index] = PxVec3(99999.0f);
						}

						continue;
					}
					//If the particle is valid and lifetimes are above 0, subtract this frame
					else
						lifetimes[index] -= time;

				}//end of bitmap for loop (b)
		}//end of particle range for loop

		//If this system is on the CPU, unlock the vertex buffer
		if(!onGPU)
			if(performCopyThisFrame)
				mVertexBufferPosition->unlock();

	}//end if valid range > 0

	//increment frames passed here, because the GPU version may not get called, but this one is guaranteed to be called
	framesPassed++;
}

void DefaultParticlePolicy::UpdatePolicyGPU(float time, physx::PxParticleReadData* readData, physx::PxParticleReadDataFlags readableData)
{
	if(readData->nbValidParticles > 0)
	{
		//CPU Update calculates this bool for us. If true, copy Host to Device
		if(performCopyThisFrame)
		{
			if(!gpuBuffers->CopyHostToDevice(DevicePointers::Lifetimes, &lifetimes[0], sizeof(float) * maxParticles))
				printf("Copy lifetimes to GPU Failed");
		}

		CUdeviceptr src_pos_data = reinterpret_cast<CUdeviceptr>(&readData->positionBuffer[0]);
		MappedGPUData dest_pos_data = gpuBuffers->MapAndGetGPUDataPointer(GraphicsResourcePointers::Positions);
		MappedGPUData src_lifetimes_data = gpuBuffers->GetNonGraphicsResource(DevicePointers::Lifetimes);

		//Ensure we have valid data
		if(dest_pos_data.isValid && src_lifetimes_data.isValid)
		{
			//Construct the parameters for the Kernel
			void* args[5] = {
				&dest_pos_data.devicePointer,
				&src_pos_data,
				&src_lifetimes_data.devicePointer,
				&readData->validParticleRange,
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

PxParticleCreationData* DefaultParticlePolicy::Emit(float gameTime)
{
	//Create creationData object but initialize to NULL
	PxParticleCreationData* creationData = NULL;

	unsigned int emissionCount(0);

	//Check amount of particles available
	if(availableIndices.size() > 0)
	{
		//If at least one particle is available, prepare creation data
		creationData = new PxParticleCreationData();

		particlesToEmitThisFrame += particlesPerSecond * gameTime;

		emissionCount = PxFloor(particlesToEmitThisFrame);

		particlesToEmitThisFrame -= emissionCount;

		//Gather available indices
		if(availableIndices.size() < emissionCount)
			emissionCount = availableIndices.size();
	}

	//If we have creation data and indices can be used
	if(creationData && emissionCount > 0)// && indicesEmitted.size() > 0)
	{
		//reset forces
		forces.clear();

		//Generate initial force and update available indices
		for (int i = 0; i < emissionCount; i++)
		{
			//initialize lifetimes
			lifetimes[availableIndices[(availableIndices.size() - 1) - i]] = rand() % 40;//initialLifetime;

			//Generate random directions and speed
			float posX = rand()%2;
			float posY = rand() % 2;
			float posZ = rand() % 2;

			float x = (rand()% 15);
			float y = (rand() % 10);
			float z = 0;//rand() % 2;

			if(posX)
				x = -x;
			if(posY)
				y = -y;
			if(posZ)
				z = -z;

			forces.push_back(PxVec3(x, y, z));
		}
		
		//set creation data parameters
		creationData->numParticles = emissionCount;
		creationData->indexBuffer = physx::PxStrideIterator<PxU32>(&availableIndices[availableIndices.size() - emissionCount]);
		creationData->positionBuffer = physx::PxStrideIterator<PxVec3>(&emitterPosition, 0);
		creationData->velocityBuffer = PxStrideIterator<PxVec3>(&forces[0]);
	}

	return creationData;
}

void DefaultParticlePolicy::ParticlesCreated(unsigned int createdCount)
{
	for (int i = 0; i < createdCount; i++)
	{
		availableIndices.pop_back();
	}
}