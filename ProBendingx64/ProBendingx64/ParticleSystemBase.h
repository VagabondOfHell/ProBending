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

	///<summary>Updates the policy. Any CPU focused updating should go here. Returns a vector of indices that represent
	///the indices that should be removed</summary>
	///<param name="time">The time that has passed between frames</param>
	///<param name="readData">The particle data as provided by PhysX</param>
	virtual void UpdateParticleSystemCPU(const float time, const physx::PxParticleReadData* const readData) ;

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
	
	AffectorData* const GetAffectorData(){return &affectors;}

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

	///<summary>Finds the best kernel suited to the current combination of GPU affectors</summary>
	///<returns>The best suited Kernel, or NULL if none exist</returns>
	virtual inline ParticleKernel* FindBestKernel(){return FluidAndParticleBase::FindBestKernel(affectors.gpuTypeCombination);}
};

