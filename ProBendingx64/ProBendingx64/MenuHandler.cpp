#include "MenuHandler.h"

#include "MenusScene.h"

#include "GUIManager.h"

#include "CEGUI/Event.h"
#include "CEGUI/widgets/PushButton.h"

MenuHandler::MenuHandler(IScene* _scene)
	:scene(_scene), rootWindow(nullptr)
{
	guiManager = scene->GetGUIManager();
}

bool MenuHandler::IsVisible()
{
	if(rootWindow)
		return rootWindow->isVisible();

	return false;
}

void MenuHandler::Show()
{
	if(rootWindow)
		rootWindow->setVisible(true);
}

void MenuHandler::Hide()
{
	if(rootWindow)
		rootWindow->setVisible(false);
}

MainMenuHandler::MainMenuHandler(IScene* _scene)
	:MenuHandler(_scene)
{
	rootWindow = guiManager->GetChildWindow("MainMenuRoot");

	startButton = (CEGUI::PushButton*)guiManager->GetChildWindow("MainMenuRoot/EnterTournamentButton");
	startButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenuHandler::StartButtonClickEvent,this));
}

MainMenuHandler::~MainMenuHandler()
{

}

bool MainMenuHandler::StartButtonClickEvent(const CEGUI::EventArgs& e)
{
	((MenusScene*)scene)->SetScreen(MenusScene::GameSetup);

	return true;
}
