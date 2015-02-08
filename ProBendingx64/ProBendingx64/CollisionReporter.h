#pragma once
#include "PxSimulationEventCallback.h"

class CollisionReporter: public physx::PxSimulationEventCallback
{
public:
	CollisionReporter(void);
	~CollisionReporter(void);

	virtual void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count);

	virtual void onWake(physx::PxActor** actors, physx::PxU32 count);

	virtual void onSleep(physx::PxActor** actors, physx::PxU32 count);

	virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);

	virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count);

};

