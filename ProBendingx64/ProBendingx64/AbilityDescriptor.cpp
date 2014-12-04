#include "AbilityDescriptor.h"
#include "AbilityEffect.h"

AbilityDescriptor::AbilityDescriptor(Probender* const _caster, const AbilityType _abilityType, const float _focusCost, 
		const ElementEnum::Element _element, const TargetType _targetType)
		:caster(_caster), abilityType(_abilityType), focusCost(_focusCost),
			element(_element), targetType(_targetType), collidedTarget(NULL)
{
	//By default we will apply a non-offensive as soon as possible, whereas an offensive one should
	//only activate upon collision
	if(abilityType == Offensive)
		applyAbilityThisFrame = false;
	else
		applyAbilityThisFrame = true;
}

AbilityDescriptor::~AbilityDescriptor(void)
{
}

AbilityDescriptor* AbilityDescriptor::Clone()
{
	AbilityDescriptor* clone = new AbilityDescriptor(*this);

	//Clone each effect so that the shared pointer points to the new effects
	for (int i = 0; i < clone->abilityEffects.size(); i++)
	{
		clone->abilityEffects[i] = SharedAbilityEffect(clone->abilityEffects[i]->Clone());
	}

	return clone;
}
