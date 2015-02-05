#include "Projectile.h"
#include "ParticleSystemBase.h"
#include "OgreSceneNode.h"
#include "AbstractParticleEmitter.h"
#include "HelperFunctions.h"
#include "ParticleComponent.h"
#include "geometry/PxBoxGeometry.h"
#include "extensions/PxSimpleFactory.h"
#include "PxPhysics.h"
#include "PxRigidDynamic.h"
#include "PxScene.h"
#include "IScene.h"

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

	printf(message.c_str());
}

void Projectile::OnTriggerLeave(GameObject* trigger, GameObject* other)
{
	std::string message = "Trigger Left with: " + trigger->GetName() + "\n";

	printf(message.c_str());
}
