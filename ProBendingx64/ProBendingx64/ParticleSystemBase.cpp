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


ParticleSystemBase::ParticleSystemBase(AbstractParticleEmitter* _emitter, size_t _maximumParticles, 
		float _initialLifetime, ParticleSystemParams& paramsStruct,	bool _ownEmitter)
		: emitter(_emitter), ownEmitter(_ownEmitter), cudaContextManager(paramsStruct.cudaContext), initialLifetime(_initialLifetime),
		nextVertexElement(0)
{
	// our vertices are just points
	mRenderOp.operationType = Ogre::RenderOperation::OT_POINT_LIST;
	mRenderOp.useIndexes = false;//EBO

	mRenderOp.vertexData = new Ogre::VertexData();
	mRenderOp.vertexData->vertexCount = maximumParticles;
	mRenderOp.vertexData->vertexBufferBinding->unsetAllBindings();
		
	//set ogre simple renderable
	mBox.setExtents(-1000, -1000, -1000, 1000, 1000, 1000);
	
	maximumParticles = _maximumParticles;

	gpuTypeCombination = 0;
	allTypesCombination = 0;

	//Check for gpu usage validity
	if(cudaContextManager == NULL)
		onGPU = false;
	else
		onGPU = true;
		
	//Create the particle system on PhysX's end
	pxParticleSystem = PxGetPhysics().createParticleSystem(maximumParticles, paramsStruct.perParticleRestOffset);
		
	//place the system on the GPU if it should be
	pxParticleSystem->setParticleBaseFlag(physx::PxParticleBaseFlag::eGPU, onGPU);
		
	//Check that PhysX didn't overwrite our GPU selection. If they did, reset our information
	if(!(pxParticleSystem->getParticleBaseFlags() & physx::PxParticleBaseFlag::eGPU))
	{
		onGPU = false;
		cudaContextManager = NULL;
	}

	cudaKernel = NULL;

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
		
	//Allocate enough space for all the indices
	availableIndices.reserve(maximumParticles);

	//Add the indices in descending order
	for (int i = maximumParticles - 1; i >= 0; --i)
	{
		availableIndices.push_back(i);
	}
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

	if(ownEmitter)
		if(emitter)
			delete emitter;
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

void ParticleSystemBase::InitializeVertexBuffers()
{
	using namespace physx;

	size_t currOffset = 0;

	mRenderOp.vertexData->vertexDeclaration->addElement(nextVertexElement, currOffset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
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
	mRenderOp.vertexData->vertexBufferBinding->setBinding(nextVertexElement, mVertexBufferPosition);

	++nextVertexElement;
	//Set the vertex count to the maximum allowed particles in case we are running on the CPU
	//Otherwise GPU update will override
	mRenderOp.vertexData->vertexCount = maximumParticles;
}

void ParticleSystemBase::Initialize(physx::PxScene* scene)
{
	lifetimes = new float[maximumParticles];

	//loop through, indicate available indices, and initialize all lifetimes to 0
	for (int i = maximumParticles - 1; i >= 0; --i)
	{
		lifetimes[i] = 0.0f;
	}
	
	//Reserve 5% of the max particles for removal
	indicesToRemove.reserve(maximumParticles * 0.05f);

	//Call child initialize
	InitializeParticleSystemData();

	InitializeVertexBuffers();

	//If we have GPU info, allow the policy to initialize its GPU data
	if(onGPU && cudaContextManager != NULL)
		onGPU = AssignAffectorKernel(FindBestKernel());

	//Find a shader for the combination of all current affectors
	setMaterial(FindBestShader());

	//Add it to the scene
	scene->addActor(*pxParticleSystem);
}

void ParticleSystemBase::Update(float time)
{
	using namespace physx;
	PxParticleReadData* rd;

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
		
	//We then call the update attributes for CPU readable data, even if the system utilizes the GPU
	rd = pxParticleSystem->lockParticleReadData(PxDataAccessFlag::eREADABLE);

	if(rd)
	{
		//Update the policy collect the indices to remove from the policy
		UpdateParticleSystemCPU(time, rd);
		rd->unlock();
	}

	//If we should remove some, remove them
	if(indicesToRemove.size() > 0)
	{
		pxParticleSystem->releaseParticles(indicesToRemove.size(), PxStrideIterator<PxU32>(&indicesToRemove[0]));

		for (int i = indicesToRemove.size() - 1; i >= 0; --i)
		{
			availableIndices.push_back(indicesToRemove[i]);
		}

		indicesToRemove.clear();
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

void ParticleSystemBase::UpdateParticleSystemCPU(const float time, const physx::PxParticleReadData* const readData)
{
	using namespace physx;

	//Check if there is any updating to do
	if (readData->validParticleRange > 0)
	{
		PxVec3* positions = NULL;

		int numParticles(0);

		//Lock Ogre GL Buffers if not using CUDA so we can update them
		if(!onGPU)
				positions = static_cast<PxVec3*>(mVertexBufferPosition->lock(Ogre::HardwareBuffer::LockOptions::HBL_WRITE_ONLY));

		for (PxU32 w = 0; w <= (readData->validParticleRange-1) >> 5; w++)
		{
			//For each bit of the bitmap
			for (PxU32 b = readData->validParticleBitmap[w]; b; b &= b-1)
			{
				PxU32 index = (w << 5 | shdfnd::lowestSetBit(b));

				//Check particle validity
				if(QueryParticleRemoval(index, readData))
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

					if(!onGPU)
					{
						// copy particle positions over
						const PxVec3& position = readData->positionBuffer[index];
						positions[index] = position;

					}

					float percentile = lifetimes[index] / initialLifetime;
					for (AffectorMapIterator start = affectorMap.begin(); start != affectorMap.end(); ++start)
					{
						//Apply affectors if on CPU
						start->second->Update(time, percentile, index);
					}

					//Allow children to update their own particle data
					UpdateParticle(index, readData);
					++numParticles;
				}
			}//end of bitmap for loop (b)
		}//end of particle range for loop

		//If this system is on the CPU, unlock the vertex buffer
		if(!onGPU)
				mVertexBufferPosition->unlock();
		else
			mRenderOp.vertexData->vertexCount = numParticles;

		for (AffectorMapIterator start = affectorMap.begin(); start != affectorMap.end(); ++start)
		{
			//Apply affectors if on CPU
			start->second->UnlockBuffers();
		}
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

bool ParticleSystemBase::AddAffector(ParticleAffector* affectorToAdd)
{
	//Try to insert and return results (if it exists, it will return false, so no need to perform a find first)
	AffectorMapInsertResult result = affectorMap.insert(AffectorMapValue(affectorToAdd->GetAffectorType(), affectorToAdd));//Try to insert

	//if successful
	if(result.second)
	{
		//update flag for CPU and GPU affectors
		allTypesCombination |= result.first->first;
		//if system and affector on GPU, update GPU flag
		if(onGPU && affectorToAdd->GetOnGPU())
			gpuTypeCombination |= result.first->first;

		affectorToAdd->Initialize(cudaContextManager, mRenderOp.vertexData, maximumParticles, nextVertexElement);
		++nextVertexElement;
	}
	return result.second;
}

bool ParticleSystemBase::RemoveAffector(ParticleAffectorType::ParticleAffectorType typeToRemove)
{
	ParticleAffector* affectorToDelete = RemoveAndGetAffector(typeToRemove);

	if(affectorToDelete)//if found
	{
		delete affectorToDelete;//delete and return true
		return true;
	}

	return false;
}

ParticleAffector* ParticleSystemBase::RemoveAndGetAffector(ParticleAffectorType::ParticleAffectorType typeToRemove)
{
	AffectorMapIterator result = affectorMap.find(typeToRemove);

	if(result != affectorMap.end())
	{
		//Update flag for CPU and GPU
		allTypesCombination &= result->first;

		//Update flag for GPU if system and affector are on GPU
		if(onGPU && result->second->GetOnGPU())
			gpuTypeCombination &= result->first;

		unsigned int source = result->second->GetBindingIndex();

		mRenderOp.vertexData->vertexBufferBinding->unsetBinding(source);
		mRenderOp.vertexData->vertexDeclaration->removeElement(source);

		affectorMap.erase(result);//remove from map if found

		return result->second; //return the affector
	}

	return NULL;
}

bool ParticleSystemBase::AssignAffectorKernel(ParticleKernel* newKernel)
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
		if(cudaKernel->PopulateData(cudaContextManager, 
			mVertexBufferPosition, maximumParticles, &affectorMap) == ParticleKernel::SUCCESS)
		{
			return true;
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