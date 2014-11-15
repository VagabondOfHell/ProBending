#include "DefaultParticlePolicy.h"
#include "pxtask\PxCudaContextManager.h"
#include "OgreHardwareBufferManager.h"
#include "OgreMaterialManager.h"
#include "OgrePrerequisites.h"
#include "OgreTechnique.h"
#include "OgreMaterial.h"
#include "pxtask\PxGpuCopyDesc.h"
#include "pxtask\PxGpuDispatcher.h"
#include "PsBitUtils.h"

using namespace physx;

DefaultParticlePolicy::DefaultParticlePolicy(void)
{
}

DefaultParticlePolicy::DefaultParticlePolicy(unsigned int _particlesPerFrame)
{
	particlesPerFrame = _particlesPerFrame;
	emitterPosition = PxVec3(0.0f, 0.0f, 0.0f);	
	mBox.setInfinite();
	frames = 0;

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
	maxParticles = _maxParticles;
	
	lifetimes = new float[maxParticles];

	for (int i = maxParticles - 1; i >= 0; --i)
	{
		availableIndices.push_back(i);
		lifetimes[i] = 0.0f;
	}

	particleSystem = _particleSystem;

	//The box around the Ogre::SimpleRenderer
	mBox.setInfinite();
	
	setMaterial("SkyChange");

	// our vertices are just points
	mRenderOp.operationType = Ogre::RenderOperation::OT_POINT_LIST;
	mRenderOp.useIndexes = false;//EBO
	
	mRenderOp.vertexData = new Ogre::VertexData();
	mRenderOp.vertexData->vertexCount = maxParticles;
	mRenderOp.vertexData->vertexBufferBinding->unsetAllBindings();

}

void DefaultParticlePolicy::InitializeGPUData(physx::PxCudaContextManager* contextManager)
{
	if(!CudaModuleHelper::LoadCUDAModule(&module, "x64/Debug/KernelTest.ptx"))
		printf("Load CUDA Module failed! Error: %i\n", CudaModuleHelper::GetLastCudaError());

	if(!CudaModuleHelper::LoadCUDAFunction(&updateParticlesKernel, module, "updateBillboardVB"))
		printf("Load CUDA Function failed! Error: %i\n", CudaModuleHelper::GetLastCudaError());
	
	gpuBuffers = new CudaGPUData(contextManager, GraphicsResourcePointers::Count, DevicePointers::Count);
	
	size_t currOffset = 0;

	mRenderOp.vertexData->vertexDeclaration->addElement(0, currOffset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
	
	// allocate the vertex buffer
	Ogre::HardwareVertexBufferSharedPtr mVertexBufferPosition = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			currOffset,
			//mRenderOp.vertexData->vertexCount,
			maxParticles,
			Ogre::HardwareBuffer::HBU_DISCARDABLE,///Add dynamicWriteOnly as well?
			false);

	PxVec3* positions = static_cast<PxVec3*>(mVertexBufferPosition->lock(Ogre::HardwareBuffer::LockOptions::HBL_WRITE_ONLY));

	for (int i = 0; i < maxParticles; i++)
	{
		positions[i] = emitterPosition;
	}

	mVertexBufferPosition->unlock();

	// bind positions to location 0
	mRenderOp.vertexData->vertexBufferBinding->setBinding(0, mVertexBufferPosition);
	
	//Register the resource to CUDA
	gpuBuffers->RegisterCudaGraphicsResource(GraphicsResourcePointers::Positions, mVertexBufferPosition);

	//Allocate room on the GPU once for the lifetimes and bitmaps
	gpuBuffers->AllocateGPUMemory(DevicePointers::Lifetimes, sizeof(float) * maxParticles);
	gpuBuffers->AllocateGPUMemory(DevicePointers::Bitmaps, sizeof(PxU32) * (((maxParticles + 31) >> 5) << 2));
}

void DefaultParticlePolicy::UpdatePolicy(float time, physx::PxParticleReadData* readData, 
							physx::PxParticleReadDataFlags readableData)
{
	indicesToRemove.clear();

	physx::PxStrideIterator<const physx::PxParticleFlags> flagsIter(readData->flagsBuffer);
	physx::PxStrideIterator<const physx::PxVec3> positionIter (readData->positionBuffer);
	physx::PxStrideIterator<const physx::PxU32> validBitmapIter (readData->validParticleBitmap);
	physx::PxStrideIterator<const physx::PxVec3> velIter (readData->velocityBuffer);

	if (readData->validParticleRange > 0)
	{
			// iterate over valid particle bitmap
			for (PxU32 w = 0; w <= (readData->validParticleRange-1) >> 5; w++)
			{
					for (PxU32 b = readData->validParticleBitmap[w]; b; b &= b-1)
					{
						PxU32 index = (w << 5 | shdfnd::lowestSetBit(b));

						// access particle position
						const PxVec3& position = readData->positionBuffer[index];
						const PxVec3& velocity = readData->velocityBuffer[index];

						/*printf("Position %i: %f, %f, %f\n", index, position.x, position.y, position.z);
						printf("Velocity %i: %f, %f, %f\n", index, velocity.x, velocity.y, velocity.z);
						printf("Lifetimes %i: %f\n", index, lifetimes[index]);*/

						if(lifetimes[index] > 0)
							lifetimes[index] -= time;
						else
						{
							indicesToRemove.push_back(index);
							availableIndices.push_back(index);
							lifetimes[index] = 0.0f;
						}
					}
			}
	}
}

void DefaultParticlePolicy::UpdatePolicyGPU(float time, physx::PxParticleReadData* readData, physx::PxParticleReadDataFlags readableData)
{
	CUdeviceptr src_pos_data = reinterpret_cast<CUdeviceptr>(&readData->positionBuffer[0]);
	CUdeviceptr d_Velocities = reinterpret_cast<CUdeviceptr>(&readData->velocityBuffer[0]);
	//CUdeviceptr src_bitmap_data = reinterpret_cast<CUdeviceptr>(&readData->validParticleBitmap[0]);

	//CUdeviceptr lifetimes_device;
	CUdeviceptr range;

	/*if(!CudaModuleHelper::AllocateAndCopyHostToDevice(&lifetimes_device, sizeof(float) * maxParticles, &lifetimes[0], sizeof(float) * maxParticles))
		printf("Lifetime Host to Device copy failed");*/

	//if(!CudaModuleHelper::AllocateAndCopyHostToDevice(&src_bitmap_data, readData->nbValidParticles * sizeof(readData->validParticleBitmap[0]),
	//	&readData->validParticleBitmap[0], readData->nbValidParticles * sizeof(readData->validParticleBitmap[0])))
	//	printf("Error with Device Allocation or Copying! Error: %i\n", CudaModuleHelper::GetLastCudaError());

	if(frames % 3 == 0)
	{
		/*if(readData->nbValidParticles >= 62000)
			printf("R");*/

		if(!gpuBuffers->CopyHostToDevice(DevicePointers::Lifetimes, &lifetimes[0], sizeof(float) * maxParticles))
			printf("Copy lifetimes to GPU Failed");

		if(!gpuBuffers->CopyHostToDevice(DevicePointers::Bitmaps, &readData->validParticleBitmap[0], ((maxParticles + 31) >> 5) * sizeof(PxU32)))
			printf("Copy Particle Bitmap to GPU Failed");
	}

	MappedGPUData dest_pos_data = gpuBuffers->MapAndGetGPUDataPointer(GraphicsResourcePointers::Positions);

	//mRenderOp.vertexData->vertexCount = readData->validParticleRange;
	
	CUdeviceptr lifetimes = gpuBuffers->GetNonGraphicsDevicePointer(DevicePointers::Lifetimes);
	CUdeviceptr bitmaps = gpuBuffers->GetNonGraphicsResource(DevicePointers::Bitmaps).devicePointer;

	//if(dest_pos_data.isValid)
	{
		void* args[5] = {
			&dest_pos_data.devicePointer,
			&src_pos_data,
			&lifetimes,
			&bitmaps,
			&readData->validParticleRange
		};

		CUresult res = cuLaunchKernel(updateParticlesKernel, 1, 1, 1, 512, 1, 1, 0, 0, args, 0); 
		
		if(res != CUDA_SUCCESS)
			printf("Failure");
	}

	gpuBuffers->UnmapResourceFromCuda(0);

	frames++;
	/*cuMemFree_v2(lifetimes_device);
	cuMemFree_v2(src_bitmap_data);*/
}

PxParticleCreationData* DefaultParticlePolicy::Emit()
{
	indicesEmitted.clear();

	PxParticleCreationData* creationData = NULL;

	if(availableIndices.size() >= particlesPerFrame)
	{
		creationData = new PxParticleCreationData();
		indicesEmitted = std::vector<unsigned int>(availableIndices.end() - particlesPerFrame, availableIndices.end());

		for (int i = 0; i < particlesPerFrame; i++)
		{
			availableIndices.pop_back();
		}

		creationData->numParticles = indicesEmitted.size();
		creationData->indexBuffer = physx::PxStrideIterator<PxU32>(&indicesEmitted[0]);
		creationData->positionBuffer = physx::PxStrideIterator<PxVec3>(&emitterPosition, 0);
	}
	else if(availableIndices.size() > 0)
	{
		creationData = new PxParticleCreationData();

		indicesEmitted = std::vector<unsigned int>(availableIndices.end() - availableIndices.size(), availableIndices.end());

		for (int i = 0; i < availableIndices.size(); i++)
		{
			availableIndices.pop_back();
		}

		creationData->numParticles = indicesEmitted.size();
		creationData->indexBuffer = physx::PxStrideIterator<PxU32>(&indicesEmitted[0]);
		creationData->positionBuffer = physx::PxStrideIterator<PxVec3>(&emitterPosition, 0);
	}

	if(creationData && creationData->numParticles > 0)
	{
		forces.clear();

		for (int i = 0; i < indicesEmitted.size(); i++)
		{
			lifetimes[indicesEmitted[i]] = 1.50f;
			float ran = rand() % 2 + 1;
			float posX = rand()%2;
			float posY = rand() % 2;
			float posZ = rand() % 2;

			float x = rand()% 5;
			float y = rand() % 2;
			float z = 0;//rand() % 2;

			if(posX)
				x = -x;
			if(posY)
				y = -y;
			if(posZ)
				z = -z;

			forces.push_back(PxVec3(x, y, z));
		}
		
		creationData->velocityBuffer = PxStrideIterator<PxVec3>(&forces[0]);
	}

	return creationData;
}