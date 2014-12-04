#include "ParticleSystemBase.h"
#include "PxPhysics.h"
#include "PxScene.h"
#include "AbstractParticleEmitter.h"
#include "OgreCamera.h"

using namespace physx;

ParticleSystemBase::ParticleSystemBase(AbstractParticleEmitter* _emitter, size_t _maximumParticles, 
		ParticleSystemParams& paramsStruct,	bool _ownEmitter)
		: emitter(_emitter), ownEmitter(_ownEmitter), cudaContextManager(paramsStruct.cudaContext)
{
		//set ogre simple renderable
		mBox.setExtents(-1000, -1000, -1000, 1000, 1000, 1000);

		maximumParticles = _maximumParticles;

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

void ParticleSystemBase::Initialize(physx::PxScene* scene)
{
	//Call child initialize
	InitializeParticleSystemData();

	//If we have GPU info, allow the policy to initialize its GPU data
	if(onGPU && cudaContextManager != NULL)
		InitializeGPUData();

	//Add it to the scene
	scene->addActor(*pxParticleSystem);
}

void ParticleSystemBase::Update(float time)
{
	using namespace physx;
		
	//We then call the update attributes for CPU readable data, even if the system utilizes the GPU
	PxParticleReadData* rd = pxParticleSystem->lockParticleReadData(PxDataAccessFlag::eREADABLE);
		
	std::vector<const unsigned int> indicesToRemove;

	if(rd)
	{
		//Update the policy collect the indices to remove from the policy
		indicesToRemove = UpdatePolicy(time, rd);
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
				//Call the policies update GPU method
				UpdatePolicyGPU(time, rd);
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

Ogre::Real ParticleSystemBase::getSquaredViewDepth(const Ogre::Camera* cam)const
{
		Ogre::Vector3 min, max, mid, dist;
		min = mBox.getMinimum();
		max = mBox.getMaximum();
		mid = ((max - min) * 0.5) + min;
		dist = cam->getDerivedPosition() - mid;
		return dist.squaredLength();
}