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

	ParticleSystem(ParticlesPolicy* _particlePolicy, physx::PxParticleReadDataFlags readFlags, size_t _maximumParticles, 
		bool useCudaGPU = false, physx::PxCudaContextManager* _cudaContext = NULL, bool perParticleRestOffset = false)
	{
		maximumParticles = _maximumParticles;
		particlePolicy = _particlePolicy;

		onGPU = useCudaGPU;
		cudaContextManager = _cudaContext;

		//Create the particle system on PhysX's end
		pxParticleSystem = PxGetPhysics().createParticleSystem(maximumParticles, perParticleRestOffset);
		
		//place the system on the GPU if it should be
		pxParticleSystem->setParticleBaseFlag(physx::PxParticleBaseFlag::eGPU, onGPU);
		
		physx::PxParticleReadDataFlags currentFlags = pxParticleSystem->getParticleReadDataFlags();

		pxParticleSystem->setMaxMotionDistance(10.0f);

		//If the current read data (the defaults) doesn't match the passed read flags, set as desired
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

		if(onGPU && cudaContextManager != NULL)
			particlePolicy->InitializeGPUData(cudaContextManager);

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

		PxParticleCreationData* creationData =  particlePolicy->Emit();

		if(creationData)
		{
			if(creationData->isValid())
			{
				bool result = pxParticleSystem->createParticles(*creationData);
			}

			delete creationData;
			creationData = NULL;
		}
		
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
				cudaContextManager->acquireContext();
				rd = pxParticleSystem->lockParticleReadData(PxDataAccessFlag::eDEVICE);

				if(rd)
				{
					particlePolicy->UpdatePolicyGPU(time, rd, readableData);
					rd->unlock();
				}

				std::vector<unsigned int> indicesToRemove = particlePolicy->ReleaseParticles();

				if(indicesToRemove.size() > 0)
					pxParticleSystem->releaseParticles(indicesToRemove.size(), PxStrideIterator<PxU32>(&indicesToRemove[0]));
		
				cudaContextManager->releaseContext();
			}
	}

};

