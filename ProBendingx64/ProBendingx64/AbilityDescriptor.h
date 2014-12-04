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
	enum AbilityType{InvalidAbility, Offensive, Defensive, Creation};

	AbilityType abilityType;//The type of ability
	Probender* caster; //The caster of the ability
	Probender* collidedTarget; //The target found through collision, if this ability uses it
	ElementEnum::Element element; //The element of the ability
	TargetType targetType; //The type of target the ability applies upon
	float focusCost; //The cost in focus of the ability

	std::vector<SharedAbilityEffect> abilityEffects; //List of effects the ability uses

	bool applyAbilityThisFrame; //True if the ability should be applied this frame. Used by the ability manager

	AbilityDescriptor(Probender* const _caster = NULL, const AbilityType _abilityType = AbilityType::InvalidAbility, const float focusCost = 0, 
		const ElementEnum::Element _element = ElementEnum::InvalidElement, const TargetType _targetType = InvalidTarget);

	///<summary>Creates a clone of the Ability, performing deep copies on some data</summary>
	///<returns>A stand-alone copy of the ability descriptor</returns>
	virtual AbilityDescriptor* Clone();

	virtual ~AbilityDescriptor(void);
};

