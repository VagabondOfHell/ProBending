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

Projectile::Projectile(IScene* _owningScene, const std::string& objectName, SharedAbilityDescriptor _attachedAbility)
	:GameObject(_owningScene, objectName), attachedAbility(_attachedAbility)
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

void Projectile::AttachAbility(SharedAbilityDescriptor abilityToAttach)
{
	if(!attachedAbility.get())
		attachedAbility = abilityToAttach;
}

void Projectile::LaunchProjectile(const physx::PxVec3& velocity)
{
	rigidBody->SetVelocity(velocity);
}

void Projectile::OnCollisionEnter(const CollisionReport& collision)
{
	std::string message = "Collision Entered with: " + collision.Collider->GetName() + "\n";

	printf(message.c_str());
}

void Projectile::OnCollisionStay(const CollisionReport& collision)
{
	std::string message = "Collision Stayed with: " + collision.Collider->GetName() + "\n";

	printf(message.c_str());
}

void Projectile::OnCollisionLeave(const CollisionReport& collision)
{
	std::string message = "Collision Left with: " + collision.Collider->GetName() + "\n";

	printf(message.c_str());
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
	std::shared_ptr<Projectile> clone = std::make_shared<Projectile>(owningScene, name, attachedAbility);

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
