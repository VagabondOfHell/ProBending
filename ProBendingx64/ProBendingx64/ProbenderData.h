#pragma once
#include "PassiveAbility.h"
#include "StatusEffectModifiers.h"
#include <array>

struct CharacterAttributes
{
	ElementEnum::Element MainElement;
	ElementEnum::Element SubElement;

	unsigned short Health, Focus, OffensiveStrength, DefensiveStrength, Agility, Luck;

	CharacterAttributes(ElementEnum::Element _mainElement = ElementEnum::InvalidElement, 
		ElementEnum::Element _subElement = ElementEnum::InvalidElement,
			unsigned short _health = 0, unsigned short _focus = 0, short _offensiveStrength = 0,
			unsigned short _defensiveStrength  = 0, unsigned short _agility = 0, short _luck = 0)
			:MainElement(_mainElement), SubElement(_subElement), Health(_health), Focus(_focus), 
			OffensiveStrength(_offensiveStrength), DefensiveStrength(_defensiveStrength), Agility(_agility), 
			Luck(_luck)
	{
		//Check to make sure attributes aren't too high
		if(Health > ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED)
			Health = ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED;
		if(Focus > ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED)
			Focus = ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED;
		if(OffensiveStrength > ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED)
			OffensiveStrength = ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED;
		if(DefensiveStrength > ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED)
			DefensiveStrength = ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED;
		if(Agility > ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED)
			Agility = ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED;
		if(Luck > ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED)
			Luck = ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED;
	}
};

struct GeneralSkills
{
	unsigned short Dodge, Jump, Fly, SameElementCatch, Block, 
		CounterAttack, ArenaInteraction, Healing;

	GeneralSkills(unsigned short dodge = 0, unsigned short jump = 0, unsigned short fly = 0, 
		unsigned short sameElementCatch = 0, unsigned short block = 0, unsigned short counterAttack = 0, 
		unsigned short arenaInteraction = 0, unsigned short healing = 0)
		:Dodge(dodge), Jump(jump), Fly(fly), SameElementCatch(sameElementCatch), Block(block), CounterAttack(counterAttack),
		ArenaInteraction(arenaInteraction), Healing(healing)
	{
		///Keep all the values below the highest allowed
		if(Dodge > ProbenderGeneralSkills::MAX_SKILL_POINTS_ALLOWED)
			Dodge = ProbenderGeneralSkills::MAX_SKILL_POINTS_ALLOWED;
		if(Jump > ProbenderGeneralSkills::MAX_SKILL_POINTS_ALLOWED)
			Jump = ProbenderGeneralSkills::MAX_SKILL_POINTS_ALLOWED;
		if(Fly > ProbenderGeneralSkills::MAX_SKILL_POINTS_ALLOWED)
			Fly = ProbenderGeneralSkills::MAX_SKILL_POINTS_ALLOWED;
		if(SameElementCatch > ProbenderGeneralSkills::MAX_SKILL_POINTS_ALLOWED)
			SameElementCatch = ProbenderGeneralSkills::MAX_SKILL_POINTS_ALLOWED;
		if(Block > ProbenderGeneralSkills::MAX_SKILL_POINTS_ALLOWED)
			Block = ProbenderGeneralSkills::MAX_SKILL_POINTS_ALLOWED;
		if(CounterAttack > ProbenderGeneralSkills::MAX_SKILL_POINTS_ALLOWED)
			CounterAttack = ProbenderGeneralSkills::MAX_SKILL_POINTS_ALLOWED;
		if(ArenaInteraction > ProbenderGeneralSkills::MAX_SKILL_POINTS_ALLOWED)
			ArenaInteraction = ProbenderGeneralSkills::MAX_SKILL_POINTS_ALLOWED;
		if(Healing > ProbenderGeneralSkills::MAX_SKILL_POINTS_ALLOWED)
			Healing = ProbenderGeneralSkills::MAX_SKILL_POINTS_ALLOWED;
	}
};

struct AbilityPoints
{
	typedef std::array<unsigned short, ProbenderAbilities::NUM_ABILITY_MODIFIERS> SkillPointArray;
	
	PassiveAbility* Passive;
	SkillPointArray SpecialAbility1;//Array of ability skill points
	SkillPointArray SpecialAbility2;
	SkillPointArray SpecialAbility3;
	unsigned short MasteryAbility;
	
	AbilityPoints(PassiveAbility* passiveAbility = 0, unsigned short masteryAbilityPoints = 0,
		SkillPointArray specialAbilityPoints1 = SkillPointArray(), SkillPointArray specialAbilityPoints2 = SkillPointArray(),
		SkillPointArray specialAbilityPoints3 = SkillPointArray())
		:Passive(passiveAbility), MasteryAbility(masteryAbilityPoints), SpecialAbility1(specialAbilityPoints1), 
		SpecialAbility2(specialAbilityPoints2), SpecialAbility3(specialAbilityPoints3)
	{
	}

	~AbilityPoints()
	{
		if(Passive)
			delete Passive;
	}
};

struct EquipmentData
{
	
};

struct ProbenderData
{
	CharacterAttributes Attributes;
	StatusEffectModifiers StatusModifiers;
	GeneralSkills Skills;
	AbilityPoints Abilities;
	EquipmentData Equipment;

	ProbenderData(CharacterAttributes attributes = CharacterAttributes(), 
		StatusEffectModifiers statusModifiers = StatusEffectModifiers(),
		GeneralSkills generalSkills = GeneralSkills(), AbilityPoints abilityPoints = AbilityPoints(),
		EquipmentData equipment = EquipmentData())
		: Attributes(attributes), StatusModifiers(statusModifiers), Skills(generalSkills), Abilities(abilityPoints),
		Equipment(equipment)
	{	}
	
};