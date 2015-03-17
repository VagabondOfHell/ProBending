#pragma once
#include "particles/PxParticleFluid.h"//including fluid includes particle base

namespace physx
{
	class PxCudaContextManager;
};

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
	bool useGravity;
	physx::PxParticleBaseFlags baseFlags;
	physx::PxCudaContextManager* cudaContext;
	physx::PxFilterData filterData;
	physx::PxParticleReadDataFlags readData;

	physx::PxReal viscosity;
	physx::PxReal stiffness;
	physx::PxReal restParticleDistance;

	///<summary>Constructor used to set all values</summary>
	///<param name="_gridSize">The size of the grid between 0 and 1000</param>
	///<param name="_maxMotionDistance">The maximum distance the particle is allowed to travel in a frame</param>
	///<param name="_cudaContext">The Cuda Context to use for GPU acceleration. Set NULL to use CPU</param>
	///<param name="_externalAcceleration">The external acceleration to apply to all particles each frame</param>
	///<param name="_particleMass">The mass of each particle</param>
	///<param name="_useGravity">True to have particles affected by gravity, false if not</param>
	///<param name="_baseFlags">The flags to configure the system</param>
	///<param name="_restOffset">The rest offset of the particles, if enabled </param>
	///<param name="_staticFriction">The friction used for collisions. Must be non-negative</param>
	///<param name="_dynamicFriction">The dynamic friction to use. Must be non-negative</param>
	///<param name="_restitution">The bounciness of the collision, between 0.0f and 1.0f</param>
	///<param name="_contactOffset">The distance that contacts are generated between particle and geometry</param>
	///<param name="_damping">Damping for particles. Must be non-negative</param>
	///<param name="_filterData">Any filter data to be used during the simulation</param>
	ParticleSystemParams(physx::PxReal _gridSize = 1.0f, physx::PxReal _maxMotionDistance = 2.0f,
		physx::PxCudaContextManager* _cudaContext = NULL, physx::PxVec3 _externalAcceleration = physx::PxVec3(0.0f), 
		physx::PxReal _particleMass = 1.0f, bool _useGravity = true, physx::PxParticleBaseFlags _baseFlags = 
		physx::PxParticleBaseFlag::eENABLED, physx::PxReal _restOffset = 1.0f, physx::PxReal _staticFriction = 0.0f, 
		physx::PxReal _dynamicFriction = 0.0f, physx::PxReal _restitution = 1.0f, physx::PxReal _contactOffset = 0.0f, 
		physx::PxReal _damping = 0.0f, physx::PxFilterData _filterData = physx::PxFilterData(),
		physx::PxParticleReadDataFlags _readData = physx::PxParticleReadDataFlag::ePOSITION_BUFFER
		| physx::PxParticleReadDataFlag::eFLAGS_BUFFER)

		:gridSize(_gridSize), maxMotionDistance(_maxMotionDistance), cudaContext(_cudaContext),
		externalAcceleration(_externalAcceleration), 
		useGravity(_useGravity), particleMass(_particleMass), baseFlags(_baseFlags), restOffset(_restOffset), 
		staticFriction(_staticFriction), dynamicFriction(_dynamicFriction), 
		restitution(_restitution), contactOffset(_contactOffset), damping(_damping), filterData(_filterData),
		readData(_readData)
	{
	}

	void SetFluidParameters(physx::PxReal _viscosity, physx::PxReal _stiffness, physx::PxReal _restParticleDistance)
	{
		viscosity = _viscosity;
		stiffness = _stiffness;
		restParticleDistance = _restParticleDistance;
	}
};
