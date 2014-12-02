#pragma once
#include "OgreSimpleRenderable.h"
#include "particles\PxParticleSystem.h"
#include "pxtask\PxCudaContextManager.h"
#include <vector>

class AbstractParticleEmitter;

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
	bool perParticleRestOffset;
	physx::PxReal restOffset;
	bool useGravity;
	physx::PxParticleBaseFlags baseFlags;
	physx::PxCudaContextManager* cudaContext;

	///<summary>Constructor used to set all values</summary>
	///<param name="_gridSize">The size of the grid between 0 and 1000</param>
	///<param name="_maxMotionDistance">The maximum distance the particle is allowed to travel in a frame</param>
	///<param name="_cudaContext">The Cuda Context to use for GPU acceleration. Set NULL to use CPU</param>
	///<param name="_externalAcceleration">The external acceleration to apply to all particles each frame</param>
	///<param name="_particleMass">The mass of each particle</param>
	///<param name="_useGravity">True to have particles affected by gravity, false if not</param>
	///<param name="_baseFlags">The flags to configure the system</param>
	///<param name="_perParticleRestOffset">True to enable, false if not</param>
	///<param name="_restOffset">The rest offset of the particles, if enabled </param>
	///<param name="_staticFriction">The friction used for collisions. Must be non-negative</param>
	///<param name="_dynamicFriction">The dynamic friction to use. Must be non-negative</param>
	///<param name="_restitution">The bounciness of the collision, between 0.0f and 1.0f</param>
	///<param name="_contactOffset">The distance that contacts are generated between particle and geometry</param>
	///<param name="_damping">Damping for particles. Must be non-negative</param>
	ParticleSystemParams(physx::PxReal _gridSize = 1.0f, physx::PxReal _maxMotionDistance = 2.0f,
		physx::PxCudaContextManager* _cudaContext = NULL, physx::PxVec3 _externalAcceleration = physx::PxVec3(0.0f), 
		physx::PxReal _particleMass = 1.0f, bool _useGravity = true, 
		physx::PxParticleBaseFlags _baseFlags = physx::PxParticleBaseFlag::eENABLED, bool _perParticleRestOffset = false,		
		physx::PxReal _restOffset = 1.0f, physx::PxReal _staticFriction = 0.0f, physx::PxReal _dynamicFriction = 0.0f,
		physx::PxReal _restitution = 1.0f, physx::PxReal _contactOffset = 0.0f, physx::PxReal _damping = 0.0f)
		
		:gridSize(_gridSize), maxMotionDistance(_maxMotionDistance), cudaContext(_cudaContext),
		externalAcceleration(_externalAcceleration), 
		useGravity(_useGravity), particleMass(_particleMass), baseFlags(_baseFlags), perParticleRestOffset(_perParticleRestOffset), 
		restOffset(_restOffset), staticFriction(_staticFriction), dynamicFriction(_dynamicFriction), 
		restitution(_restitution), contactOffset(_contactOffset), damping(_damping) 
	{
	}
};


class ParticleSystemBase: public Ogre::SimpleRenderable
{
protected:
	AbstractParticleEmitter* emitter;

	std::vector<physx::PxU32> availableIndices; //The available indices within the particle system

	physx::PxParticleSystem* pxParticleSystem; ///The physX particle system
	physx::PxCudaContextManager* cudaContextManager; ///The physX cuda context manager

	size_t maximumParticles; ///The maximum number of particles allowed in the particle system
	bool onGPU; ///Whether or not the particle system is used on the GPU

	physx::PxParticleReadDataFlags readableData; ///The readable data of the particle system
		
	///<summary>Sets the read data flags as specified by the newFlags argument</summary>
	///<param name="newFlags">The new read data flags to use</param>
	void SetParticleReadFlags(physx::PxParticleReadDataFlags newFlags);

#pragma region Virtual Methods for Inherited System Customization

	///<summary>Provides children the opportunity to initialize their custom data</summary>
	virtual void InitializeParticleSystemData() = 0;
	
	///<summary>Provides children the opportunity to initialize their custom data. It is only called
	///if the system is allowed to perform on the GPU</summary>
	///<param name="contextManager">The cuda context manager to use</param>
	virtual void InitializeGPUData() = 0;

	///<summary>Callback for when particles are created, so children can set custom data</summary>
	///<param name="createdCount>The amount of particles that were created</param>
	///<param name="emittedIndices>The indices that were used this creation frame</param>
	virtual void ParticlesCreated(const unsigned int createdCount, physx::PxStrideIterator<const physx::PxU32> emittedIndices) = 0;

	///<summary>Updates the policy. Any CPU focused updating should go here. Returns a vector of indices that represent
	///the indices that should be removed</summary>
	///<param name="time">The time that has passed between frames</param>
	///<param name="readData">The particle data as provided by PhysX</param>
	virtual std::vector<const physx::PxU32> UpdatePolicy(const float time, const physx::PxParticleReadData* const readData) = 0;

	///<summary>Updates the policy. Any GPU focused updating should go here. This is only called if physX 
	///is running on the GPU</summary>
	///<param name="time">The time that has passed between frames</param>
	///<param name="readData">The particle data as provided by PhysX in a GPU format</param>
	virtual void UpdatePolicyGPU(const float time, physx::PxParticleReadData* const readData) = 0;

#pragma endregion

public:
	bool ownEmitter;

	ParticleSystemBase(AbstractParticleEmitter* _emitter, size_t _maximumParticles, 
		ParticleSystemParams& paramsStruct = ParticleSystemParams(), bool _ownEmitter = true);

	virtual ~ParticleSystemBase(void);

	///<summary>Gets the maximum number of particles allowed to be managed by the particle system at a time</summary>
	///<returns>The maximum number of particles, as declared when constructing the particle system</returns>
	inline size_t GetMaximumParticlesAllowed()const{	return maximumParticles;}

	///<summary>Gets the emitter used by the particle system</summary>
	///<returns>Pointer to the abstract particle emitter</returns>
	inline AbstractParticleEmitter* const GetEmitter()const{return emitter;}

	///<summary>Gets the PhysX Particle System that this class wraps</summary>
	///<returns>The physx Particle system</returns>
	inline physx::PxParticleSystem* const GetPhysXParticleSystem()const{return pxParticleSystem;}

	///<summary>Initializes particle system and attaches to the specified scene</summary>
	///<param name="scene">The physX scene to add the particle system to</param>
	void Initialize(physx::PxScene* scene);

	///<summary>Updates the information of the particle system</summary>
	///<param name="time">The time step of the frame</param>
	void Update(float time);

	virtual Ogre::Real getBoundingRadius(void) const
	{
		return 0;
	}

	virtual Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam)const;
};

