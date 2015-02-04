#include "FluidAndParticleBase.h"

using namespace physx;

void FluidAndParticleBase::SetParticleReadFlags(physx::PxParticleBase* pxParticleSystem, physx::PxParticleReadDataFlags newFlags)
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

void FluidAndParticleBase::SetParticleBaseFlags(physx::PxParticleBase* pxParticleSystem, physx::PxParticleBaseFlags newFlags)
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

void FluidAndParticleBase::SetSystemData(physx::PxParticleBase* pxParticleSystem, const ParticleSystemParams& paramsStruct)
{
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
}
