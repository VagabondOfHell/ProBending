#pragma once
#include "AbilityEffect.h"
class DecoratorAbilityEffect :
	public AbilityEffect
{
public:
	DecoratorAbilityEffect(void);
	virtual ~DecoratorAbilityEffect(void);

	///<summary>Clones the current DecoratorAbilityEffect into its own object</summary>
	///<returns> A deep copy of the DecoratorAbilityEffect</returns>
	virtual DecoratorAbilityEffect* Clone()
	{
		return new DecoratorAbilityEffect(*this);
	}

	///<summary>Gets the type of effect as a string that matches the class name</summary>
	///<returns>A string matching the name of the class</returns>
	virtual std::string GetEffectType()
	{
		return "DecoratorAbilityEffect";
	}

	///<summary>Updates and applies the effect to the specified target</summary>
	///<param name="gameTime">The time that has passed between frames</param>
	///<param name="target">The Probender to apply the effect to</param>
	virtual void Update(const float gameTime, Probender* const target);
};

