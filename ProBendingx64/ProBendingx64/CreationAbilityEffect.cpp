#include "CreationAbilityEffect.h"


CreationAbilityEffect::CreationAbilityEffect(void)
{
}

CreationAbilityEffect::CreationAbilityEffect(GameObject* const _objectTocreate, const float _duration, const physx::PxVec3 creationLocation)
	:objectToCreate(_objectTocreate), AbilityEffect(_duration), positionOfCreation(creationLocation)
{
	
}

CreationAbilityEffect::~CreationAbilityEffect(void)
{
}

void CreationAbilityEffect::Update(const float gameTime, Probender* const target)
{
	
}