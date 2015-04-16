#include "Projectile.h"

#include "IScene.h"
#include "ParticleSystemBase.h"
#include "AbstractParticleEmitter.h"
#include "HelperFunctions.h"
#include "ParticleComponent.h"
#include "RigidBodyComponent.h"

#include "geometry/PxBoxGeometry.h"
#include "extensions/PxSimpleFactory.h"
#include "PxPhysics.h"
#include "PxRigidDynamic.h"
#include "PxScene.h"

#include "OgreSceneNode.h"
#include "Probender.h"
#include "RandomNumberGenerator.h"

#include "SpecialEffect.h"

Projectile::Projectile(IScene* _owningScene,  ProjectileAttributes _baseAttributes, const std::string& objectName)
	:GameObject(_owningScene, objectName), baseAttributes(_baseAttributes), Attributes(_baseAttributes),
	DestructionTriggers(0), DestructionEffect(NULL), CreationEffect(NULL)
{

}

Projectile::~Projectile(void)
{
	if(DestructionEffect)
		delete DestructionEffect;

	if(CreationEffect)
		delete CreationEffect;
}

void Projectile::Start()
{
	GameObject::Start();

	if(DestructionEffect)
		DestructionEffect->Start();
	if(CreationEffect)
		CreationEffect->Start();
}

void Projectile::Update(float gameTime)
{
	GameObject::Update(gameTime);

	if(CreationEffect)
		CreationEffect->Update(gameTime);

	if(DestructionEffect)
		DestructionEffect->Update(gameTime);
}

void Projectile::LaunchProjectile(const physx::PxVec3& direction, float attackSpeedBonus, float attackBonus)
{
	float speed = RandomNumberGenerator::GetInstance()->GenerateRandom(Attributes.MinSpeed, Attributes.MaxSpeed);
	speed *= attackSpeedBonus;
	
	Attributes.AttackBonus = attackBonus;

	rigidBody->SetVelocity(direction * speed);
}

void Projectile::OnCollisionEnter(const CollisionReport& collision)
{
	if(collision.Collider->tag == TagsAndLayersManager::ContestantTag)
	{
		Probender* bender = (Probender*)collision.Collider;

		if(bender->GetContestantID() != CasterContestantID)
		{
			float damage = RandomNumberGenerator::GetInstance()->GenerateRandom(Attributes.MinDamage, Attributes.MaxDamage);
			float knockback = RandomNumberGenerator::GetInstance()->
				GenerateRandom(Attributes.MinKnockback, Attributes.MaxKnockback);

			bender->ApplyProjectileCollision(damage, knockback);
			Disable();

			ShowDestructionEffect();
		}
	}

	if(DestructionTriggers > 0)
	{
		if(DestructionTriggers & ArenaData::PROJECTILE && collision.OtherFilterData & ArenaData::PROJECTILE
			&&((Projectile*)collision.Collider)->DestructionTriggers)
		{
			collision.Collider->Disable();

			((Projectile*)collision.Collider)->ShowDestructionEffect();
		}

		for (unsigned int i = 1; i < 9; i++)
		{
			unsigned int val = 1 << i;

			if((DestructionTriggers & val) && (collision.OtherFilterData & val))
			{
				ShowDestructionEffect();

				Disable();
			}
		}
	}
}

void Projectile::OnCollisionStay(const CollisionReport& collision)
{
}

void Projectile::OnCollisionLeave(const CollisionReport& collision)
{
}

void Projectile::OnTriggerEnter(GameObject* trigger, GameObject* other)
{
	if(trigger->tag == TagsAndLayersManager::WaterTag)
		Disable();
}

void Projectile::OnTriggerLeave(GameObject* trigger, GameObject* other)
{
}

std::shared_ptr<Projectile> Projectile::Clone()const
{
	///NEED TO CLONE ATTACHED ABILITY
	std::shared_ptr<Projectile> clone = std::make_shared<Projectile>(owningScene, baseAttributes, name);
	clone->Attributes = Attributes;

	clone->DestructionTriggers = DestructionTriggers;

	clone->SetWorldTransform(gameObjectNode->_getDerivedPosition(), gameObjectNode->_getDerivedOrientation(), gameObjectNode->_getDerivedScale());
	clone->SetInheritOrientation(GetInheritOrientation());
	clone->SetInheritScale(GetInheritScale());

	for (auto start = components.begin(); start != components.end(); ++start)
	{
		clone->AttachComponent(start->second->Clone(clone.get()));
	}

	for (auto start = children.begin(); start != children.end(); ++start)
	{
		clone->AddChild(start->get()->Clone());
	}

	if(DestructionEffect)
	{
		clone->DestructionEffect = DestructionEffect->Clone();
		clone->DestructionEffect->HideEffect();
	}

	if(CreationEffect)
	{
		clone->CreationEffect = CreationEffect->Clone();
		clone->CreationEffect->HideEffect();
	}

	if(!enabled)
		clone->Disable();

	return clone;
}

void Projectile::Enable()
{
	if(CreationEffect)
	{
		CreationEffect->EventPosition = GetWorldPosition();
		CreationEffect->ShowEffect();
	}

	GameObject::Enable();
}

bool Projectile::ShowDestructionEffect()
{
	if(DestructionEffect)
	{
		DestructionEffect->EventPosition = GetWorldPosition();
		DestructionEffect->ShowEffect();
	}

	return true;
}

bool Projectile::ValidForReuse()
{
	if(enabled)
		return false;

	if(CreationEffect)
	{
		if(CreationEffect->IsValid())
			if(CreationEffect->GetGameObject()->GetEnabled())
				return false;
	}

	if(DestructionEffect)
	{
		if(DestructionEffect->IsValid())
			if(DestructionEffect->GetGameObject()->GetEnabled())
				return false;
	}

	return true;
}
