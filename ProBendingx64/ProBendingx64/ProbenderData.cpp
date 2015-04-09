#include "ProbenderData.h"

const float CharacterAttributes::MAX_DODGE_SPEED = 30.0f;
const float CharacterAttributes::MIN_DODGE_SPEED = 10.0f;

const float CharacterAttributes::MIN_JUMP_HEIGHT = 24.0f;
const float CharacterAttributes::MAX_JUMP_HEIGHT = 36.0f;

const float CharacterAttributes::MIN_ENERGY_VAL = 10.0f;
const float CharacterAttributes::MAX_ENERGY_VAL = 100.0f;

const float CharacterAttributes::MIN_ENERGY_REGEN_RATE = 25.0f;
const float CharacterAttributes::MAX_ENERGY_REGEN_RATE = 50.0f;

const float CharacterAttributes::MIN_RECOVERY_TIME = 1.0f;
const float CharacterAttributes::MAX_RECOVERY_TIME = 0.1f;

const float CharacterAttributes::MIN_ATTACK_BONUS = 1.0f;
const float CharacterAttributes::MAX_ATTACK_BONUS = 2.0f;

const float CharacterAttributes::MIN_ATTACK_SPEED_BONUS = 1.0f;
const float CharacterAttributes::MAX_ATTACK_SPEED_BONUS = 2.0f;

void CharacterAttributes::CalculateStats()
{
	//Give 1% of the speed for a 0 skill
	if(Agility == 0)
	{
		DodgeSpeed = MIN_DODGE_SPEED;
		JumpHeight = MIN_JUMP_HEIGHT;
	}
	else
	{
		DodgeSpeed = ((MAX_DODGE_SPEED - MIN_DODGE_SPEED) * 
			((float)Agility / ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED) + MIN_DODGE_SPEED);

		JumpHeight = ((MAX_JUMP_HEIGHT - MIN_JUMP_HEIGHT) * 
			((float)Agility / ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED) + MIN_JUMP_HEIGHT);
	}

	if(Endurance == 0)
	{
		MaxEnergy = MIN_ENERGY_VAL;
	}
	else
	{
		MaxEnergy = ((MAX_ENERGY_VAL - MIN_ENERGY_VAL) * 
			((float)Endurance / ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED) + MIN_ENERGY_VAL);
	}

	if(Recovery == 0)
	{
		EnergyRegenRate = MIN_ENERGY_REGEN_RATE;
		RecoveryRate = MIN_RECOVERY_TIME;
	}
	else
	{
		EnergyRegenRate = ((MAX_ENERGY_REGEN_RATE - MIN_ENERGY_REGEN_RATE) * 
			((float)Recovery / ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED) + MIN_ENERGY_REGEN_RATE);
		RecoveryRate = ((MIN_RECOVERY_TIME - MAX_RECOVERY_TIME) *
			((float)(ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED - Recovery) / 
			ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED) + MAX_RECOVERY_TIME);
	}
	
	if(Strength == 0)
	{
		AttackSpeed = MIN_ATTACK_SPEED_BONUS;
		AttackBonus = MIN_ATTACK_BONUS;
	}
	else
	{
		AttackSpeed = ((MAX_ATTACK_SPEED_BONUS - MIN_ATTACK_SPEED_BONUS) * 
			((float)Strength / ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED) + MIN_ATTACK_SPEED_BONUS);
		AttackBonus = ((MAX_ATTACK_BONUS - MIN_ATTACK_BONUS) *
			((float)Strength / ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED) + MIN_ATTACK_BONUS);
	}
}


