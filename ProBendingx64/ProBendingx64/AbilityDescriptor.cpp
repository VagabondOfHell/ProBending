#include "AbilityDescriptor.h"


AbilityDescriptor::AbilityDescriptor(void)
{
}

AbilityDescriptor::AbilityDescriptor(Probender* const _caster, const float _focusCost, 
		const ElementEnum::Element _element, const TargetType _targetType)
		:caster(_caster), focusCost(_focusCost), element(_element), targetType(_targetType), collidedTarget(NULL)
{
	
}

AbilityDescriptor::~AbilityDescriptor(void)
{
}

AbilityDescriptor* AbilityDescriptor::Clone()
{
	////DO WE NEED TO LOOP THROUGH AND CALL CLONE ON EVERY EFFECT?!!/////
	AbilityDescriptor* clone = new AbilityDescriptor(*this);

	return clone;
}
