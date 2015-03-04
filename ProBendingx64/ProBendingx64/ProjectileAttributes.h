#include "HelperFunctions.h"
struct ProjectileAttributes
{
	float MinDamage, MaxDamage;
	float MinKnockback, MaxKnockback;
	float MinSpeed, MaxSpeed;

	float AttackBonus;

	ProjectileAttributes(float minDamage = 0.0f, float maxDamage = 0.0f, 
		float minKnockback = 0.0f, float maxKnockback = 0.0f, float minSpeed = 0.0f, float maxSpeed = 0.0f)
		:MinDamage(minDamage), MaxDamage(maxDamage), MinKnockback(minKnockback), MaxKnockback(maxKnockback),
		MinSpeed(minSpeed), MaxSpeed(maxSpeed), AttackBonus(0.0f)
	{
		HelperFunctions::ValidateMinMax(MinDamage, MaxDamage);
		HelperFunctions::ValidateMinMax(MinKnockback, MaxKnockback);
		HelperFunctions::ValidateMinMax(MinSpeed, MaxSpeed);
	}
};