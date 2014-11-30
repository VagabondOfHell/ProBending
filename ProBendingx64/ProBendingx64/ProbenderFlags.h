#pragma once

namespace ElementEnum
{
	enum Element{InvalidElement, Air, Earth, Fire, Water, Toxin, Sonic, Astral, 
		Metal, Sand, Vine, Lightning, Laser, Lava, Ice, Blood, Acid};
};

namespace ElementFlags
{
	enum ElementFlags{
		InvalidElement = 0, 
		Air = 1, 
		Earth = 1 << 1, 
		Fire = 1 << 2, 
		Water = 1 << 3, 
		Toxin = 1 << 4, 
		Sonic = 1 << 5, 
		Astral = 1 << 6, 
		Metal = 1 << 7, 
		Sand = 1 << 8, 
		Vine = 1 << 9, 
		Lightning = 1 << 10, 
		Laser = 1 << 11, 
		Lava = 1 << 12, 
		Ice = 1 << 13, 
		Blood = 1 << 14, 
		Acid = 1 << 15,
		//Sets all the above
		All = 65535
	};
};

namespace ProbenderAttributes
{
	///The maximum allowed skill points for attributes
	static const short MAX_ATTRIBUTE_POINTS_ALLOWED = 99;

	enum Attributes{InvalidAttribute, Health, Focus, OffensiveStrength, DefensiveStrength, Agility, Luck};
};

namespace ProbenderGeneralSkills
{
	///The maximum allowed skill points for skills
	static const short MAX_SKILL_POINTS_ALLOWED = 99;
	enum Skills{InvalidSkill, Dodge, Jump, Fly, SameElementCatch, Block, CounterAttack, ArenaInteraction, Healing};
};

namespace ProbenderStatusModifiers
{
	enum Modifiers{InvalidModifier, Damage, Burn, Stun, Poison, Immobilization, Distraction, Electrocution, Knockback,
		BloodControl, Freeze, AirAttack, GroundAttack, FallingMiniGame};
};

namespace ProbenderAbilities
{
	///The number of modifiers for abilities (where points are placed in)
	static const short NUM_ABILITY_MODIFIERS = 3;
	//Maximum for skill point application to special abilities
	static const short MAX_SKILL_POINTS_ALLOWED = 99;
};