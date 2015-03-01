#include "HelperFunctions.h"
struct ProjectileAttributes
{
	float MinDamage;
	float MaxDamage;
	float MinKnockback;
	float MaxKnockback;

	ProjectileAttributes(float minDamage = 0.0f, float maxDamage = 0.0f, 
		float minKnockback = 0.0f, float maxKnockback = 0.0f)
		:MinDamage(minDamage), MaxDamage(maxDamage), MinKnockback(minKnockback), MaxKnockback(maxKnockback)
	{
		HelperFunctions::ValidateMinMax(minDamage, maxDamage);
		HelperFunctions::ValidateMinMax(minKnockback, MaxKnockback);
	}
};