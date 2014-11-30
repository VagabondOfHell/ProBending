#pragma once
#include "ProbenderFlags.h"

class PassiveAbility
{
public:
	unsigned short SkillPoint;
	float BaseValue;
	float ValueClimb;

	PassiveAbility(unsigned short skillPoint = 0, float baseValue = 0.0f,
		float valueClimb = 0.0f)
		:SkillPoint(skillPoint), BaseValue(baseValue), ValueClimb(valueClimb)
	{
	}

	///<summary>Gets the new value by adding the skill point * the value climb to the baseValue
	///<returns>The new calculated value. If 0 or less, there may be an invalid parameter</returns>
	virtual float GetNewValue()const
	{
		return BaseValue + (SkillPoint * ValueClimb);
	}
};

class AttributePassiveAbility
	:public PassiveAbility
{
	ProbenderAttributes::Attributes AttributeModified;

	AttributePassiveAbility(ProbenderAttributes::Attributes attribute = ProbenderAttributes::InvalidAttribute,
		unsigned short skillPoint = 0, float baseValue = 0.0f, float valueClimb = 0.0f)
		:PassiveAbility(skillPoint, baseValue, valueClimb), AttributeModified(attribute)
	{
	}

	///<summary>Gets the new value by adding the skill point * the value climb to the baseValue
	///<returns>The new calculated value. If 0 or less, there may be an invalid parameter</returns>
	virtual float GetNewValue()const
	{
		//Check for validity
		if(AttributeModified != ProbenderAttributes::InvalidAttribute)
			return BaseValue + (SkillPoint * ValueClimb);
		else
			return 0;
	}
};

class GeneralSkillPassiveAbility
	:public PassiveAbility
{
	ProbenderGeneralSkills::Skills SkillModified;

	GeneralSkillPassiveAbility(ProbenderGeneralSkills::Skills skill = ProbenderGeneralSkills::InvalidSkill,
		unsigned short skillPoint = 0, float baseValue = 0.0f, float valueClimb = 0.0f)
		:PassiveAbility(skillPoint, baseValue, valueClimb), SkillModified(skill)
	{}

	///<summary>Gets the new value by adding the skill point * the value climb to the baseValue
	///<returns>The new calculated value. If 0 or less, there may be an invalid parameter</returns>
	virtual float GetNewValue()const
	{
		//Check for validity
		if(SkillModified != ProbenderGeneralSkills::InvalidSkill)
			return BaseValue + (SkillPoint * ValueClimb);
		else
			return 0;
	}
};

class StatusModifierPassiveAbility
	:public PassiveAbility
{
	ProbenderStatusModifiers::Modifiers ModiferModified;

	StatusModifierPassiveAbility(ProbenderStatusModifiers::Modifiers modifier = ProbenderStatusModifiers::InvalidModifier,
		unsigned short skillPoint = 0, float baseValue = 0.0f, float valueClimb = 0.0f)
		:PassiveAbility(skillPoint, baseValue, valueClimb), ModiferModified(modifier)
	{}

	///<summary>Gets the new value by adding the skill point * the value climb to the baseValue
	///<returns>The new calculated value. If 0 or less, there may be an invalid parameter</returns>
	virtual float GetNewValue()const
	{
		//Check for validity
		if(ModiferModified != ProbenderStatusModifiers::InvalidModifier)
			return BaseValue + (SkillPoint * ValueClimb);
		else
			return 0;
	}
};