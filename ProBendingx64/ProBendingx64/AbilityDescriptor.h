#pragma once
#include <vector>
#include "AbilityEffect.h"
#include <memory>
#include "ProbenderFlags.h"

class Probender;

typedef std::shared_ptr<AbilityEffect> SharedAbilityEffect;

class AbilityDescriptor
{
public:

	enum TargetType{InvalidTarget, Self, CollidedTarget, Allies, Enemies, All };

protected:
	Probender* caster; //The caster of the ability
	Probender* collidedTarget; //The target found through collision, if this ability uses it
	ElementEnum::Element element; //The element of the ability
	TargetType targetType; //The type of target the ability applies upon
	float focusCost; //The cost in focus of the ability

	std::vector<SharedAbilityEffect> abilityEffects; //List of effects the ability uses

public:
	bool applyAbilityThisFrame; //True if the ability should be applied this frame. Used by the ability manager

	AbilityDescriptor(void);

	AbilityDescriptor(Probender* const _caster, const float focusCost = 0, 
		const ElementEnum::Element _element = ElementEnum::InvalidElement, const TargetType _targetType = InvalidTarget);

	virtual AbilityDescriptor* Clone();

	virtual ~AbilityDescriptor(void);
};

