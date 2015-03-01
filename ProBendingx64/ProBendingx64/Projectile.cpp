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

Projectile::Projectile(IScene* _owningScene,  ProjectileAttributes _baseAttributes, const std::string& objectName)
	:GameObject(_owningScene, objectName), baseAttributes(_baseAttributes), Attributes(_baseAttributes)
{

}

Projectile::~Projectile(void)
{
}

void Projectile::Start()
{
	GameObject::Start();
}

void Projectile::Update(float gameTime)
{
	GameObject::Update(gameTime);
}

void Projectile::LaunchProjectile(const physx::PxVec3& velocity)
{
	rigidBody->SetVelocity(velocity);
}

void Projectile::OnCollisionEnter(const CollisionReport& collision)
{
	if(collision.Collider->tag == TagsAndLayersManager::ContestantTag)
	{
		/*std::string message = "Collision Entered with: " + collision.Collider->GetName() + "\n";

		printf(message.c_str());*/
		Probender* bender = (Probender*)collision.Collider;

		float damage = RandomNumberGenerator::GetInstance()->GenerateRandom(Attributes.MinDamage, Attributes.MaxDamage);
		float knockback = RandomNumberGenerator::GetInstance()->
			GenerateRandom(Attributes.MinKnockback, Attributes.MaxKnockback);

		bender->ApplyProjectileCollision(damage, knockback);

		Disable();
	}
}

void Projectile::OnCollisionStay(const CollisionReport& collision)
{
	/*std::string message = "Collision Stayed with: " + collision.Collider->GetName() + "\n";

	printf(message.c_str());*/
}

void Projectile::OnCollisionLeave(const CollisionReport& collision)
{
	/*std::string message = "Collision Left with: " + collision.Collider->GetName() + "\n";

	printf(message.c_str());*/
}

void Projectile::OnTriggerEnter(GameObject* trigger, GameObject* other)
{
	std::string message = "Trigger Entered with: " + trigger->GetName() + "\n";

	if(trigger->tag == TagsAndLayersManager::WaterTag)
	{
		Disable();
	}

	printf(message.c_str());
}

void Projectile::OnTriggerLeave(GameObject* trigger, GameObject* other)
{
	std::string message = "Trigger Left with: " + trigger->GetName() + "\n";

	printf(message.c_str());
}

std::shared_ptr<Projectile> Projectile::Clone()const
{
	///NEED TO CLONE ATTACHED ABILITY
	std::shared_ptr<Projectile> clone = std::make_shared<Projectile>(owningScene, baseAttributes, name);
	clone->Attributes = Attributes;

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

	if(!enabled)
		clone->Disable();

	return clone;
}
