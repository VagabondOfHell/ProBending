#pragma once
#include <string>

class Arena;

class ArenaBuilder
{
public:
	static void LoadArena(Arena& arenaToFill, const std::string arenaName);
};

