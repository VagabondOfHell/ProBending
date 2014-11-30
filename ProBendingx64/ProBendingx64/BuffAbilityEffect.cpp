#include "BuffAbilityEffect.h"


BuffAbilityEffect::BuffAbilityEffect(const float _duration, const ModifiableAttribute _statModified, 
		const float _percentageModified, bool applyEverySecond)
		:AbilityEffect(_duration), statModified(_statModified), percentageModified(_percentageModified),
		applyPerSecond(applyEverySecond), previouslyApplied(false)
{
}


BuffAbilityEffect::~BuffAbilityEffect(void)
{
}

void BuffAbilityEffect::Update(const float gameTime, Probender* const target)
{
	AbilityEffect::Update(gameTime, target);

	if(!applyPerSecond)//If meant to be applied once
	{
		if(!previouslyApplied)//Check if previously applied
		{
			//Do effect
		}
	}
	else
	{
		//Do the effect every second
	}

}