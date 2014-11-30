#include "StatusAbilityEffect.h"


StatusAbilityEffect::StatusAbilityEffect(void)
{
}

StatusAbilityEffect::StatusAbilityEffect(const float _duration, DamageType _effectType, 
		float _damage, float _damagePerSecond, float _chance)
		:AbilityEffect(_duration), damageType(_effectType), damage(_damage), damagePerSecond(_damagePerSecond), chance(_chance)
{

}

StatusAbilityEffect::~StatusAbilityEffect(void)
{
}

void StatusAbilityEffect::Update(const float gameTime, Probender* const target)
{
	
}