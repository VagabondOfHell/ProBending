#pragma once
#include "ProbenderInGameData.h"

#include "ProbenderData.h"

const float ProbenderInGameData::MAX_DODGE_SPEED = 30.0f;
const float ProbenderInGameData::MIN_DODGE_SPEED = 10.0f;

void ProbenderInGameData::FillFromProbenderData(const ProbenderData& data)
{
	ElementAbilities.Element = data.Attributes.MainElement;
	SubelementAbilities.Element = data.Attributes.SubElement;

	SkillsBonus.DodgeSpeed = CalculateDodgeSpeed(data.Skills.Dodge);
}

float ProbenderInGameData::CalculateDodgeSpeed(const short dodgeSkill)
{
	//Give 1% of the speed for a 0 skill
	if(dodgeSkill == 0)
		return MIN_DODGE_SPEED;

	//Get the amount of time the dodge should take by using the inverse of the percentage of skill points placed in the skill
	//so if dodgeSkill = 35 and MAX_ATTRIBUTE_POINTS = 100 and baseDodgeTime = 0.5
	//0.5 * (1 - (35 / 100))
	//0.5 * (1 - 0.35)
	//0.5 * 0.65
	//0.325f is the amount of time it should take to complete the dodge
	return ((MAX_DODGE_SPEED - MIN_DODGE_SPEED) * ((float)dodgeSkill / ProbenderGeneralSkills::MAX_SKILL_POINTS_ALLOWED) + MIN_DODGE_SPEED);
}
