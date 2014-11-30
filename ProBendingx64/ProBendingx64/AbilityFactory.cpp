#include "AbilityFactory.h"
//#include "AbilityPrototypeDatabase.h"

AbilityFactory::AbilityFactory(void)
{
	AbilityPrototypeDatabase::LoadFromCode(ElementFlags::All);
}

AbilityFactory::AbilityFactory(ElementFlags::ElementFlags elementsToLoad)
{
	AbilityPrototypeDatabase::LoadFromCode(elementsToLoad);
}

AbilityFactory::~AbilityFactory(void)
{
}

AbilityDescriptor AbilityFactory::CreateAbility
	(AbilityPrototypeDatabase::AbilityID abilityID, Probender* const caster)const
{
	return AbilityDescriptor(NULL);
}