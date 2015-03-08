#include "GameSetupMenuHandler.h"
#include "GUIManager.h"

GameSetupMenuHandler::GameSetupMenuHandler(IScene* scene)
	:MenuHandler(scene)
{
	rootWindow = guiManager->GetChildWindow("SetupMenuRoot");
}


GameSetupMenuHandler::~GameSetupMenuHandler(void)
{
}
