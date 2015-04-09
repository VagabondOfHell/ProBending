#pragma once
#include "ProjectileController.h"

#include "PxPhysics.h"

class HandMoveController: public ProjectileController
{
private:
	void UpdateHandAxis();

	void UpdateProjectilePosition(Probender* probender, float armLength, 
		const CameraSpacePoint& shoulderPos, const CameraSpacePoint& handPos);

protected:
	enum HandAxis{HA_INVALID = 0, HA_X = 1, HA_Y = 2, HA_Z = 4};
	short handAxis;

	physx::PxVec3 minOffset;
	physx::PxVec3 maxOffset;

public:
	enum ControllingHand{CH_INVALID, CH_LEFT, CH_RIGHT, CH_BOTH};

	ControllingHand HandUsed;

	//How close the hands must be to each other when using both hands. 
	//If they exceed this distance, they are ignored this frame. This distance should be the distance without Square Root
	float DualHandThreshold;

	HandMoveController(Projectile* projectileToControl, ControllingHand handToUse, 
		const physx::PxVec3& minOffset, const physx::PxVec3& maxOffset, 
		GestureEnums::TransitionRules transitionFromPrevious = GestureEnums::TRANRULE_NONE);

	virtual ~HandMoveController(void);

	void SetMinOffset(const physx::PxVec3& newMinOffset){minOffset = newMinOffset; UpdateHandAxis();}
	void SetMaxOffset(const physx::PxVec3& newMaxOffset){maxOffset = newMaxOffset; UpdateHandAxis();}

	virtual void ControlProjectile(Probender* bender, const CompleteData& currentData, 
		const CompleteData& previousData);

	virtual void ReceivePreviousResults(GestureEnums::BodySide prevResults);

};

