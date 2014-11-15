#include "ParticleRenderer.h"

//#include "CudaInteropBuffer.h"
#include "OgreRoot.h"
#include "PxPhysicsAPI.h"
#include "IScene.h"

#include "kernelTest.cuh"
#include "device_launch_parameters.h"

#include "CudaModuleHelper.h"

ParticleRenderer::ParticleRenderer(IScene* _sceneOwner, UINT32 numParticles)
	:particleCount(0), volumeSize(2)
{
	owningScene = _sceneOwner;

	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create("CudaVertexBufferMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	
	//The box around the Ogre::SimpleRenderer
	mBox.setInfinite();
	
	material->createTechnique()->createPass();
	material->getTechnique(0)->getPass(0)->setLightingEnabled(false);
	material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
	material->getTechnique(0)->getPass(0)->setVertexColourTracking(Ogre::TVC_DIFFUSE);
	setMaterial("CudaVertexBufferMaterial");

	Resize(numParticles);

	physx::PxU32* particleIndices = new physx::PxU32[numParticles];
	physx::PxVec3* particlePositions = new physx::PxVec3[numParticles];
	physx::PxVec3* particleVelocities = new physx::PxVec3[numParticles];

	physx::PxParticleSystem* ps = PxGetPhysics().createParticleSystem(numParticles, false);

	for (physx::PxU32 i = 0; i < numParticles; i++)
	{
		particleIndices[i] = i;
		particlePositions[i] = physx::PxVec3(1 + i * 0.2f, 0.0f, 0);
		particleVelocities[i] = physx::PxVec3(0.0f, 0.0f, 0.0f);
	}

	physx::PxParticleCreationData data = physx::PxParticleCreationData();
	data.numParticles = numParticles;
	data.indexBuffer = physx::PxStrideIterator<physx::PxU32>(particleIndices);
	data.positionBuffer = physx::PxStrideIterator<physx::PxVec3>(particlePositions);
	data.velocityBuffer = physx::PxStrideIterator<physx::PxVec3>(particleVelocities);
	
	ps->setParticleBaseFlag(physx::PxParticleBaseFlag::eGPU, true);
	
	physx::PxParticleBaseFlags flags = ps->getParticleBaseFlags();
	
	if(flags & physx::PxParticleBaseFlag::eGPU)
		printf("On GPU");

	ps->createParticles(data);
	ps->setParticleReadDataFlag(physx::PxParticleReadDataFlag::eVELOCITY_BUFFER, true);
	
	ps->setMaxMotionDistance(10.0f);
	
	owningScene->GetPhysXScene()->addActor(*ps);

	particleSystem = ps;

	delete particlePositions;
	delete particleIndices;
	delete particleVelocities;
}

ParticleRenderer::~ParticleRenderer(void)
{
	if(positionBuffer)
	{
		delete positionBuffer;
		positionBuffer = NULL;
	}

	cuModuleUnload(module);
}

void ParticleRenderer::Resize(UINT32 newParticleNumber)
{
	if(particleCount == newParticleNumber)
		return;

	particleCount = newParticleNumber;

	CreateVertexBuffers();
}

void ParticleRenderer::CreateVertexBuffers()
{
	// our vertices are just points
	mRenderOp.operationType = Ogre::RenderOperation::OT_POINT_LIST;
	mRenderOp.useIndexes = false;//EBO
	
	mRenderOp.vertexData = new Ogre::VertexData();
	mRenderOp.vertexData->vertexCount = particleCount;
	mRenderOp.vertexData->vertexBufferBinding->unsetAllBindings();
	
	// POSITIONS
	// define the vertex format
	size_t currOffset = 0;
	mRenderOp.vertexData->vertexDeclaration->addElement(0, currOffset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

	// allocate the vertex buffer
	Ogre::HardwareVertexBufferSharedPtr mVertexBufferPosition = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			currOffset,
			mRenderOp.vertexData->vertexCount,
			Ogre::HardwareBuffer::HBU_DISCARDABLE,///Add dynamicWriteOnly as well?
			false);

	// bind positions to location 0
	mRenderOp.vertexData->vertexBufferBinding->setBinding(0, mVertexBufferPosition);

	// Fill some random positions for particles
	Ogre::Vector3* pVertexPos = static_cast<Ogre::Vector3*>(mVertexBufferPosition->lock(Ogre::HardwareBuffer::HBL_NORMAL));

	for(UINT32 i = 0; i< particleCount;i++)
	{
		Ogre::Vector3 pos = Ogre::Vector3(1 + i * 0.2f, 0.0f, 0);//, 1.0f);
		pVertexPos[i] = pos;
	}

	mVertexBufferPosition->unlock();

	positionBuffer = new CudaInteropBuffer(owningScene, mVertexBufferPosition);

	if(!CudaModuleHelper::LoadCUDAModule(&module, "x64/Debug/KernelTest.ptx"))
		printf("Load CUDA Module failed! Error: %i\n", CudaModuleHelper::GetLastCudaError());

	if(!CudaModuleHelper::LoadCUDAFunction(&updateParticlesKernel, module, "UpdateParticlesKernel"))
		printf("Load CUDA Function failed! Error: %i\n", CudaModuleHelper::GetLastCudaError());

	// COLORS
	// define the vertex format
	//currOffset = 0;
	//mRenderOp.vertexData->vertexDeclaration->addElement(1, currOffset, Ogre::VET_FLOAT4, Ogre::VES_DIFFUSE);
	//currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT4);
	//// allocate the color buffer
	//mVertexBufferColor = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
	//mRenderOp.vertexData->vertexDeclaration->getVertexSize(0),
	//mRenderOp.vertexData->vertexCount,
	//Ogre::HardwareBuffer::HBU_DISCARDABLE,///Add dynamicWriteOnly as well?
	//false);

	// bind colors to 1
//	mRenderOp.vertexData->vertexBufferBinding->setBinding(1, mVertexBufferColor);



	// Fill some colors for particles
	//Ogre::RGBA* pVertexColor = static_cast<Ogre::RGBA*>(mVertexBufferColor->lock(Ogre::HardwareBuffer::HBL_NORMAL));
	//Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();

	//for(UINT32 i = 0; i < particleCount; i++)
	//{
	//	rs->convertColourValue(Ogre::ColourValue::Red, &pVertexColor[i]);
	//}

	//mVertexBufferColor->unlock();
	//

}

void ParticleRenderer::UpdateParticles(physx::PxVec3* posArray, UINT32 size)
{
	// Fill some random positions for particles
	Ogre::Vector3* pVertexPos = static_cast<Ogre::Vector3*>(positionBuffer->GetOgreBuffer()->lock(Ogre::HardwareBuffer::HBL_NORMAL));
	
	for(UINT32 i = 0; i< size ;i++)
	{
		printf("OgrePosition %i: %f, %f, %f\n", i, pVertexPos[i].x, pVertexPos[i].y, pVertexPos[i].z);
		Ogre::Vector3 pos = Ogre::Vector3(posArray[i].x, posArray[i].y, posArray[i].z);
		pVertexPos[i] = pos;
	}

	positionBuffer->GetOgreBuffer()->unlock();
}

void ParticleRenderer::Update(float gameTime)
{
	physx::PxVec3* particleForces = new physx::PxVec3[particleCount];
	physx::PxU32* particleIndices = new physx::PxU32[particleCount];

	for (int i = 0; i < particleCount; i++)
	{
		particleForces[i] = physx::PxVec3(0.0f, -0.0f, 0.02f);
		particleIndices[i] = i;
	}

	owningScene->GetCudaContextManager()->acquireContext();

	physx::PxParticleReadData* rd = particleSystem->lockParticleReadData(physx::PxDataAccessFlag::eDEVICE);
	
	CUdeviceptr dev = reinterpret_cast<CUdeviceptr>(&rd->positionBuffer[0]);
	CUdeviceptr d_ValidBitmap = reinterpret_cast<CUdeviceptr>(&rd->validParticleBitmap);
	CUdeviceptr d_Velocities = reinterpret_cast<CUdeviceptr>(&rd->velocityBuffer[0]);

	PxU32 validRange = rd->validParticleRange;

	CUresult result = positionBuffer->MapAndGetCudaGraphicsMemoryPointer();

	if(result != CUDA_SUCCESS)
	{
		printf("Get Map Graphics Resource FAILED: %i", result);
	}

	CUdeviceptr p = positionBuffer->GetCudaDevicePointer();

	CUdeviceptr d_validRange;

	if(!CudaModuleHelper::AllocateAndCopyHostToDevice(&d_validRange, sizeof(PxU32), &validRange, sizeof(PxU32)))
		printf("Error with Device Allocation or Copying! Error: %i\n", CudaModuleHelper::GetLastCudaError());

	//Set up Kernel Arguments
	void* args[2] = 
	{
		&p,
		&dev
	};
	
	CUresult res = cuLaunchKernel(updateParticlesKernel, 1000, 1, 1, 10, 1, 1, 0, 0, args, 0); 
	//positionBuffer->LaunchPhysxCopyKernel(dev);

	cuMemFree(d_validRange);

	//if(res != CUDA_SUCCESS)
	//{
	//	printf("err");
	//}

	//Ogre::Vector3* pVertexPos = new Ogre::Vector3[particleCount];

	//result = cuMemcpyDtoH_v2(pVertexPos, positionBuffer->GetCudaDevicePointer(), positionBuffer->GetBufferSize());

	
	/*
	if(result != CUDA_SUCCESS)
	{
		printf("Copy To Host FAILED: %i", result);
	}
	*/
	if(rd)
		rd->unlock();

	std::vector<unsigned int> tempIndices = std::vector<unsigned int>();

	rd = particleSystem->lockParticleReadData(physx::PxDataAccessFlag::eREADABLE);

	if(rd)
	{
		physx::PxStrideIterator<const physx::PxParticleFlags> flagsIter(rd->flagsBuffer);
		physx::PxStrideIterator<const physx::PxVec3> positionIter (rd->positionBuffer);
		
		for (unsigned int i = 0; i < rd->validParticleRange; ++i, ++flagsIter, ++positionIter)
		{
			if(*flagsIter & physx::PxParticleFlag::eVALID)
			{
				const physx::PxVec3& position = *positionIter;

				if(position.magnitude() > 25.0f)
				{
					tempIndices.push_back(i);
				}
			}
		}

		particleSystem->addForces(particleCount, physx::PxStrideIterator<physx::PxU32>(particleIndices),
			physx::PxStrideIterator<physx::PxVec3>(particleForces), physx::PxForceMode::eFORCE);

	//	
	//	/*particleSystem->setPositions(particleCount, physx::PxStrideIterator<physx::PxU32>(particleIndices),
	//		physx::PxStrideIterator<physx::PxVec3>(particleForces));*/

		rd->unlock();
	}

	if(tempIndices.size() > 0)
	{
		RemoveParticles(tempIndices.size(), PxStrideIterator<PxU32>(&tempIndices[0]));
		SpawnParticles(tempIndices.size(), PxStrideIterator<PxU32>(&tempIndices[0]));
	}

positionBuffer->UnmapFromCudaBuffer();
	owningScene->GetCudaContextManager()->releaseContext();

	delete particleForces;
	delete particleIndices;
//	delete pVertexPos;
}

void ParticleRenderer::SpawnParticles(unsigned int numToSpawn, PxStrideIterator<PxU32> indices)
{
	PxParticleCreationData spawnData;
	spawnData.numParticles = numToSpawn;
	
	PxVec3 *positions = new PxVec3[numToSpawn];
	PxVec3 *velocities = new PxVec3[numToSpawn];

	for (int i = 0; i < numToSpawn; i++)
	{
		positions[i] = PxVec3(0.0f);
		
		velocities[i] = PxVec3(rand() % 20 + 1);
	}

	spawnData.indexBuffer = indices;
	spawnData.positionBuffer = PxStrideIterator<PxVec3>(positions);
	spawnData.velocityBuffer = PxStrideIterator<PxVec3>(velocities);

	particleSystem->createParticles(spawnData);

	delete positions;
	delete velocities;
}

void ParticleRenderer::RemoveParticles(unsigned int numToRemove, PxStrideIterator<PxU32> indices)
{
	particleSystem->releaseParticles(numToRemove, indices);
}

Ogre::Real ParticleRenderer::getBoundingRadius(void) const
{
	return 0;
}

Ogre::Real ParticleRenderer::getSquaredViewDepth(const Ogre::Camera *) const
{
	return 0;
}