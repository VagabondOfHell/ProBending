#pragma once
#include "KinectBody.h"
#include "ProbenderData.h"

#include "foundation/PxVec3.h"
#include <memory>
#include "GestureEnums.h"

class Projectile;
class Probender;

class ProjectileController
{
protected:
	Projectile* projectile;

public:

	GestureEnums::TransitionRules TransitionFromPrevious;

	physx::PxVec3 ProjectileOrigin;
	physx::PxVec3 ProbenderForward;
	physx::PxVec3 ProbenderRight;
	physx::PxVec3 ProbenderUp;

	ProjectileController(GestureEnums::TransitionRules transitionFromPrevious = GestureEnums::TRANRULE_NONE)
		:projectile(nullptr), TransitionFromPrevious(transitionFromPrevious)
	{
	}

	ProjectileController(Projectile* projectileToControl, GestureEnums::TransitionRules transitionFromPrevious = GestureEnums::TRANRULE_NONE)
		: projectile(projectileToControl), TransitionFromPrevious(transitionFromPrevious)
	{
	}

	virtual ~ProjectileController(){}

	void SetProjectileToControl(Projectile* newProjectile){projectile = newProjectile;}

	virtual void ControlProjectile(Probender* bender, const BodyDimensions& bodyDimensions,
		const CompleteData& currentData, const CompleteData& previousData) = 0;

	virtual void ReceivePreviousResults(GestureEnums::BodySide prevResults){}
};