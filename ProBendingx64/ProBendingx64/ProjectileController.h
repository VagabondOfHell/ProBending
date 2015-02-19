#pragma once
#include "KinectBody.h"
#include "ProbenderData.h"

#include "foundation/PxVec3.h"
#include <memory>

class Projectile;
class Probender;

class ProjectileController
{
protected:
	Projectile* projectile;

public:

	physx::PxVec3 ProjectileOrigin;
	physx::PxVec3 ProbenderForward;
	physx::PxVec3 ProbenderRight;
	physx::PxVec3 ProbenderUp;

	ProjectileController()
		:projectile(nullptr)
	{
	}

	ProjectileController(Projectile* projectileToControl)
		: projectile(projectileToControl)
	{
	}

	virtual ~ProjectileController(){}

	void SetProjectileToControl(Projectile* newProjectile){projectile = newProjectile;}

	virtual void ControlProjectile(Probender* bender, const BodyDimensions& bodyDimensions,
		const CompleteData& currentData, const CompleteData& previousData) = 0;
};