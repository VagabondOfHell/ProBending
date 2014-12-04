#pragma once
#include "ProbenderFlags.h"

class Projectile;
class IScene;

class ProjectileFactory
{
	friend class ProjectileManager;

private:
	static Projectile* CreateProjectile(IScene* const scene, const ElementEnum::Element element,const AbilityIDs::AbilityID abilityID);
};

