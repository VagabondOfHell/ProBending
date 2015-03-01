#pragma once
#include "ProbenderFlags.h"
#include "ArenaData.h"

#include <array>

struct BodyDimensions
{
	float RightArmLength;
	float LeftArmLength;
	float RightLegLength;
	float LeftLegLength;
	float RightHipToKnee;
	float LeftHipToKnee;

	BodyDimensions(float rightArmLength = 0.4f, float leftArmLength = 0.4f, float rightLegLength = 0.6f, float leftLegLength = 0.6f,
		float rightHipToKnee = 0.3f, float leftHipToKnee = 0.3f )
		:RightArmLength(rightArmLength), LeftArmLength(leftArmLength), RightLegLength(rightLegLength), LeftLegLength(leftLegLength),
		RightHipToKnee(rightHipToKnee), LeftHipToKnee(leftHipToKnee)
	{

	}
};

struct CharacterAttributes
{
private:
	static const float MAX_DODGE_SPEED;
	static const float MIN_DODGE_SPEED;
	static const float MIN_JUMP_HEIGHT;
	static const float MAX_JUMP_HEIGHT;

	unsigned short Endurance, Spirit, Strength, Defense, Agility, Luck;

	float MaxHealth, MaxFocus;
	float HealthRegenRate, FocusRegenRate;
	float AttackBonus, AttackSpeed;
	float DefenseBonus, AgilityBonus;
	float DodgeSpeed, JumpHeight;
	float LuckBonus;

	void CalculateStats();

public:
	float Health, Focus;

	CharacterAttributes(ElementEnum::Element _mainElement = ElementEnum::InvalidElement, 
		ElementEnum::Element _subElement = ElementEnum::InvalidElement,
			unsigned short _endurance = 0, unsigned short _spirit = 0, short _strength = 0,
			unsigned short _defense = 0, unsigned short _agility = 0, short _luck = 0)
			:Endurance(_endurance), Spirit(_spirit), 
			Strength(_strength), Defense(_defense), Agility(_agility), 
			Luck(_luck)
	{
		//Check to make sure attributes aren't too high
		if(Endurance > ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED)
			Endurance = ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED;
		if(Spirit > ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED)
			Spirit = ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED;
		if(Strength > ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED)
			Strength = ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED;
		if(Defense > ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED)
			Defense = ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED;
		if(Agility > ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED)
			Agility = ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED;
		if(Luck > ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED)
			Luck = ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED;

		CalculateStats();
	}

	inline void SetAttribute(ProbenderAttributes::Attributes attribute, unsigned short value)
	{
		switch (attribute)
		{
		case ProbenderAttributes::Endurance:
			Endurance = value;
			break;
		case ProbenderAttributes::Spirit:
			Spirit = value;
			break;
		case ProbenderAttributes::Strength:
			Strength = value;
			break;
		case ProbenderAttributes::Defense:
			Defense = value;
			break;
		case ProbenderAttributes::Agility:
			Agility = value;
			break;
		case ProbenderAttributes::Luck:
			Luck = value;
			break;
		default:
			return;
			break;
		}
		CalculateStats();
	}

	inline float GetDodgeSpeed()const{return DodgeSpeed;}
	inline float GetJumpHeight()const{return JumpHeight;}
};

struct TeamData
{
	enum ContestantColour{INVALID_COLOUR, RED, BLUE, GREEN, YELLOW, PURPLE, ORANGE};

	inline static std::string EnumToString(const ContestantColour colour)
	{
		switch (colour)
		{
		case TeamData::RED:
			return "Red";
			break;
		case TeamData::BLUE:
			return "Blue";
			break;
		case TeamData::GREEN:
			return "Green";
			break;
		case TeamData::YELLOW:
			return "Yellow";
			break;
		case TeamData::PURPLE:
			return "Purple";
			break;
		case TeamData::ORANGE:
			return "Orange";
			break;
		default:
			return "";
			break;
		}
	}

	ArenaData::Team Team;
	ArenaData::Zones CurrentZone;
	ContestantColour PlayerColour;

	TeamData(ArenaData::Team _team = ArenaData::INVALID_TEAM, 
		ArenaData::Zones _zone = ArenaData::INVALID_ZONE, ContestantColour _colour = INVALID_COLOUR)
		:Team(_team), CurrentZone(_zone), PlayerColour(_colour)
	{

	}
};

struct ProbenderData
{
	unsigned short Level;

	ElementEnum::Element MainElement;
	ElementEnum::Element SubElement;

	ElementEnum::Element CurrentElement;

	CharacterAttributes BaseAttributes;
	CharacterAttributes CurrentAttributes;

	TeamData TeamDatas;

	ProbenderData(ElementEnum::Element _mainElement = ElementEnum::InvalidElement, ElementEnum::Element _subElement = ElementEnum::InvalidElement, 
		unsigned short level = 0, CharacterAttributes baseAttributes = CharacterAttributes(), TeamData _teamData = TeamData())
		: MainElement(_mainElement), SubElement(_subElement), CurrentElement(_mainElement), Level(level), 
		BaseAttributes(baseAttributes), CurrentAttributes(baseAttributes), TeamDatas(_teamData)
	{	}
	
};