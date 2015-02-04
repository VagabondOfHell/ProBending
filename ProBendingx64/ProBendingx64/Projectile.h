#pragma once
#include "GameObject.h"
#include "AbilityDescriptor.h"

#include <memory>

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

	///<summary>At the moment this is used to differentiate between standard Game Objects and Projectiles and Probenders</summary>
	///<returns>True if serializable, false if not</returns>
	virtual inline bool IsSerializable()const{return false;}

	virtual void Start();

	virtual void Update(float gameTime);

	///<summary>Attaches an ability if there isn't one already</summary>
	///<param name="abilityToAttach">The new ability to be attached to the projectile</param>
	void AttachAbility(SharedAbilityDescriptor abilityToAttach);
};

