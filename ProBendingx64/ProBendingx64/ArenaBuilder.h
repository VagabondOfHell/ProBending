#pragma once
#include <string>

class Arena;

class ArenaBuilder
{
private:
	static bool LoadArenaFromXML(Arena& arenaToFill, const std::string fileName);

public:
	static bool LoadArena(Arena& arenaToFill, const std::string arenaName);
};

