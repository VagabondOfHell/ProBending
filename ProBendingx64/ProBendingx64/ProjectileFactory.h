#pragma once
#include "ProbenderFlags.h"
#include "Projectile.h"

#include <memory>

class IScene;

typedef std::shared_ptr<Projectile> SharedProjectile;

class ProjectileFactory
{
	friend class ProjectileManager;

private:
	static SharedProjectile CreateProjectile(IScene* const scene, const ElementEnum::Element element,const AbilityIDs::AbilityID abilityID);
};

