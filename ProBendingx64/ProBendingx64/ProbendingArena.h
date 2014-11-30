#pragma once
#include "GameObject.h"
#include <map>

class ProbendingArena
{
private:
	std::map<std::string, GameObject*>staticGameObjects;
	std::map<std::string, GameObject*>dynamicGameObjects;

public:
	ProbendingArena(void);
	virtual ~ProbendingArena(void);

	void Update(float gameTime);
};

