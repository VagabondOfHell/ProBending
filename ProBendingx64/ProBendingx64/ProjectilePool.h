#pragma once
#include "Projectile.h"
#include <vector>
#include <memory>
#include "IScene.h"
//Intended to have a projectile pool for each ability

class ProjectilePool
{
private:
	std::vector<std::shared_ptr<Projectile>> pool;

public:
	ProjectilePool(unsigned int poolSize, const std::shared_ptr<Projectile> projectile)
	{
		if(poolSize > 0)
		{
			projectile->Disable();
			pool.push_back(projectile);
			projectile->Start();

			for (unsigned int i = 1; i < poolSize; i++)
			{
				pool.push_back(projectile->Clone());
				pool[i]->Start();
			}
		}
	}

	~ProjectilePool(void)
	{

	}

	inline Projectile* GetWeakProjectile()
	{
		std::shared_ptr<Projectile> proj = GetSharedProjectile();

		if(proj)
			return proj.get();
		else
			return NULL;
	}

	inline std::shared_ptr<Projectile> GetSharedProjectile()
	{
		auto result = std::find_if(pool.begin(), pool.end(), 
			[](std::shared_ptr<Projectile> proj){return proj->ValidForReuse();});

		if(result != pool.end())
			return *result;
		else
			return NULL;
	}

	inline void Update(float gameTime)
	{
		for (int i = 0; i < pool.size(); i++)
		{
			pool[i]->Update(gameTime);
		}
	}
};

