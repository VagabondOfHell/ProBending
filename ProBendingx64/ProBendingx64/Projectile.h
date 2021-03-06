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

class SpecialEffect;

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
	ProjectileAttributes baseAttributes;

	bool ShowDestructionEffect();

public:
	ProjectileAttributes Attributes;

	SpecialEffect* CreationEffect, *DestructionEffect;

	ArenaData::RaycastFilter DestructionTriggers;

	unsigned short CasterContestantID;

	Projectile(IScene* owningScene, ProjectileAttributes baseAttributes, const std::string& objectName);
	virtual ~Projectile(void);

	inline float GetBaseMinDamage()const{return baseAttributes.MinDamage;}
	inline float GetBaseMaxDamage()const{return baseAttributes.MaxDamage;}

	inline float GetBaseMinKnockback()const{return baseAttributes.MinKnockback;}
	inline float GetBaseMaxKnockback()const{return baseAttributes.MaxKnockback;}

	inline void SetHalfExtents(Ogre::Vector3& newHalfExtents){baseAttributes.HalfExtents = Attributes.HalfExtents = newHalfExtents;}

	inline Ogre::Vector3 GetHalfExtents()const{return baseAttributes.HalfExtents;}
	inline bool GetUseGravity()const{return baseAttributes.UseGravity;}

	///<summary>At the moment this is used to differentiate between standard Game Objects and Projectiles and Probenders</summary>
	///<returns>True if serializable, false if not</returns>
	virtual inline bool IsSerializable()const{return false;}

	virtual void Start();

	virtual void Update(float gameTime);

	std::shared_ptr<Projectile> Clone()const;

	void LaunchProjectile(const physx::PxVec3& direction, float attackSpeedBonus, float attackBonus);

	virtual void OnCollisionEnter(const CollisionReport& collision);

	virtual void OnCollisionStay(const CollisionReport& collision);

	virtual void OnCollisionLeave(const CollisionReport& collision);

	virtual void OnTriggerEnter(GameObject* trigger, GameObject* other);

	virtual void OnTriggerLeave(GameObject* trigger, GameObject* other);

	virtual void Enable();

	bool ValidForReuse();
};

