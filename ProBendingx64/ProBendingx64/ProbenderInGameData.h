#pragma once
#include "ProbenderFlags.h"
#include <array>
#include "StatusEffectModifiers.h"

struct ProbenderData;//Forward declaration

struct AttributeBonuses
{
	float HealthRegenRate, FocusRegenRate;
	float AttackBonus, AttackSpeed;
	float DefenseBonus, AgilityBonus;
	float LuckBonus;

	AttributeBonuses(float healthRegenRate = 0.0f, float focusRegenRate = 0.0f, float attackBonus = 0.0f,
		float attackSpeed = 0.0f, float defenseBonus = 0.0f, float agilityBonus = 0.0f, float luckBonus = 0.0f)
		:HealthRegenRate(healthRegenRate), FocusRegenRate(focusRegenRate), AttackBonus(attackBonus), AttackSpeed(attackSpeed),
		DefenseBonus(defenseBonus), AgilityBonus(agilityBonus), LuckBonus(luckBonus)
	{
	}
};

struct SkillsBonuses
{
	float DodgeSpeed, JumpHeight, FlyDrainRate;
	//Element Catch Data
	float BlockStrength, CounterAttackChance, CounterAttackDamage;
	//Arena Interaction Data
	float HealingAmount, HealCost;

	SkillsBonuses(float dodgeSpeed = 0.0f, float jumpHeight = 0.0f, float flyDrainRate = 0.0f,
		float blockStrength = 0.0f, float counterAttackChance = 0.0f, float counterAttackDamage = 0.0f,
		float healingAmount = 0.0f, float healCost = 0.0f)
		:DodgeSpeed(dodgeSpeed), JumpHeight(jumpHeight), FlyDrainRate(flyDrainRate), 
		BlockStrength(blockStrength), CounterAttackChance(counterAttackChance), CounterAttackDamage(counterAttackDamage),
		HealingAmount(healingAmount), HealCost(healCost)
	{
	}
};

struct EquipmentBonuses
{

};

struct AbilityBonuses
{
	ElementEnum::Element Element;//The element the ability pertains to
	typedef std::array<unsigned short, ProbenderAbilities::NUM_ABILITY_MODIFIERS> SkillPointArray;

	SkillPointArray SpecialAbility1;//Special ability 1 skill points distribution
	SkillPointArray SpecialAbility2;//Special ability 2 skill points distribution
	SkillPointArray SpecialAbility3;//Special ability 3 skill points distribution

	float MasteryDamageBonus; //Damage bonus from points invested in Mastery Ability
	float MasteryFocusRegenBonus; //Focus bonus from points invested in Mastery Ability

	AbilityBonuses(ElementEnum::Element element = ElementEnum::InvalidElement, SkillPointArray specialAbilityPoints1 = SkillPointArray(),
		SkillPointArray specialAbilityPoints2 = SkillPointArray(), SkillPointArray specialAbilityPoints3 = SkillPointArray(),
		float masteryDamageBonus = 0.0f, float masteryFocusRegenBonus = 0.0f)
		:Element(element), SpecialAbility1(specialAbilityPoints1), SpecialAbility2(specialAbilityPoints2), 
		SpecialAbility3(specialAbilityPoints3), MasteryDamageBonus(masteryDamageBonus), MasteryFocusRegenBonus(masteryFocusRegenBonus)
	{
	}

};

struct ProbenderInGameData
{
	StatusEffectModifiers StatusModifiers;//Status effect modifiers
	SkillsBonuses SkillsBonus;//Skill bonuses
	AbilityBonuses ElementAbilities;//Element ability point bonuses
	AbilityBonuses SubelementAbilities;//Subelement ability point bonuses

	EquipmentBonuses EquipmentEffects;//Bonuses from equipment

	///<summary>Gets the Main Element of the character</summary>
	///<returns>The element associated with the character. If Invalid, then the Ability Structures
	///for this character are also invalid</returns>
	inline const ElementEnum::Element GetMainElement()const{return ElementAbilities.Element;}

	///<summary>Gets the Sub Element of the character</summary>
	///<returns>The subelement associated with the character. If Invalid, then the Subelement Ability Structure
	///for this character is invalid or the character doesn't have a sub element</returns>
	inline const ElementEnum::Element GetSubElement()const{return SubelementAbilities.Element;}

	void FillFromProbenderData(const ProbenderData& data);

private:
	static const float MAX_DODGE_SPEED;//The number of frames until a dodge maneuver is completed
	static const float MIN_DODGE_SPEED;

	float CalculateDodgeSpeed(const short dodgeSkill);
};