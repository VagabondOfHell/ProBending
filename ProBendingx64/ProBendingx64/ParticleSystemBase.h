#pragma once
#include "FluidAndParticleBase.h"
#include "AffectorData.h"
#include <vector>

class ParticleSystemBase: public FluidAndParticleBase
{
	friend class SceneSerializer;

protected:
	
	physx::PxParticleSystem* pxParticleSystem; ///The physX particle system
			
	AffectorData affectors;

#pragma region Virtual Methods for Inherited System Customization
	
	///<summary>Called before updating to allow children to lock extra buffers, or other pre-update actions</summary>
	virtual void PreUpdateCPU(){}

	///<summary>Updates the policy. Any CPU focused updating should go here. Returns a vector of indices that represent
	///the indices that should be removed</summary>
	///<param name="time">The time that has passed between frames</param>
	///<param name="readData">The particle data as provided by PhysX</param>
	virtual void UpdateParticleSystemCPU(const float time, const physx::PxParticleReadData* const readData) ;

	///<summary>Called per particle to check if the particle should be removed or not. Children can also
	///apply removal changes here (i.e. reset value to initials)</summary>
	///<param name="particleIndex">The index of the particle queried</param>
	///<param name="readData">The PhysX particle data</param>
	///<returns>True to remove, false if valid</returns>
	virtual inline bool QueryParticleRemoval(const unsigned int particleIndex, const physx::PxParticleReadData* const readData);

	///<summary>Allows children to add additional update information on a per-particle basis. Children don't have to implement this method</summary>
	///<param name="particleIndex">The index of the current particle</param>
	///<param name="readData">Any physx particle data that may be required</param>
	virtual inline void UpdateParticle(const unsigned int particleIndex, const physx::PxParticleReadData* const readData){}

	///<summary>Called after updating to allow children to unlock extra buffers, or other post-update actions</summary>
	virtual void PostUpdateCPU(){}

	///<summary>Updates the policy. Any GPU focused updating should go here. This is only called if physX 
	///is running on the GPU</summary>
	///<param name="time">The time that has passed between frames</param>
	///<param name="readData">The particle data as provided by PhysX in a GPU format</param>
	virtual void UpdatePolicyGPU(const float time, physx::PxParticleReadData* const readData){};

#pragma endregion

public:
	//Constructor used by serializer
	ParticleSystemBase(physx::PxParticleSystem* physxParticleSystem,
		std::shared_ptr<AbstractParticleEmitter> _emitter, size_t _maximumParticles, float _initialLifetime);

	ParticleSystemBase(std::shared_ptr<AbstractParticleEmitter> _emitter, size_t _maximumParticles, float _initialLifetime,
		ParticleSystemParams& paramsStruct = ParticleSystemParams());

	virtual ~ParticleSystemBase(void);

	///<summary>Gets the PhysX Particle System that this class wraps</summary>
	///<returns>The physx Particle system</returns>
	inline physx::PxParticleSystem* const GetPhysXParticleSystem()const{return pxParticleSystem;}

	///<summary>Initializes particle system and attaches to the specified scene</summary>
	///<param name="scene">The physX scene to add the particle system to</param>
	void Initialize(physx::PxScene* scene);

	///<summary>Updates the information of the particle system</summary>
	///<param name="time">The time step of the frame</param>
	void Update(float time);

	///<summary>Adds an affector to the particle system. The system claims ownership of the pointer</summary>
	///<param name="affectorToAdd">The affector to add</param>
	///<returns>True if added, false if an identical type exists (uses Affector.GetType())</returns>
	virtual bool AddAffector(std::shared_ptr<ParticleAffector> affectorToAdd);

	///<summary>Remove and deletes the affector of the specified type</summary>
	///<param name="typeToRemove">The type of affector to remove</param>
	///<returns>True if found and deleted, false if not found</returns>
	virtual bool RemoveAffector(ParticleAffectorType::ParticleAffectorType typeToRemove);

	///<summary>Removes the affector from the system and returns it, allowing it to be used elsewhere.
	///If the return value is not caught, there is a good chance for a memory leak, as particle systems
	///own affectors that are added to them</summary>
	///<param name="typeToRemove">The type of affector to remove</param>
	///<returns>The found affector, or NULL if none exist by that type in the list</returns>
	virtual std::shared_ptr<ParticleAffector> RemoveAndGetAffector(ParticleAffectorType::ParticleAffectorType typeToRemove);

	///<summary>Sets the kernel to the specified one</summary>
	///<param name="newKernel">The new kernel to use</param>
	virtual bool AssignAffectorKernel(ParticleKernel* newKernel);

	///<summary>Finds the best matching shader for the current collection of affectors</summary>
	///<returns>The name of the best matching shader, or DefaultParticleShader if not found</returns>
	virtual std::string FindBestShader();

	///<summary>Finds the best kernel suited to the current combination of GPU affectors</summary>
	///<returns>The best suited Kernel, or NULL if none exist</returns>
	virtual inline ParticleKernel* FindBestKernel(){return FluidAndParticleBase::FindBestKernel(affectors.gpuTypeCombination);}
};

