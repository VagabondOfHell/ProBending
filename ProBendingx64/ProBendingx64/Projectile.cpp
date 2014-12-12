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

Projectile::Projectile(IScene* _owningScene, SharedAbilityDescriptor _attachedAbility)
	:GameObject(_owningScene), attachedAbility(_attachedAbility)
{
}


Projectile::~Projectile(void)
{
}

void Projectile::Start()
{
	GameObject::Start();
	
	physx::PxReal density = 1.0f;
	physx::PxTransform transform(physx::PxVec3(0.0f), physx::PxQuat::createIdentity());
	physx::PxBoxGeometry geometry;

	ConstructBoxFromEntity(geometry);

	geometry.halfExtents = physx::PxVec3(1, 1, 1);
	rigidBody = physx::PxCreateDynamic(PxGetPhysics(), transform, geometry, *PxGetPhysics().createMaterial(0.5,0.5,0.5), density);
	rigidBody->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);

	owningScene->GetPhysXScene()->addActor(*rigidBody);
}

void Projectile::Update(float gameTime)
{
	/*auto itr = components.find(Component::ComponentType::PARTICLE_COMPONENT);

	if(itr != components.end())
	{
		ParticleComponent* particleComponent = (ParticleComponent*)itr->second;
		ParticleSystemBase* fs = particleComponent->particleSystem;
		fs->GetEmitter()->position = HelperFunctions::OgreToPhysXVec3(gameObjectNode->getPosition());
	}*/
	GameObject::Update(gameTime);
}

void Projectile::AttachAbility(SharedAbilityDescriptor abilityToAttach)
{
	if(!attachedAbility.get())
	{
		attachedAbility = abilityToAttach;
	}
}
