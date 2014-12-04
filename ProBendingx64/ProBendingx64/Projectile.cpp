#include "Projectile.h"


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
	{
		attachedAbility = abilityToAttach;
	}
}
