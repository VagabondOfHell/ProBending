#include "HelperFunctions.h"
struct ProjectileAttributes
{
	Ogre::Vector3 HalfExtents;
	bool UseGravity;
	float MinDamage, MaxDamage;
	float MinKnockback, MaxKnockback;
	float MinSpeed, MaxSpeed;
	
	float AttackBonus;

	ProjectileAttributes(Ogre::Vector3 halfExtents = Ogre::Vector3(0.0f), float minDamage = 0.0f, float maxDamage = 0.0f, 
		float minKnockback = 0.0f, float maxKnockback = 0.0f, float minSpeed = 0.0f, float maxSpeed = 0.0f, bool useGravity = false)
		:HalfExtents(halfExtents),
		MinDamage(minDamage), MaxDamage(maxDamage), MinKnockback(minKnockback), MaxKnockback(maxKnockback),
		MinSpeed(minSpeed), MaxSpeed(maxSpeed), AttackBonus(0.0f), UseGravity(useGravity)
	{
		HelperFunctions::ValidateMinMax(MinDamage, MaxDamage);
		HelperFunctions::ValidateMinMax(MinKnockback, MaxKnockback);
		HelperFunctions::ValidateMinMax(MinSpeed, MaxSpeed);
	}
};