#pragma once
#include <string>
#include "foundation/PxVec3.h"

static const physx::PxVec3 PROBENDER_HALF_EXTENTS = physx::PxVec3(0.60f, 1.30f, 0.40f);

namespace RenderableJointType
{
	//Values taken from Kinect.h to avoid including the whole file
	enum RenderableJointType{
		Head = 3,
		Neck = 2,
		SpineShoulder = 20,
		SpineMid = 1,
		SpineBase = 0,
		ShoulderRight = 8,
		ElbowRight = 9,
		WristRight = 10,
		HandRight = 11,
		HandTipRight = 23,
		ShoulderLeft = 4,
		ElbowLeft = 5,
		WristLeft = 6,
		HandLeft = 7,
		ThumbRight = 24,
		HandTipLeft = 21,
		ThumbLeft = 22,
		HipLeft = 12,
		KneeLeft = 13,
		AnkleLeft = 14,
		FootLeft = 15,
		HipRight = 16,
		KneeRight = 17,
		AnkleRight = 18,
		FootRight = 19,
		Count = ThumbRight + 1
	};
};

namespace ElementEnum
{
	enum Element{InvalidElement, Air, Earth, Fire, Water, Toxin, Sonic, Astral, 
		Metal, Sand, Vine, Lightning, Laser, Lava, Ice, Blood, Acid};

	static std::string EnumToString(const Element elementToConvert)
	{
		switch (elementToConvert)
		{
		case ElementEnum::InvalidElement:
			return "Invalid Element";
			break;
		case ElementEnum::Air:
			return "Air";
			break;
		case ElementEnum::Earth:
			return "Earth";
			break;
		case ElementEnum::Fire:
			return "Fire";
			break;
		case ElementEnum::Water:
			return "Water";
			break;
		case ElementEnum::Toxin:
			return "Toxin";
			break;
		case ElementEnum::Sonic:
			return "Sonic";
			break;
		case ElementEnum::Astral:
			return "Astral";
			break;
		case ElementEnum::Metal:
			return "Metal";
			break;
		case ElementEnum::Sand:
			return "Sand";
			break;
		case ElementEnum::Vine:
			return "Vine";
			break;
		case ElementEnum::Lightning:
			return "Lightning";
			break;
		case ElementEnum::Laser:
			return "Laser";
			break;
		case ElementEnum::Lava:
			return "Lava";
			break;
		case ElementEnum::Ice:
			return "Ice";
			break;
		case ElementEnum::Blood:
			return "Blood";
			break;
		case ElementEnum::Acid:
			return "Acid";
			break;
		default:
			return "Default";
			break;
		}
		
		return "";
	}
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
	//An unsigned short's max value is 65535
	typedef unsigned short ElementFlag;

	static ElementFlag EnumToFlags(const ElementEnum::Element elementToConvert)
	{
		switch (elementToConvert)
		{
		case ElementEnum::InvalidElement:
			return ElementFlags::InvalidElement;
			break;
		case ElementEnum::Air:
			return ElementFlags::Air;
			break;
		case ElementEnum::Earth:
			return ElementFlags::Earth;
			break;
		case ElementEnum::Fire:
			return ElementFlags::Fire;
			break;
		case ElementEnum::Water:
			return ElementFlags::Water;
			break;
		case ElementEnum::Toxin:
			return ElementFlags::Toxin;
			break;
		case ElementEnum::Sonic:
			return ElementFlags::Sonic;
			break;
		case ElementEnum::Astral:
			return ElementFlags::Astral;
			break;
		case ElementEnum::Metal:
			return ElementFlags::Metal;
			break;
		case ElementEnum::Sand:
			return ElementFlags::Sand;
			break;
		case ElementEnum::Vine:
			return ElementFlags::Vine;
			break;
		case ElementEnum::Lightning:
			return ElementFlags::Lightning;
			break;
		case ElementEnum::Laser:
			return ElementFlags::Laser;
			break;
		case ElementEnum::Lava:
			return ElementFlags::Lava;
			break;
		case ElementEnum::Ice:
			return ElementFlags::Ice;
			break;
		case ElementEnum::Blood:
			return ElementFlags::Blood;
			break;
		case ElementEnum::Acid:
			return ElementFlags::Acid;
			break;
		default:
			return ElementFlags::InvalidElement;
			break;
		}
	}
};

namespace ProbenderAttributes
{
	///The maximum allowed skill points for attributes
	static const short MAX_ATTRIBUTE_POINTS_ALLOWED = 10;

	enum Attributes{InvalidAttribute, Endurance, Recovery, Strength, Defense, Agility, Luck};
};

namespace AbilityIDs
{
	typedef unsigned short AbilityID;
	enum FireAbilities{FIRE_JAB, FIRE_BLAST, FIRE_ABILITIES_COUNT};
	enum EarthAbilities{EARTH_JAB, EARTH_COIN, EARTH_ABILITIES_COUNT};
	enum WaterAbilities{WATER_JAB, WATER_RISE, WATER_ABILITIES_COUNT};

	static std::string FireEnumToString(const FireAbilities fireAbilities)
	{
		switch (fireAbilities)
		{
		case FIRE_JAB:
			return "Fire_Jab";
			break;
		case FIRE_BLAST:
			return "Fire_Blast";
			break;
		default:
			return "";
			break;
		}
	}

	static std::string EarthEnumToString(const EarthAbilities earthAbilities)
	{
		switch (earthAbilities)
		{
		case EARTH_JAB:
			return "Earth_Jab";
			break;
		case EARTH_COIN:
			return "Earth_Coin";
			break;
		default:
			return "";
			break;
		}
	}

	static std::string WaterEnumToString(const WaterAbilities waterAbilities)
	{
		switch (waterAbilities)
		{
		case AbilityIDs::WATER_JAB:
			return "Water_Jab";
			break;
		case AbilityIDs::WATER_RISE:
			return "Water_Rise";
			break;
		default:
			return "";
			break;
		}
	}
}