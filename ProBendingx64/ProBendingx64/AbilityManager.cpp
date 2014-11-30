#include "AbilityManager.h"


AbilityManager::AbilityManager(void)
{
}


AbilityManager::~AbilityManager(void)
{
}
	
SharedAbilityDescriptor AbilityManager::CreateAbility(const std::string abilityName, Probender* const caster)
{
	return SharedAbilityDescriptor(new AbilityDescriptor(NULL));
}

void AbilityManager::RemoveAbility(SharedAbilityDescriptor abilityToRemove)
{
	
}


void AbilityManager::Update(const float gameTime)
{
	
}