#include "ProbenderData.h"

const float CharacterAttributes::MAX_DODGE_SPEED = 30.0f;

const float CharacterAttributes::MIN_DODGE_SPEED = 10.0f;

const float CharacterAttributes::MIN_JUMP_HEIGHT = 6.0f;

const float CharacterAttributes::MAX_JUMP_HEIGHT = 12.0f;

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
}
