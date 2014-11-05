#include "ParticleRenderer.h"

//#include "CudaInteropBuffer.h"
#include "OgreRoot.h"
#include "PxPhysicsAPI.h"
#include "IScene.h"

#include "kernelTest.cuh"
#include "device_launch_parameters.h"

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

	// bind positions to 0
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

	
	 FILE *fp = fopen("C:\\Users\\Adam\\Desktop\\Capstone\\ProBendingx64\\ProBendingx64\\KernelTest.ptx", "rb");
            fseek(fp, 0, SEEK_END);
            int file_size = ftell(fp);
            char *buf = new char[file_size+1];
            fseek(fp, 0, SEEK_SET);
            fread(buf, sizeof(char), file_size, fp);
            fclose(fp);
            buf[file_size] = '\0';
            
	CUresult res = cuModuleLoadData(&module, buf );

	if(res != CUDA_SUCCESS)
	{
		printf("err");
	}

	res = cuModuleGetFunction(&updateParticlesKernel, module, "UpdateParticlesKernel");

	if(res != CUDA_SUCCESS)
	{
		printf("err");
	}

	delete[] buf;

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
	PxU32 validRange = rd->validParticleRange;

	CUresult result = positionBuffer->MapAndGetCudaGraphicsMemoryPointer();

	if(result != CUDA_SUCCESS)
	{
		printf("Get Map Graphics Resource FAILED: %i", result);
	}

	//positionBuffer->LaunchPhysxCopyKernel(dev);

	///Take in cuDevicePtrs for this function. In this function, perform Cuda memCpy Device to Device like in AddKernel.
	///Therefore we need the cuDevicePtrs and the buffer sizes
	//LaunchUpdateParticlesKernel((physx::PxVec3*)positionBuffer->GetCudaDevicePointer(), 12, (physx::PxVec4*)dev, (physx::PxU32*)d_ValidBitmap, validRange);

	CUdeviceptr p = positionBuffer->GetCudaDevicePointer();
	int stride = 12;
	CUdeviceptr d_stride;
	CUresult res = cuMemAlloc(&d_stride, sizeof(int));
	if(res!= CUDA_SUCCESS)
		printf("err");

	res = cuMemcpyHtoD(d_stride, &stride, sizeof(int));
	if(res!= CUDA_SUCCESS)
		printf("err");

	CUdeviceptr d_validRange;
	res = cuMemAlloc(&d_validRange, sizeof(PxU32));
	if(res!= CUDA_SUCCESS)
		printf("err");

	res = cuMemcpyHtoD(d_validRange, &validRange, sizeof(PxU32));
	if(res!= CUDA_SUCCESS)
		printf("err");

	void* args[2] = 
	{
		&p,
		&dev
	};

	/*void* args[5] = {
		(void*)&p,
		&d_stride,
		&dev,
		&d_ValidBitmap,
		&d_validRange
	};*/
	
	res = cuLaunchKernel(updateParticlesKernel, 1, 1, 1, 100, 1, 1, 0, 0, args, 0); 

	if(res != CUDA_SUCCESS)
	{
		printf("err");
	}

	Ogre::Vector3* pVertexPos = new Ogre::Vector3[particleCount];

	result = cuMemcpyDtoH_v2(pVertexPos, positionBuffer->GetCudaDevicePointer(), positionBuffer->GetBufferSize());

	
	
	if(result != CUDA_SUCCESS)
	{
		printf("Copy To Host FAILED: %i", result);
	}
	
	if(rd)
		rd->unlock();

	rd = particleSystem->lockParticleReadData(physx::PxDataAccessFlag::eREADABLE);

	if(rd)
	{
		physx::PxStrideIterator<const physx::PxParticleFlags> flagsIter(rd->flagsBuffer);
		physx::PxStrideIterator<const physx::PxVec3> positionIter (rd->positionBuffer);
		physx::PxStrideIterator<const physx::PxVec3> velocityIter (rd->velocityBuffer);
		
		for (unsigned int i = 0; i < rd->validParticleRange; ++i, ++flagsIter, ++velocityIter, ++positionIter)
		{
			if(*flagsIter & physx::PxParticleFlag::eVALID)
			{
				const physx::PxVec3& position = *positionIter;
				const physx::PxVec3& velocity = *velocityIter;

				printf("Position %i: %f, %f, %f \n", i, position.x, position.y, position.z);
				printf("OgrePosition %i: %f, %f, %f\n", i, pVertexPos[i].x, pVertexPos[i].y, pVertexPos[i].z);
			}
		}

		particleSystem->addForces(particleCount, physx::PxStrideIterator<physx::PxU32>(particleIndices),
			physx::PxStrideIterator<physx::PxVec3>(particleForces), physx::PxForceMode::eFORCE);
		/*particleSystem->setPositions(particleCount, physx::PxStrideIterator<physx::PxU32>(particleIndices),
			physx::PxStrideIterator<physx::PxVec3>(particleForces));*/

		rd->unlock();
	}

positionBuffer->UnmapFromCudaBuffer();
	owningScene->GetCudaContextManager()->releaseContext();

	delete particleForces;
	delete particleIndices;
	delete pVertexPos;
}

Ogre::Real ParticleRenderer::getBoundingRadius(void) const
{
	return 0;
}

Ogre::Real ParticleRenderer::getSquaredViewDepth(const Ogre::Camera *) const
{
	return 0;
}