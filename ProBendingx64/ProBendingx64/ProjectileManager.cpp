#include "ProjectileManager.h"
#include "Probender.h"

#include "RigidBodyComponent.h"
#include "ProjectileDatabase.h"
///When a projectile gets removed, check its ability data for the caster
///and check if the projectile is on one of the casters hands. If it is,
///set that hand to null before deleting the projectile
///Perhaps have an index pool as well so we can re-use indices

const unsigned short ProjectileManager::BASE_NUM_AIR_PROJECTILES = 3;
const unsigned short ProjectileManager::BASE_NUM_WATER_PROJECTILES = 4;
const unsigned short ProjectileManager::BASE_NUM_FIRE_PROJECTILES = 4;
const unsigned short ProjectileManager::BASE_NUM_EARTH_PROJECTILES = 4;

ProjectileManager::ProjectileManager(IScene* _owningScene)
	:owningScene(_owningScene)
{
}

ProjectileManager::~ProjectileManager(void)
{

}

SharedProjectile const ProjectileManager::CreateProjectile(const ElementEnum::Element element, const AbilityIDs::AbilityID abilityID)
{
	if(element == ElementEnum::InvalidElement)
		return NULL;

	//Search for the proper element collection
	auto elementResult = projectileMap.find(element);
	if(elementResult == projectileMap.end())
		return NULL;

	//Search for the ability id
	auto abilityResult = elementResult->second.find(abilityID);
	if(abilityResult == elementResult->second.end())
		return NULL;

	SharedProjectile retVal = abilityResult->second.GetSharedProjectile();

	//if we reach this point, ask the Pool if it has one available for us
	return retVal;
}

void ProjectileManager::RemoveProjectile(SharedProjectile projectile)
{
	projectilesToDisable.push_back(projectile.get());
}

void ProjectileManager::Update(const float gameTime)
{
	for (int i = 0; i < projectilesToDisable.size(); i++)
	{
		projectilesToDisable[i]->Disable();
	}

	projectilesToDisable.clear();

	//Loop through projectile pool collection
	for (auto start = projectileMap.begin(); start != projectileMap.end(); ++start)
	{
		//Loop through projectile pools
		for (auto projectiles = start->second.begin(); projectiles != start->second.end(); ++projectiles)
		{
			projectiles->second.Update(gameTime);
		}
	}
}

void ProjectileManager::CreatePool(ElementEnum::Element elementPool, unsigned short numberOfElement)
{
	if(numberOfElement == 0)
		return;

	ProjectileMap::iterator result;

	//if it exists, ignore
	if(FindExistingWithHint(elementPool, result))
		return;

	switch (elementPool)
	{
	case ElementEnum::Air:
		return;
		break;
	case ElementEnum::Earth:
		{
			ProjectileDatabase::ProjectileDictionary earthDictionary = ProjectileDatabase::GetEarthProjectiles(owningScene);

			AbilityPool abilityPool;
			
			for (auto start = earthDictionary.begin(); start != earthDictionary.end(); ++start)
			{
				ProjectilePool pool = ProjectilePool(BASE_NUM_EARTH_PROJECTILES * numberOfElement, start->second);
				abilityPool.insert(AbilityPool::value_type(start->first, pool));
			}

			projectileMap.insert(ProjectileMap::value_type(ElementEnum::Earth, abilityPool));
		}
		
		break;
	case ElementEnum::Fire:
		{
			ProjectileDatabase::ProjectileDictionary fireDictionary = ProjectileDatabase::GetFireProjectiles(owningScene);

			AbilityPool abilityPool;

			for (auto start = fireDictionary.begin(); start != fireDictionary.end(); ++start)
			{
				ProjectilePool pool = ProjectilePool(BASE_NUM_FIRE_PROJECTILES * numberOfElement, start->second);
				abilityPool.insert(AbilityPool::value_type(start->first, pool));
			}

			projectileMap.insert(ProjectileMap::value_type(ElementEnum::Fire, abilityPool));
		}
		break;
	case ElementEnum::Water:
		{
			ProjectileDatabase::ProjectileDictionary waterDictionary = 
				ProjectileDatabase::GetWaterProjectiles(owningScene);

			AbilityPool abilityPool;

			for (auto start = waterDictionary.begin(); start != waterDictionary.end(); ++start)
			{
				ProjectilePool pool = ProjectilePool(BASE_NUM_WATER_PROJECTILES * numberOfElement, start->second);
				abilityPool.insert(AbilityPool::value_type(start->first, pool));
			}

			projectileMap.insert(ProjectileMap::value_type(ElementEnum::Water, abilityPool));
		}
		break;
	default:
		return;
		break;
	}
}
