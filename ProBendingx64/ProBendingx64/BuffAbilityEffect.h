#pragma once
#include "AbilityEffect.h"

class BuffAbilityEffect :
	public AbilityEffect
{
private:
	bool previouslyApplied; //If false, we need to apply the effect. If true, it's been applied already and we just wait for it to end
	bool applyPerSecond; //True to apply the value every second, false if it should be applied only once

public:
	//Move this to the attributes structure later
	enum ModifiableAttribute{CounterAttack, Health, Invalid};

	ModifiableAttribute statModified;// The stat to be modified
	float percentageModified;// The percentage to apply to the attribute

	
	BuffAbilityEffect(const float _duration = 1.0f, const ModifiableAttribute _statModified = Invalid, 
		const float _percentageModified = 0.1f, bool applyEverySecond = false);

	virtual ~BuffAbilityEffect(void);

	///<summary>Clones the current BuffAbilityEffect into its own object</summary>
	///<returns> A deep copy of the BuffAbilityEffect</returns>
	virtual BuffAbilityEffect* Clone()
	{
		return new BuffAbilityEffect(*this);
	}

	///<summary>Gets the type of effect as a string that matches the class name</summary>
	///<returns>A string matching the name of the class</returns>
	virtual inline std::string GetEffectType()
	{
		return "BuffAbilityEffect";
	}

	///<summary>Updates and applies the effect to the specified target</summary>
	///<param name="gameTime">The time that has passed between frames</param>
	///<param name="target">The Probender to apply the effect to</param>
	virtual void Update(const float gameTime, Probender* const target);
};

