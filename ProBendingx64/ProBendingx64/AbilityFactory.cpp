#include "AbilityFactory.h"

AbilityFactory::AbilityFactory(void)
{
}

AbilityFactory::~AbilityFactory(void)
{
}

AbilityPrototypeDatabase::SharedAbilityDescriptor AbilityFactory::CreateAbility
	(const ElementEnum::Element element, const AbilityIDs::AbilityID abilityID, Probender* const caster) const
{
	return AbilityPrototypeDatabase::GetAbilityClone(element, abilityID);
}
