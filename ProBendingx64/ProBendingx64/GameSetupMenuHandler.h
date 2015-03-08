#pragma once
#include "MenuHandler.h"

class GameSetupMenuHandler: public MenuHandler
{
protected:
	CEGUI::PushButton* gameModeLeftSelect;
	CEGUI::PushButton* gameModeRightSelect;

public:
	GameSetupMenuHandler(IScene* scene);
	virtual ~GameSetupMenuHandler(void);
};

