#pragma once
#include "particles\PxParticleSystem.h"
#include "pxtask\PxCudaContextManager.h"

template <class ParticlesPolicy>
class ParticleSystem
{
private:
	
	physx::PxParticleSystem* pxParticleSystem; ///The physX particle system
	physx::PxCudaContextManager* cudaContextManager; ///The physX cuda context manager

	ParticlesPolicy* particlePolicy; ///The policy providing customization to the particle system

	size_t maximumParticles; ///The maximum number of particles allowed in the particle system
	bool onGPU; ///Whether or not the particle system is used on the GPU

	physx::PxParticleReadDataFlags readableData; ///The readable data of the particle system

	///<summary>Sets the read data flags as specified by the newFlags argument</summary>
	///<param name="newFlags">The new read data flags to use</param>
	inline void SetParticleReadFlags(physx::PxParticleReadDataFlags newFlags)
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

public:
	ParticleSystem(void)
	{
		maximumParticles = 0;
		onGPU = false;

		particlePolicy = NULL;
	}

	///<summary>Constructor of the particle system</summary>
	///<param name="_particlePolicy">The instantiation of the particle policy. This particle system assumes control of it</param>
	///<param name="readFlags">The flags to use for physX particle system</param>
	///<param name="_maximumParticles">The maximum allowed number of particles in the system</param>
	///<param name="useCudaGPU">True to utilize the GPU, false if not</param>
	///<param name="_cudaContext">The cuda context to use with the GPU. If useCudaGPU is true, this must not be NULL </param>
	///<param name="perParticleRestOffset">True to use per particle rest offsets. This is passed to PhysX on creation of the system</param>
	ParticleSystem(ParticlesPolicy* _particlePolicy, physx::PxParticleReadDataFlags readFlags, size_t _maximumParticles, 
		bool useCudaGPU = false, physx::PxCudaContextManager* _cudaContext = NULL, bool perParticleRestOffset = false)
	{
		maximumParticles = _maximumParticles;
		particlePolicy = _particlePolicy;

		onGPU = useCudaGPU;
		cudaContextManager = _cudaContext;

		//Check for gpu usage validity
		if(cudaContextManager == NULL)
			onGPU = false;

		//Create the particle system on PhysX's end
		pxParticleSystem = PxGetPhysics().createParticleSystem(maximumParticles, perParticleRestOffset);
		
		//place the system on the GPU if it should be
		pxParticleSystem->setParticleBaseFlag(physx::PxParticleBaseFlag::eGPU, onGPU);
		
		//Check that PhysX didn't overwrite our GPU selection. If they did, reset our information
		if(!(pxParticleSystem->getParticleBaseFlags() & physx::PxParticleBaseFlag::eGPU))
		{
			onGPU = false;
			cudaContextManager = NULL;
		}

		//The maximum motion distance. Set to a default of 10. The policy has access to this, so they can customize it from there
		pxParticleSystem->setMaxMotionDistance(10.0f);
		pxParticleSystem->setGridSize(400);

		//If the current read data (the defaults) doesn't match the passed read flags, set as desired
		physx::PxParticleReadDataFlags currentFlags = pxParticleSystem->getParticleReadDataFlags();
		if(currentFlags != readFlags)
			SetParticleReadFlags(readFlags);
	}

	~ParticleSystem(void)
	{
		if(particlePolicy)
			delete particlePolicy;
	}

	///<summary>Initializes particle system, particle policy, and attaches to the specified scene</summary>
	///<param name="scene">The physX scene to add the particle system to</param>
	void Initialize(physx::PxScene* scene)
	{
		particlePolicy->Initialize(maximumParticles, pxParticleSystem);

		//If we have GPU info, allow the policy to initialize its GPU data
		if(onGPU && cudaContextManager != NULL)
			particlePolicy->InitializeGPUData(cudaContextManager);

		//Add it to the scene
		scene->addActor(*pxParticleSystem);
	}

	///<summary>Gets the maximum number of particles allowed to be managed by the particle system at a time</summary>
	///<returns>The maximum number of particles, as declared when constructing the particle system</returns>
	inline size_t GetMaximumParticlesAllowed()
	{
		return maximumParticles;
	}

	///<summary>Gets the policy used by the particle system</summary>
	///<returns>Pointer to the policy being used</returns>
	inline const ParticlesPolicy* const GetPolicy()const
	{
		return particlePolicy;
	}

	///<summary>Updates the information of the particle system</summary>
	///<param name="time">The time step of the frame</param>
	void Update(float time)
	{
		using namespace physx;
		
		//We then call the update attributes for CPU readable data, even if the system utilizes the GPU
		PxParticleReadData* rd = pxParticleSystem->lockParticleReadData(PxDataAccessFlag::eREADABLE);
		
		if(rd)
		{
			//Update the policy
			particlePolicy->UpdatePolicy(time, rd, readableData);
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
					particlePolicy->UpdatePolicyGPU(time, rd, readableData);
					rd->unlock();
				}
				//release the cuda context
				cudaContextManager->releaseContext();
			}

		//Collect the indices to remove from the policy
		std::vector<unsigned int> indicesToRemove = particlePolicy->ReleaseParticles();

		//If we should remove some, remove them
		if(indicesToRemove.size() > 0)
			pxParticleSystem->releaseParticles(indicesToRemove.size(), PxStrideIterator<PxU32>(&indicesToRemove[0]));

		//Get emission data from the policy
		PxParticleCreationData* creationData =  particlePolicy->Emit(time);

		//If creation data exists
		if(creationData)
		{
			//Check validity
			if(creationData->isValid())
			{
				//Create the particles. Don't catch the results, because we won't be doing anything with them
				if(pxParticleSystem->createParticles(*creationData))
					particlePolicy->ParticlesCreated(creationData->numParticles);
				else
					printf("CREATION ERROR");
			}
			//Clean up the creation data
			delete creationData;
			creationData = NULL;
		}
	}

};

