#pragma once
#include "ProbenderFlags.h"
#include "Projectile.h"

#include <memory>
#include <map>

class IScene;

class ProjectileDatabase
{
	typedef std::shared_ptr<Projectile> SharedProjectile;

	static const std::string ShapeString;

	static SharedProjectile CreateEarthJab(IScene* scene);
	static SharedProjectile CreateEarthCoin(IScene* scene);

	static SharedProjectile CreateFireJab(IScene* scene);
	static SharedProjectile CreateFireBlast(IScene* scene);

	static SharedProjectile CreateWaterJab(IScene* scene);
	static SharedProjectile CreateWaterRise(IScene* scene);

public:
	typedef std::map<AbilityIDs::AbilityID, SharedProjectile> ProjectileDictionary;

	///<summary>Gets a map of all the earth projectiles</summary>
	///<param name="scene">The scene that will create the scene nodes for the projectiles</param>
	///<returns>A map of Ability ID's as keys and Projectiles as Values</returns>
	static ProjectileDictionary GetEarthProjectiles(IScene* scene);

	static ProjectileDictionary GetFireProjectiles(IScene* scene);
	static ProjectileDictionary GetWaterProjectiles(IScene* scene);

};

