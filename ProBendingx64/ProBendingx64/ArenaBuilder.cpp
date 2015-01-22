#include "ArenaBuilder.h"
#include "Arena.h"
#include "XMLReader.h"

bool ArenaBuilder::LoadArenaFromXML(Arena& arenaToFill, const std::string fileName)
{
	XMLReader xmlReader;

	if(!xmlReader.OpenFile(fileName))
		return false;



	return true;
}

bool ArenaBuilder::LoadArena(Arena& arenaToFill, const std::string arenaName)
{
	std::string fileName = "";

	if(arenaName == "ProbendingArena")
		fileName = "ProbendingArena.xml";

	return LoadArenaFromXML(arenaToFill, fileName);
}