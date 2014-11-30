#pragma once
#include <string>

class Probender;

class AbilityEffect
{
protected:
	float duration; //The duration the effect will last

public:
	AbilityEffect(const float _duration = 1.0f)
		:duration(_duration){}

	virtual ~AbilityEffect(void){}
	
	///<summary> Method used to return a Deep Copy of the Ability Effect</summary>
	///<returns> A deep copy of the Ability Effect</returns>
	virtual AbilityEffect* Clone() = 0;

	///<summary>Sets the effects Duration to a new value</summary>
	///<param name="newDuration">The new value the duration will be set to</param>
	virtual void SetEffectDuration(const float newDuration){duration = newDuration;}

	///<summary>Gets the type of effect as a string that matches the class name</summary>
	///<returns>A string matching the name of the class</returns>
	virtual inline std::string GetEffectType()=0;

	///<summary>Updates and applies the effect to the specified target</summary>
	///<param name="gameTime">The time that has passed between frames</param>
	///<param name="target">The Probender to apply the effect to</param>
	virtual void Update(const float gameTime, Probender* const target)
	{
		if(target)
		{
			duration -= gameTime;

			if(duration < 0.0f)
				duration = 0.0f;
		}
	}
};

