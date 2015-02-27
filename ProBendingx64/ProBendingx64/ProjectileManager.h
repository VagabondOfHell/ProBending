#pragma once
#include "ProjectilePool.h"
#include "ProbenderFlags.h"
#include <map>

class IScene;

typedef std::shared_ptr<Projectile> SharedProjectile;

class ProjectileManager
{
private:
	static const unsigned short BASE_NUM_EARTH_PROJECTILES;
	static const unsigned short BASE_NUM_FIRE_PROJECTILES;
	static const unsigned short BASE_NUM_WATER_PROJECTILES;
	static const unsigned short BASE_NUM_AIR_PROJECTILES;

	typedef std::map<AbilityIDs::AbilityID, ProjectilePool> AbilityPool;
	typedef std::map<ElementEnum::Element, AbilityPool> ProjectileMap;

	ProjectileMap projectileMap;

	inline bool FindExistingWithHint(const ElementEnum::Element e, ProjectileMap::iterator& outVal)
	{
		outVal = projectileMap.lower_bound(e);
		//returns true if existing
		return (outVal != projectileMap.end() && !(projectileMap.key_comp()(e, outVal->first)));
	}

	IScene* owningScene;//The scene that owns this manager

public:
	ProjectileManager(IScene* _owningScene);
	~ProjectileManager(void);

	void CreatePool(ElementEnum::Element elementPool, unsigned short numberOfElement);

	///<summary>Creates a new projectile based on the element and ability id given</summary>
	///<param name="element">The element that the ability belongs to</param>
	///<param name="abilityID">The ID of the ability, as defined in the ProbenderFlags.h header</param>
	///<returns>A pointer to the newly created projectile, or NULL if failed</returns>
	SharedProjectile const CreateProjectile(const ElementEnum::Element element,const AbilityIDs::AbilityID abilityID);
	
	inline SharedProjectile const CreateProjectile(const ProjectileIdentifier& projID)
	{
		return CreateProjectile(projID.Element, projID.AbilityID);
	}

	///<summary>Removes the projectile from the manager</summary>
	///<param name="projectile">The projectile to remove</param>
	void RemoveProjectile(SharedProjectile projectile);

	void Update(const float gameTime);
};

