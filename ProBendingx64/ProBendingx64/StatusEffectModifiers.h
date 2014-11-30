#pragma once

struct StatusEffectModifiers
{
private:
	///<summary>Clamps a float between a range. If minVal is greater than maxVal, there are undefined results.
	///No validity checking performed.</summary>
	///<param name="valueToClamp">The value passed by reference to be clamped</param>
	///<param name="minVal">The minimum value allowed</param>
	///<param name="maxVal">The maximum value allowed</param>
	static inline void ClampFloat(float& valueToClamp, const float minVal, const float maxVal)
	{
		//Ternary that checks if the value is less then the min. If it is, sets the min
		//If it isn't, checks if value is greater than maxVal. If it is, sets max, otherwise returns unmodified
		valueToClamp = valueToClamp < minVal ? valueToClamp = minVal : 
			(valueToClamp > maxVal ? valueToClamp = maxVal : valueToClamp);
	}

public:
	///The highest percentage allowed for the values (80% resistance/weakness allowed)
	static const short HIGHEST_VALUE = 80;

	float Damage, Burn, Stun, Poison, Immobilization, Distraction, Electrocution, Knockback,
		BloodControl, Freeze, AirAttack, GroundAttack, FallingMiniGame;

	StatusEffectModifiers(float damage = 0.0f, float burn = 0.0f, float stun = 0.0f, 
		float poison = 0.0f, float immobilization = 0.0f, float distraction = 0.0f, 
		float electrocution = 0.0f, float knockback = 0.0f, float bloodControl = 0.0f,
		float freeze = 0.0f, float airAttack = 0.0f, float groundAttack = 0.0f, 
		float fallingMinigame = 0.0f)
		:Damage(damage), Burn(burn), Poison(poison), Immobilization(immobilization), 
		Distraction(distraction), Electrocution(electrocution), Knockback(knockback),
		BloodControl(bloodControl), Freeze(freeze), AirAttack(airAttack), GroundAttack(groundAttack),
		FallingMiniGame(fallingMinigame)
	{
		///Clamp all the values between negative highest and positive highest
		ClampFloat(Damage, -HIGHEST_VALUE, HIGHEST_VALUE);
		ClampFloat(Burn, -HIGHEST_VALUE, HIGHEST_VALUE);
		ClampFloat(Poison, -HIGHEST_VALUE, HIGHEST_VALUE);
		ClampFloat(Immobilization, -HIGHEST_VALUE, HIGHEST_VALUE);
		ClampFloat(Distraction, -HIGHEST_VALUE, HIGHEST_VALUE);
		ClampFloat(Electrocution, -HIGHEST_VALUE, HIGHEST_VALUE);
		ClampFloat(Knockback, -HIGHEST_VALUE, HIGHEST_VALUE);
		ClampFloat(BloodControl, -HIGHEST_VALUE, HIGHEST_VALUE);
		ClampFloat(Freeze, -HIGHEST_VALUE, HIGHEST_VALUE);
		ClampFloat(AirAttack, -HIGHEST_VALUE, HIGHEST_VALUE);
		ClampFloat(GroundAttack, -HIGHEST_VALUE, HIGHEST_VALUE);
		ClampFloat(FallingMiniGame, -HIGHEST_VALUE, HIGHEST_VALUE);
	}
};