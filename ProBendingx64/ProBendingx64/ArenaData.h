#pragma once
#include "TagsAndLayersManager.h"

namespace ArenaData
{
	enum Team{INVALID_TEAM, RED_TEAM, BLUE_TEAM};
	enum Zones{INVALID_ZONE, RED_ZONE_1, RED_ZONE_2, RED_ZONE_3, BLUE_ZONE_1, BLUE_ZONE_2, BLUE_ZONE_3};

	enum RaycastFilterGroups{
		NONE = 0,
		GROUND = 1 << 0,
		WATER = 1 << 1,
		ZONE_TRIGGER = 1 << 2,
		CONTESTANT = 1 << 3,
		PLATFORM = 1 << 4,
		PILLAR = 1 << 5,
		WALL = 1 << 6,
		ARENA_SURFACE = 1 << 7,
		GUARD_RAIL = 1 << 8,
		PROJECTILE = 1 << 9,
		ALL = GROUND + WATER + ZONE_TRIGGER + CONTESTANT + PLATFORM + PILLAR + WALL + ARENA_SURFACE + GUARD_RAIL + PROJECTILE
	};

	typedef unsigned int RaycastFilter;

	///<summary>Gets the zone enum from the specified string</summary>
	///<param name="objectName">The name of the object to check</param>
	///<returns>The specified zone, or INVALID_ZONE if not found. Must be one of the 6 options
	///from TagsAndLayersManager to be valid</returns>
	static Zones GetZoneFromString(const std::string& objectName)
	{
		if(objectName == TagsAndLayersManager::RedZone1)
			return RED_ZONE_1;
		else if(objectName == TagsAndLayersManager::RedZone2)
			return RED_ZONE_2;
		else if(objectName == TagsAndLayersManager::RedZone3)
			return RED_ZONE_3;
		else if(objectName == TagsAndLayersManager::BlueZone1)
			return BLUE_ZONE_1;
		else if(objectName == TagsAndLayersManager::BlueZone2)
			return BLUE_ZONE_2;
		else if(objectName == TagsAndLayersManager::BlueZone3)
			return BLUE_ZONE_3;
		else
			return INVALID_ZONE;
	}

	static std::string GetStringFromZone(const Zones zone)
	{
		switch (zone)
		{
		case ArenaData::RED_ZONE_1:
			return TagsAndLayersManager::RedZone1;
			break;
		case ArenaData::RED_ZONE_2:
			return TagsAndLayersManager::RedZone2;
			break;
		case ArenaData::RED_ZONE_3:
			return TagsAndLayersManager::RedZone3;
			break;
		case ArenaData::BLUE_ZONE_1:
			return TagsAndLayersManager::BlueZone1;
			break;
		case ArenaData::BLUE_ZONE_2:
			return TagsAndLayersManager::BlueZone2;
			break;
		case ArenaData::BLUE_ZONE_3:
			return TagsAndLayersManager::BlueZone3;
			break;
		default:
			return "Invalid Zone";
			break;
		}
	}
};