#pragma once
#include "GameObject.h"
#include <memory>

class AbilityDescriptor;
class ParticleGameObject;

namespace Ogre
{
	class SceneNode;
	class Entity;
};

namespace physx
{
	class PxRigidDynamic;
};

typedef std::shared_ptr<AbilityDescriptor> SharedAbilityDescriptor;

class Projectile : public GameObject
{
	friend class ProjectileManager;

private:
	SharedAbilityDescriptor attachedAbility;
	unsigned int projectileID;

public:
	
	Projectile(IScene* owningScene, SharedAbilityDescriptor _attachedAbility);
	virtual ~Projectile(void);

	virtual void Start();

	virtual void Update(float gameTime);

	///<summary>Attaches an ability if there isn't one already</summary>
	///<param name="abilityToAttach">The new ability to be attached to the projectile</param>
	void AttachAbility(SharedAbilityDescriptor abilityToAttach);
};

