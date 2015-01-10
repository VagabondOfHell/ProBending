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
