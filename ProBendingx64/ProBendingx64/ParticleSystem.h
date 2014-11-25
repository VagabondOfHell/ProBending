#pragma once
#include "particles\PxParticleSystem.h"
#include "pxtask\PxCudaContextManager.h"
#include <vector>
#include "AbstractParticleEmitter.h"

struct ParticleSystemParams
{
	physx::PxReal gridSize;
	physx::PxReal maxMotionDistance;
	physx::PxReal particleMass;
	physx::PxReal staticFriction;
	physx::PxReal restitution;
	physx::PxReal contactOffset;
	physx::PxReal damping;
	physx::PxReal dynamicFriction;
	physx::PxVec3 externalAcceleration;
	physx::PxReal restOffset;
	physx::PxParticleBaseFlags baseFlags;

	///<summary>Constructor used to set all values</summary>
	///<param name="_gridSize">The size of the grid between 0 and 1000</param>
	///<param name="_maxMotionDistance">The maximum distance the particle is allowed to travel in a frame</param>
	///<param name="_externalAcceleration">The external acceleration to apply to all particles each frame</param>
	///<param name="_particleMass">The mass of each particle</param>
	///<param name="_baseFlags">The flags to configure the system</param>
	///<param name="_restOffset">The rest offset of the particles, if enabled </param>
	///<param name="_staticFriction">The friction used for collisions. Must be non-negative</param>
	///<param name="_dynamicFriction">The dynamic friction to use. Must be non-negative</param>
	///<param name="_restitution">The bounciness of the collision, between 0.0f and 1.0f</param>
	///<param name="_contactOffset">The distance that contacts are generated between particle and geometry</param>
	///<param name="_damping">Damping for particles. Must be non-negative</param>
	ParticleSystemParams(physx::PxReal _gridSize = 1.0f, physx::PxReal _maxMotionDistance = 2.0f,
		physx::PxVec3 _externalAcceleration = physx::PxVec3(0.0f), physx::PxReal _particleMass = 1.0f, 
		physx::PxParticleBaseFlags _baseFlags = physx::PxParticleBaseFlag::eENABLED,		
		physx::PxReal _restOffset = 1.0f, physx::PxReal _staticFriction = 0.0f, physx::PxReal _dynamicFriction = 0.0f,
		physx::PxReal _restitution = 1.0f, physx::PxReal _contactOffset = 0.0f, physx::PxReal _damping = 0.0f)
		
		:gridSize(_gridSize), maxMotionDistance(_maxMotionDistance), externalAcceleration(_externalAcceleration), 
		particleMass(_particleMass), baseFlags(_baseFlags), restOffset(_restOffset), staticFriction(_staticFriction), 
		dynamicFriction(_dynamicFriction), restitution(_restitution), contactOffset(_contactOffset), damping(_damping) 
		
	{
	}
};

template <class ParticlesPolicy>
class ParticleSystem
{
private:
	AbstractParticleEmitter* emitter;

	std::vector<physx::PxU32> availableIndices; //The available indices within the particle system

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
	///<summary>Constructor of the particle system</summary>
	///<param name="_particlePolicy">The instantiation of the particle policy. This particle system assumes control of it</param>
	///<param name="_emitter">The emitter used to create the particles</param>
	///<param name="_maximumParticles">The maximum allowed number of particles in the system</param>
	///<param name="readFlags">The flags to use for physX particle system</param>
	///<param name="params">Physx Custom parameters to easily set fields</params>
	///<param name="useCudaGPU">True to utilize the GPU, false if not</param>
	///<param name="_cudaContext">The cuda context to use with the GPU. If useCudaGPU is true, this must not be NULL </param>
	///<param name="perParticleRestOffset">True to use per particle rest offsets. This is passed to PhysX on creation of the system</param>
	ParticleSystem(ParticlesPolicy* _particlePolicy, AbstractParticleEmitter* _emitter, size_t _maximumParticles, 
		ParticleSystemParams params = ParticleSystemParams(), bool useCudaGPU = false, 
		physx::PxCudaContextManager* _cudaContext = NULL, bool perParticleRestOffset = false,
		physx::PxParticleReadDataFlags readFlags = physx::PxParticleReadDataFlag::ePOSITION_BUFFER 
		| physx::PxParticleReadDataFlag::eFLAGS_BUFFER)
	{
		maximumParticles = _maximumParticles;
		particlePolicy = _particlePolicy;

		onGPU = useCudaGPU;
		cudaContextManager = _cudaContext;

		emitter = _emitter;

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
		
		//If the current read data (the defaults) doesn't match the passed read flags, set as desired
		physx::PxParticleReadDataFlags currentFlags = pxParticleSystem->getParticleReadDataFlags();
		if(currentFlags != readFlags)
			SetParticleReadFlags(readFlags);

		//Set parameters
		pxParticleSystem->setGridSize(params.gridSize);
		pxParticleSystem->setMaxMotionDistance(params.maxMotionDistance);
		pxParticleSystem->setParticleMass(params.particleMass);
		pxParticleSystem->setRestitution(params.restitution);
		pxParticleSystem->setStaticFriction(params.staticFriction);
		pxParticleSystem->setContactOffset(params.contactOffset);
		pxParticleSystem->setDamping(params.damping);
		pxParticleSystem->setDynamicFriction(params.dynamicFriction);
		pxParticleSystem->setExternalAcceleration(params.externalAcceleration);
		
		//Allocate enough space for all the indices
		availableIndices.reserve(maximumParticles);

		//Add the indices in descending order
		for (int i = maximumParticles - 1; i >= 0; --i)
		{
			availableIndices.push_back(i);
		}
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
	inline ParticlesPolicy* const GetPolicy()const
	{
		return particlePolicy;
	}

	///<summary>Gets the emitter used by the particle system</summary>
	///<returns>Pointer to the abstract particle emitter</returns>
	inline AbstractParticleEmitter* const GetEmitter()const
	{
		return emitter;
	}

	///<summary>Gets the PhysX Particle System that this class wraps</summary>
	///<returns>The physx Particle system</returns>
	inline physx::PxParticleSystem* const GetPhysXParticleSystem()const
	{
		return pxParticleSystem;
	}

	///<summary>Updates the information of the particle system</summary>
	///<param name="time">The time step of the frame</param>
	void Update(float time)
	{
		using namespace physx;
		
		//We then call the update attributes for CPU readable data, even if the system utilizes the GPU
		PxParticleReadData* rd = pxParticleSystem->lockParticleReadData(PxDataAccessFlag::eREADABLE);
		
		std::vector<const unsigned int> indicesToRemove;

		if(rd)
		{
			//Update the policy collect the indices to remove from the policy
			indicesToRemove = particlePolicy->UpdatePolicy(time, rd, readableData);
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
					particlePolicy->ParticlesCreated(creationData.numParticles, creationData.indexBuffer);

					//Remove the particles from the available list
					for (int i = 0; i < creationData.numParticles; i++)
					{
						availableIndices.pop_back();
					}
				}
				else
					printf("CREATION ERROR");
			}	
		}
	}

};

