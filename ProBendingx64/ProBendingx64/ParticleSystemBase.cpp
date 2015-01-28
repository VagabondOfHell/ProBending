#include "ParticleSystemBase.h"
#include "PxPhysics.h"
#include "PsBitUtils.h"
#include "PxScene.h"
#include "AbstractParticleEmitter.h"
#include "OgreCamera.h"
#include "OgreHardwareBufferManager.h"
#include "ParticleAffectors.h"
#include "ParticleKernel.h"

using namespace physx;

ParticleMaterialMap ParticleSystemBase::materialsMap = ParticleMaterialMap();

ParticleKernelMap ParticleSystemBase::kernelsMap = ParticleKernelMap();

//Constructor for material map
ParticleMaterialMap::ParticleMaterialMap()
{
	materialMap.insert(MaterialMap::value_type(ParticleAffectorType::None, "DefaultParticleShader"));
	materialMap.insert(MaterialMap::value_type(ParticleAffectorType::Scale, "ScaleParticleShader"));
	materialMap.insert(MaterialMap::value_type(ParticleAffectorType::ColourToColour, "ColorParticleShader"));
	materialMap.insert(MaterialMap::value_type(ParticleAffectorType::Scale | ParticleAffectorType::ColourToColour, "ColorParticleShader"));
}

//Constructor for Kernel Map
ParticleKernelMap::ParticleKernelMap()
{
	std::shared_ptr<ParticleKernel>mainKernel = std::shared_ptr<ParticleKernel>(new ParticleKernel());
	kernelMap.insert(KernelMap::value_type(ParticleAffectorType::None, mainKernel));
	kernelMap.insert(KernelMap::value_type(ParticleAffectorType::Scale, mainKernel));
	kernelMap.insert(KernelMap::value_type(ParticleAffectorType::ColourToColour, mainKernel));
	kernelMap.insert(KernelMap::value_type(ParticleAffectorType::Scale | ParticleAffectorType::ColourToColour, mainKernel));
}


ParticleSystemBase::ParticleSystemBase(std::shared_ptr<AbstractParticleEmitter> _emitter, size_t _maximumParticles, 
		float _initialLifetime, ParticleSystemParams& paramsStruct)
		: emitter(_emitter), cudaContextManager(paramsStruct.cudaContext), initialLifetime(_initialLifetime)
{
	maximumParticles = _maximumParticles;

	// our vertices are just points
	mRenderOp.operationType = Ogre::RenderOperation::OT_POINT_LIST;
	mRenderOp.useIndexes = false;//EBO

	mRenderOp.vertexData = new Ogre::VertexData();
	mRenderOp.vertexData->vertexCount = maximumParticles;
	mRenderOp.vertexData->vertexBufferBinding->unsetAllBindings();
		
	//set ogre simple renderable
	mBox.setExtents(-1000, -1000, -1000, 1000, 1000, 1000);
	
	gpuTypeCombination = 0;
	allTypesCombination = 0;

	//Check for gpu usage validity
	if(cudaContextManager == NULL)
		onGPU = false;
	else
		onGPU = true;
	
	//Create the particle system on PhysX's end. Ignores per particle rest offset arguments here because set base flags handles it
	pxParticleSystem = PxGetPhysics().createParticleSystem(maximumParticles);
		
	//place the system on the GPU if it should be
	pxParticleSystem->setParticleBaseFlag(physx::PxParticleBaseFlag::eGPU, onGPU);
	
	//Check that PhysX didn't overwrite our GPU selection. If they did, reset our information
	if(!(pxParticleSystem->getParticleBaseFlags() & physx::PxParticleBaseFlag::eGPU))
	{
		onGPU = false;
		cudaContextManager = NULL;
	}

	cudaKernel = NULL;

	SetParticleBaseFlags(paramsStruct.baseFlags);

	//Set the gravity flag
	pxParticleSystem->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !paramsStruct.useGravity);
	
	//Set parameters
	pxParticleSystem->setGridSize(paramsStruct.gridSize);
	pxParticleSystem->setMaxMotionDistance(paramsStruct.maxMotionDistance);
	pxParticleSystem->setParticleMass(paramsStruct.particleMass);
	pxParticleSystem->setRestitution(paramsStruct.restitution);
	pxParticleSystem->setStaticFriction(paramsStruct.staticFriction);
	pxParticleSystem->setContactOffset(paramsStruct.contactOffset);
	pxParticleSystem->setDamping(paramsStruct.damping);
	pxParticleSystem->setDynamicFriction(paramsStruct.dynamicFriction);
	pxParticleSystem->setExternalAcceleration(paramsStruct.externalAcceleration);
	pxParticleSystem->setRestOffset(paramsStruct.restOffset);
	pxParticleSystem->setSimulationFilterData(paramsStruct.filterData);
	
	//Allocate enough space for all the indices
	availableIndices.reserve(maximumParticles);

	//Add the indices in descending order
	for (int i = maximumParticles - 1; i >= 0; --i)
	{
		availableIndices.push_back(i);
	}

	hostAffector = false;
}

ParticleSystemBase::ParticleSystemBase(physx::PxParticleSystem* physxParticleSystem, 
				std::shared_ptr<AbstractParticleEmitter> _emitter, size_t _maximumParticles, float _initialLifetime)
		: emitter(_emitter), initialLifetime(_initialLifetime)
{
	maximumParticles = _maximumParticles;

	// our vertices are just points
	mRenderOp.operationType = Ogre::RenderOperation::OT_POINT_LIST;
	mRenderOp.useIndexes = false;//EBO

	mRenderOp.vertexData = new Ogre::VertexData();
	mRenderOp.vertexData->vertexCount = maximumParticles;
	mRenderOp.vertexData->vertexBufferBinding->unsetAllBindings();

	//set ogre simple renderable
	mBox.setExtents(-1000, -1000, -1000, 1000, 1000, 1000);

	gpuTypeCombination = 0;
	allTypesCombination = 0;

	cudaKernel = NULL;

	pxParticleSystem = physxParticleSystem;

	readableData = pxParticleSystem->getParticleReadDataFlags();

	//Allocate enough space for all the indices
	availableIndices.reserve(maximumParticles);

	//Add the indices in descending order
	for (int i = maximumParticles - 1; i >= 0; --i)
	{
		availableIndices.push_back(i);
	}

	hostAffector = false;
}


ParticleSystemBase::~ParticleSystemBase(void)
{
	if(cudaKernel)
		delete cudaKernel;

	if(lifetimes)
	{
		delete[] lifetimes;
		lifetimes = NULL;
	}
}

void ParticleSystemBase::SetParticleReadFlags(physx::PxParticleReadDataFlags newFlags)
{
	using namespace physx;

	//Set all the flags as indicated by the newFlags variable
	pxParticleSystem->setParticleReadDataFlag(PxParticleReadDataFlag::ePOSITION_BUFFER, 
		newFlags & PxParticleReadDataFlag::ePOSITION_BUFFER);

	pxParticleSystem->setParticleReadDataFlag(PxParticleReadDataFlag::eVELOCITY_BUFFER, 
		newFlags & PxParticleReadDataFlag::eVELOCITY_BUFFER);

	pxParticleSystem->setParticleReadDataFlag(PxParticleReadDataFlag::eREST_OFFSET_BUFFER, 
		newFlags & PxParticleReadDataFlag::eREST_OFFSET_BUFFER);

	pxParticleSystem->setParticleReadDataFlag(PxParticleReadDataFlag::eFLAGS_BUFFER, 
		newFlags & PxParticleReadDataFlag::eFLAGS_BUFFER);

	pxParticleSystem->setParticleReadDataFlag(PxParticleReadDataFlag::eCOLLISION_NORMAL_BUFFER, 
		newFlags & PxParticleReadDataFlag::eCOLLISION_NORMAL_BUFFER);

	pxParticleSystem->setParticleReadDataFlag(PxParticleReadDataFlag::eCOLLISION_VELOCITY_BUFFER, 
		newFlags & PxParticleReadDataFlag::eCOLLISION_VELOCITY_BUFFER);

	pxParticleSystem->setParticleReadDataFlag(PxParticleReadDataFlag::eDENSITY_BUFFER, 
		newFlags & PxParticleReadDataFlag::eDENSITY_BUFFER);

	//Set the flags to the current data
	readableData = newFlags;
}

void ParticleSystemBase::SetParticleBaseFlags(physx::PxParticleBaseFlags newFlags)
{
	pxParticleSystem->setParticleBaseFlag(physx::PxParticleBaseFlag::eCOLLISION_TWOWAY, 
		newFlags & PxParticleBaseFlag::eCOLLISION_TWOWAY);

	pxParticleSystem->setParticleBaseFlag(physx::PxParticleBaseFlag::eCOLLISION_WITH_DYNAMIC_ACTORS, 
		newFlags & PxParticleBaseFlag::eCOLLISION_WITH_DYNAMIC_ACTORS);

	pxParticleSystem->setParticleBaseFlag(physx::PxParticleBaseFlag::eENABLED, 
		newFlags & PxParticleBaseFlag::eENABLED);

	pxParticleSystem->setParticleBaseFlag(physx::PxParticleBaseFlag::ePER_PARTICLE_COLLISION_CACHE_HINT, 
		newFlags & PxParticleBaseFlag::ePER_PARTICLE_COLLISION_CACHE_HINT);

	pxParticleSystem->setParticleBaseFlag(physx::PxParticleBaseFlag::ePER_PARTICLE_REST_OFFSET, 
		newFlags & PxParticleBaseFlag::ePER_PARTICLE_REST_OFFSET);

	pxParticleSystem->setParticleBaseFlag(physx::PxParticleBaseFlag::ePROJECT_TO_PLANE, 
		newFlags & PxParticleBaseFlag::ePROJECT_TO_PLANE);
}

Ogre::HardwareVertexBufferSharedPtr ParticleSystemBase::CreateVertexBuffer(Ogre::VertexElementSemantic semantic, unsigned short uvSource)
{
	Ogre::HardwareVertexBufferSharedPtr returnVal = GetBuffer(semantic);
	physx::PxVec4* lockedData;

	//If it already exists, return it
	if(!returnVal.isNull())
		return returnVal;

	bool success = false;

	switch (semantic)
	{
	case Ogre::VES_POSITION:
	case Ogre::VES_BLEND_WEIGHTS:
	case Ogre::VES_BLEND_INDICES:
	case Ogre::VES_DIFFUSE:
	case Ogre::VES_SPECULAR:
		//Bind to the underlying semantic value
		mRenderOp.vertexData->vertexDeclaration->addElement(semantic, 0, Ogre::VET_FLOAT4, semantic);

		returnVal = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT4),
			maximumParticles,
			Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE,
			false);

		success = true;

		lockedData = (physx::PxVec4*)returnVal->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
		for (int i = 0; i < maximumParticles; i++)
		{
			lockedData[i].x = 0.0f;
			lockedData[i].y = 0.0f;
			lockedData[i].z = 0.0f;
			lockedData[i].w = 1.0f;
		}
		returnVal->unlock();
		break;
	
	case Ogre::VES_BINORMAL:
	case Ogre::VES_TANGENT:
	case Ogre::VES_NORMAL:
		//Bind to the underlying semantic value
		mRenderOp.vertexData->vertexDeclaration->addElement(semantic, 0, Ogre::VET_FLOAT3, semantic);

		returnVal = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3),
			maximumParticles,
			Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE,
			false);

		success = true;
		break;
	
	case Ogre::VES_TEXTURE_COORDINATES:
		//Bind to the underlying semantic value
		mRenderOp.vertexData->vertexDeclaration->addElement(semantic, 0, Ogre::VET_FLOAT4, semantic, uvSource);

		returnVal = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT4),
			maximumParticles,
			Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE,
			false);

		success = true;
		break;
	

	default:
		break;
	}

	if(success)
	{
		BufferMapInsertResult result = bufferMap.insert(BufferMap::value_type(semantic, returnVal));

		if(result.second = true)
		{
			//Set the binding to the semantic value
			mRenderOp.vertexData->vertexBufferBinding->setBinding(semantic, returnVal);
			return returnVal;
		}
	}

	return Ogre::HardwareVertexBufferSharedPtr(NULL);
}

void ParticleSystemBase::InitializeVertexBuffers()
{
	using namespace physx;

	// allocate the vertex buffer
	mVertexBufferPosition = CreateVertexBuffer(Ogre::VES_POSITION);
	
	float* positions = (float*)(mVertexBufferPosition->lock(Ogre::HardwareBuffer::LockOptions::HBL_NORMAL));
	for (unsigned int i = 0; i < maximumParticles; i++)
	{
		positions[i] = std::numeric_limits<float>::quiet_NaN();
	}
	mVertexBufferPosition->unlock();

	//Set the vertex count to the maximum allowed particles in case we are running on the CPU
	//Otherwise GPU update will override
	//Eventually change this so CPU only tries to draw the number of valid particles, similar to GPU
	mRenderOp.vertexData->vertexCount = maximumParticles;
}

void ParticleSystemBase::Initialize(physx::PxScene* scene)
{
	lifetimes = new float[maximumParticles];
	
	//Reserve 5% of the max particles for removal
	indicesToRemove.reserve(maximumParticles * 0.05f);

	//Call child initialize
	InitializeParticleSystemData();

	InitializeVertexBuffers();

	//Add it to the scene
	scene->addActor(*pxParticleSystem);
}

void ParticleSystemBase::Update(float time)
{
	using namespace physx;
	PxParticleReadData* rd;
	
	//We then call the update attributes for CPU readable data, even if the system utilizes the GPU
	rd = pxParticleSystem->lockParticleReadData(PxDataAccessFlag::eREADABLE);

	if(rd)
	{
		//Update the policy collect the indices to remove from the policy
		UpdateParticleSystemCPU(time, rd);
		rd->unlock();
	}
	//If we have the particle system using the GPU
	if(onGPU)			
		if(cudaContextManager)
		{
			//Prepare the data on the GPU
			cudaContextManager->acquireContext();
			rd = pxParticleSystem->lockParticleReadData(PxDataAccessFlag::eDEVICE);

			if(rd)
			{
				if(!cudaKernel->LaunchKernel(rd, lifetimes, initialLifetime, maximumParticles))
					printf("Cuda Launch Failed\n");
				//Call the policies update GPU method
				//UpdatePolicyGPU(time, rd);
				rd->unlock();
			}
			//release the cuda context
			cudaContextManager->releaseContext();
		}
	
		//If we should remove some, remove them
	if(indicesToRemove.size() > 0)
	{
		pxParticleSystem->releaseParticles(indicesToRemove.size(), PxStrideIterator<PxU32>(&indicesToRemove[0]));

		for (int i = indicesToRemove.size() - 1; i >= 0; --i)
		{
			availableIndices.push_back(indicesToRemove[i]);
			indicesToRemove.pop_back();
		}
	}

	//Create the emission data and initialize number to 0
	PxParticleCreationData creationData; 
	creationData.numParticles = 0;

	//Indicate the emitter should emit
	emitter->Emit(time, availableIndices.size(), creationData);

	if(creationData.numParticles > 0)
	{
		//Fill the index buffer
		creationData.indexBuffer = physx::PxStrideIterator<PxU32>(&availableIndices[availableIndices.size() - creationData.numParticles]);

		//Check validity
		if(creationData.isValid())
		{
			//Create the particles
			if(pxParticleSystem->createParticles(creationData))
			{
				//Callback for successful creation
				ParticlesCreated(creationData.numParticles, creationData.indexBuffer);

				//Remove the particles from the available list
				for (unsigned int i = 0; i < creationData.numParticles; i++)
				{
					availableIndices.pop_back();
				}
			}
			else
				printf("CREATION ERROR");
		}	
	}
}

GPUResourcePointers ParticleSystemBase::LockBuffersCPU()
{
	GPUResourcePointers pointers;

	for (BufferMap::iterator start = bufferMap.begin(); start != bufferMap.end(); ++start)
	{
		switch (start->first)
		{
		case Ogre::VES_POSITION:
			pointers.positions = (physx::PxVec4*)start->second->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
			break;
		case Ogre::VES_BLEND_WEIGHTS:
			pointers.blendWeights = (physx::PxVec4*)start->second->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
			break;
		case Ogre::VES_BLEND_INDICES:
			pointers.blendIndices = (physx::PxVec4*)start->second->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
			break;
		case Ogre::VES_DIFFUSE:
			pointers.primaryColour = (physx::PxVec4*)start->second->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
			break;
		case Ogre::VES_SPECULAR:
			pointers.secondaryColour = (physx::PxVec4*)start->second->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
			break;
		case Ogre::VES_TEXTURE_COORDINATES:
			pointers.uv0 = (physx::PxVec4*)start->second->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
			break;

		case Ogre::VES_BINORMAL:
			pointers.binormals = (physx::PxVec3*)start->second->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
			break;
		case Ogre::VES_TANGENT:
			pointers.tangent = (physx::PxVec3*)start->second->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
			break;
		case Ogre::VES_NORMAL:
			pointers.normals = (physx::PxVec3*)start->second->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
			break;

		default:
			break;
		}
	}

	return pointers;
}

void ParticleSystemBase::UnlockBuffersCPU()
{
	for (BufferMap::iterator start = bufferMap.begin(); start != bufferMap.end(); ++start)
	{
		start->second->unlock();
	}
}

void ParticleSystemBase::UpdateParticleSystemCPU(const float time, const physx::PxParticleReadData* const readData)
{
	using namespace physx;

	//Check if there is any updating to do
	if (readData->validParticleRange > 0)
	{
		GPUResourcePointers lockedBufferData;
		int numParticles(0);

		if(hostAffector)
		{
			lockedBufferData = LockBuffersCPU();

			//Lock Ogre GL Buffers if not using CUDA so we can update them
			for (AffectorMap::iterator start = affectorMap.begin(); start != affectorMap.end(); ++start)
			{
				//Apply affectors if on CPU
				start->second->PreUpdate();
			}
		}
		
		for (PxU32 w = 0; w <= (readData->validParticleRange-1) >> 5; w++)
		{
			//For each bit of the bitmap
			for (PxU32 b = readData->validParticleBitmap[w]; b; b &= b-1)
			{
				PxU32 index = (w << 5 | shdfnd::lowestSetBit(b));

				++numParticles;

				//Check particle validity
				if(QueryParticleRemoval(index, readData))
				{
					//If lifetime is equal or below zero
					indicesToRemove.push_back(index); //indicate removal
					lifetimes[index] = 0;//set lifetime to 0
				
					if(!onGPU)
						//Reset position. Not concerned of W component here because its a dead particle anyways
						lockedBufferData.positions[index] = PxVec4(std::numeric_limits<float>::quiet_NaN());

					continue;
				}
				//If the particle is valid and lifetimes are above 0, subtract this frame
				else
				{
					lifetimes[index] -= time;

					if(!onGPU)
					{
						// copy particle positions over
						const PxVec3& position = readData->positionBuffer[index];
						lockedBufferData.positions[index].x = position.x;
						lockedBufferData.positions[index].y = position.y;
						lockedBufferData.positions[index].z = position.z;
					}
					
					if(hostAffector)
					{
						float percentile = lifetimes[index] / initialLifetime;
						for (AffectorMap::iterator start = affectorMap.begin(); start != affectorMap.end(); ++start)
						{
							//Apply affectors if on CPU
							start->second->Update(time, lockedBufferData, percentile, index);								
						}
					}
					
					//Allow children to update their own particle data
					UpdateParticle(index, readData);
				}
			}//end of bitmap for loop (b)
		}//end of particle range for loop

		//If this system is on the CPU, unlock the vertex buffer
		if(hostAffector)
		{
			UnlockBuffersCPU();
			for (AffectorMap::iterator start = affectorMap.begin(); start != affectorMap.end(); ++start)
			{
				//Apply affectors if on CPU
				start->second->PostUpdate();								
			}
		}
		
		if(onGPU)
			mRenderOp.vertexData->vertexCount = numParticles;
	}//end if valid range > 0
}


bool ParticleSystemBase::QueryParticleRemoval(const unsigned int particleIndex, const physx::PxParticleReadData* const readData)
{
	return readData->flagsBuffer[particleIndex] & PxParticleFlag::eSPATIAL_DATA_STRUCTURE_OVERFLOW ||
		!(readData->flagsBuffer[particleIndex] & PxParticleFlag::eVALID) || lifetimes[particleIndex] <= 0.0f;
}


void ParticleSystemBase::ParticlesCreated(const unsigned int createdCount, physx::PxStrideIterator<const physx::PxU32> emittedIndices)
{
	//Loop through all the created particles and set their lifetimes
	for (unsigned int i = 0; i < createdCount; i++)
	{
		lifetimes[emittedIndices[i]] = initialLifetime;
	}
}

bool ParticleSystemBase::AddAffector(std::shared_ptr<ParticleAffector> affectorToAdd)
{
	//Try to insert and return results (if it exists, it will return false, so no need to perform a find first)
	AffectorMapInsertResult result = affectorMap.insert(AffectorMap::value_type(affectorToAdd->GetAffectorType(), affectorToAdd));//Try to insert

	//if successful
	if(result.second)
	{
		//update flag for CPU and GPU affectors
		allTypesCombination |= result.first->first;
		//if system and affector on GPU, update GPU flag
		if(onGPU && affectorToAdd->GetOnGPU())
		{
			gpuTypeCombination |= result.first->first;
		}
		else
		{
			hostAffector = true;
			affectorToAdd->onGPU = false;
		}

		affectorToAdd->Initialize(this);
	}
	return result.second;
}

bool ParticleSystemBase::RemoveAffector(ParticleAffectorType::ParticleAffectorType typeToRemove)
{
	return RemoveAndGetAffector(typeToRemove).get() != NULL;
}

std::shared_ptr<ParticleAffector> ParticleSystemBase::RemoveAndGetAffector(ParticleAffectorType::ParticleAffectorType typeToRemove)
{
	AffectorMap::iterator result = affectorMap.find(typeToRemove);

	if(result != affectorMap.end())
	{
		//Update flag for CPU and GPU
		allTypesCombination &= result->first;

		//Update flag for GPU if system and affector are on GPU
		if(onGPU && result->second->GetOnGPU())
			gpuTypeCombination &= result->first;

		Ogre::VertexElementSemantic semantic = result->second->GetDesiredBuffer();
		bool anotherFound = false;

		//Check if any other affectors use the buffer the removed affector did
		for (AffectorMap::iterator start = affectorMap.begin();start != affectorMap.end();++start)
		{
			if(start->second->GetDesiredBuffer() == semantic)
			{
				anotherFound = true;
				break;
			}
		}

		if(!anotherFound)//If no other affector is using the buffer, destroy it
		{
			mRenderOp.vertexData->vertexBufferBinding->unsetBinding(semantic);
			mRenderOp.vertexData->vertexDeclaration->removeElement(semantic);
		}

		affectorMap.erase(result);//remove from map if found

		return result->second; //return the affector
	}

	return NULL;
}

bool ParticleSystemBase::AssignAffectorKernel(ParticleKernel* newKernel)
{
	if(onGPU)
	{
		if(cudaKernel)
		{
			delete cudaKernel;
			cudaKernel = NULL;
		}

		cudaKernel = newKernel;

		if(cudaKernel)
		{
			//Fill the kernel with the necessary data
			if(cudaKernel->PopulateData(this, &affectorMap) == ParticleKernel::SUCCESS)
			{
				return true;
			}
		}
	}

	return false;
}

Ogre::Real ParticleSystemBase::getSquaredViewDepth(const Ogre::Camera* cam)const
{
		Ogre::Vector3 min, max, mid, dist;
		min = mBox.getMinimum();
		max = mBox.getMaximum();
		mid = ((max - min) * 0.5) + min;
		dist = cam->getDerivedPosition() - mid;
		return dist.squaredLength();
}

std::string ParticleSystemBase::FindBestShader()
{
	ParticleMaterialMap::MaterialMap::iterator iter = materialsMap.materialMap.find(allTypesCombination);

	if(iter != materialsMap.materialMap.end())
		return iter->second;
	else
	{
		iter = materialsMap.materialMap.find(gpuTypeCombination);
		if(iter != materialsMap.materialMap.end())
			return iter->second;
	}

	return "DefaultParticleShader";
}

std::string ParticleSystemBase::FindBestShader(ParticleAffectorType::ParticleAffectorFlag combination)
{
	ParticleMaterialMap::MaterialMap::iterator iter = materialsMap.materialMap.find(combination);

	if(iter != materialsMap.materialMap.end())
		return iter->second;

	return "DefaultParticleShader";
}

ParticleKernel* ParticleSystemBase::FindBestKernel()
{
	ParticleKernelMap::KernelMap::iterator iter = kernelsMap.kernelMap.find(gpuTypeCombination);

	if(iter != kernelsMap.kernelMap.end())
		return iter->second->Clone();

	return NULL;
}

ParticleKernel* ParticleSystemBase::FindBestKernel(ParticleAffectorType::ParticleAffectorFlag combination)
{
	ParticleKernelMap::KernelMap::iterator iter = kernelsMap.kernelMap.find(combination);

	if(iter != kernelsMap.kernelMap.end())
		return iter->second->Clone();

	return NULL;
}

Ogre::HardwareVertexBufferSharedPtr ParticleSystemBase::GetBuffer(Ogre::VertexElementSemantic semantic)
{
	BufferMap::iterator iter = bufferMap.find(semantic);

	if(iter != bufferMap.end())
		return iter->second;
	else
		return Ogre::HardwareVertexBufferSharedPtr(NULL);
}
