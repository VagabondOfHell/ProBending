#pragma once
#include "Projectile.h"
#include "ProbenderFlags.h"
#include <map>

class IScene;

class ProjectileManager
{
private:
	IScene* owningScene;//The scene that owns this manager

	unsigned int NEXT_PROJECTILE_ID;//The next projectile ID that will be used for the next created projectile
	typedef std::map<unsigned int, Projectile*> ProjectileMap;//Typedef for the projectile map
	
	ProjectileMap projectileMap;//The projectile map

public:
	ProjectileManager(IScene* _owningScene);
	~ProjectileManager(void);

	///<summary>Creates a new projectile based on the element and ability id given</summary>
	///<param name="element">The element that the ability belongs to</param>
	///<param name="abilityID">The ID of the ability, as defined in the ProbenderFlags.h header</param>
	///<returns>A pointer to the newly created projectile, or NULL if failed</returns>
	Projectile* const CreateProjectile(const ElementEnum::Element element,const AbilityIDs::AbilityID abilityID);
	
	///<summary>Destroys the specified projectile</summary>
	///<param name="projectile">The projectile to be destroyed</param>
	///<returns>True if found and successful, false if not</returns>
	bool DestroyProjectile(Projectile* projectile);

	void Update(const float gameTime);
};

