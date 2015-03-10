#include "CollisionFilterShaders.h"
#include "ArenaData.h"

#include <stdio.h>

using namespace physx;

physx::PxFilterFlags CollisionFilterShaders::GameSceneFilterShader
	(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, 
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, 
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	// let triggers through
	if(physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
	{
		//Check if trigger collision is between contestant and zone. If it isn't ignore the Zone Trigger
		if((filterData0.word0 == ArenaData::ZONE_TRIGGER || filterData1.word0 == ArenaData::ZONE_TRIGGER) 
			&& (filterData0.word0 != ArenaData::CONTESTANT && filterData1.word0 != ArenaData::CONTESTANT))
			return physx::PxFilterFlag::eKILL;

		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
		return physx::PxFilterFlag::eDEFAULT;
	}

	// generate contacts for all that were not filtered above
	pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
	pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;
	pairFlags |= PxPairFlag::eCCD_LINEAR;
	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	//if((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;

	pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
	pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS;

	return physx::PxFilterFlag::eDEFAULT;
}
