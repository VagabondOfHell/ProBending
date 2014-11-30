#pragma once
#include "AbilityEffect.h"

class StatusAbilityEffect :
	public AbilityEffect
{
public:
	enum DamageType{InvalidDamageType, NormalDamage, Poison, Bleed};

	float damage; //The damage to deal on first application
	float damagePerSecond; //The damage to deal per second, after initial damage
	float chance; //The chance the effect will take hold

	DamageType damageType; //The type of offensive effect this is

	StatusAbilityEffect(void);

	StatusAbilityEffect(const float _duration, DamageType _effectType, 
		float _damage, float _damagePerSecond = 0.0f, float _chance = 100.0f);

	virtual ~StatusAbilityEffect(void);
	
	///<summary>Clones the current StatusAbilityEffect into its own object</summary>
	///<returns> A deep copy of the StatusAbilityEffect</returns>
	virtual StatusAbilityEffect* Clone()
	{
		return new StatusAbilityEffect(*this);
	}

	///<summary>Gets the type of effect as a string that matches the class name</summary>
	///<returns>A string matching the name of the class</returns>
	virtual inline std::string GetEffectType()
	{
		return "StatusAbilityEffect";
	}

	///<summary>Updates and applies the effect to the specified target</summary>
	///<param name="gameTime">The time that has passed between frames</param>
	///<param name="target">The Probender to apply the effect to</param>
	virtual void Update(const float gameTime, Probender* const target);
};

