#include "ProjectileManager.h"
#include "ProjectileFactory.h"
#include "Probender.h"
#include "AbilityDescriptor.h"
#include "RigidBodyComponent.h"
///When a projectile gets removed, check its ability data for the caster
///and check if the projectile is on one of the casters hands. If it is,
///set that hand to null before deleting the projectile
///Perhaps have an index pool as well so we can re-use indices


ProjectileManager::ProjectileManager(IScene* _owningScene)
	:owningScene(_owningScene), NEXT_PROJECTILE_ID(0)
{
}

ProjectileManager::~ProjectileManager(void)
{
	//Destroy all projectiles
	ProjectileMap::iterator iter = projectileMap.begin();
	while (iter != projectileMap.end())
	{
		delete iter->second;
		++iter;
	}

}

Projectile* const ProjectileManager::CreateProjectile(const ElementEnum::Element element, const AbilityIDs::AbilityID abilityID)
{
	if(element == ElementEnum::InvalidElement)
		return NULL;

	Projectile* projectile = ProjectileFactory::CreateProjectile(owningScene, element, abilityID);

	if(projectile)
	{
		//If insertion is successful
		if(projectileMap.insert(ProjectileMap::value_type(NEXT_PROJECTILE_ID, projectile)).second)
		{
			projectile->projectileID = NEXT_PROJECTILE_ID;

			//increment id
			++NEXT_PROJECTILE_ID;

			projectile->Start();
			//return the projectile
			return projectile;
		}
	}
	
	return NULL;
}

bool ProjectileManager::DestroyProjectile(Projectile* projectile)
{
	ProjectileMap::iterator iter = projectileMap.find(projectile->projectileID);
	if(iter != projectileMap.end())
	{
		//Remove it from the caster
		projectile->attachedAbility->caster->RemoveProjectile(projectile);
		//Delete it (change this to return to object pool
		delete iter->second;

		projectileMap.erase(iter);

		return true;
	}

	return false;
}

void ProjectileManager::Update(const float gameTime)
{
	//Loop through and update all projectiles
	ProjectileMap::iterator iter = projectileMap.begin();
	while (iter != projectileMap.end())
	{
		iter->second->Update(gameTime);
		++iter;
	}
}
