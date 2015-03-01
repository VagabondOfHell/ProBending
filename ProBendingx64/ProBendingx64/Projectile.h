#pragma once
#include "ProbenderFlags.h"
#include "ProjectileAttributes.h"

#define NOMINMAX
#include "GameObject.h"

namespace Ogre
{
	class SceneNode;
	class Entity;
};

namespace physx
{
	class PxRigidDynamic;
};

struct ProjectileIdentifier
{
	ElementEnum::Element Element;
	AbilityIDs::AbilityID AbilityID;

	ProjectileIdentifier()
		:Element(ElementEnum::InvalidElement), AbilityID(0)
	{

	}
};

class Projectile : public GameObject
{
	friend class ProjectileManager;
	const ProjectileAttributes baseAttributes;

public:
	ProjectileAttributes Attributes;

	unsigned short CasterContestantID;

	Projectile(IScene* owningScene, ProjectileAttributes baseAttributes, const std::string& objectName);
	virtual ~Projectile(void);

	inline float GetBaseMinDamage()const{return baseAttributes.MinDamage;}
	inline float GetBaseMaxDamage()const{return baseAttributes.MaxDamage;}

	inline float GetBaseMinKnockback()const{return baseAttributes.MinKnockback;}
	inline float GetBaseMaxKnockback()const{return baseAttributes.MaxKnockback;}

	///<summary>At the moment this is used to differentiate between standard Game Objects and Projectiles and Probenders</summary>
	///<returns>True if serializable, false if not</returns>
	virtual inline bool IsSerializable()const{return false;}

	virtual void Start();

	virtual void Update(float gameTime);

	std::shared_ptr<Projectile> Clone()const;

	void LaunchProjectile(const physx::PxVec3& direction, const float speed){LaunchProjectile(direction * speed);}

	void LaunchProjectile(const physx::PxVec3& velocity);

	virtual void OnCollisionEnter(const CollisionReport& collision);

	virtual void OnCollisionStay(const CollisionReport& collision);

	virtual void OnCollisionLeave(const CollisionReport& collision);

	virtual void OnTriggerEnter(GameObject* trigger, GameObject* other);

	virtual void OnTriggerLeave(GameObject* trigger, GameObject* other);

};

