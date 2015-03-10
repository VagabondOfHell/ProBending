#include "MenusScene.h"

#include "InputNotifier.h"
#include "GUIManager.h"
#include "OgreSceneManager.h"

#include "CharacterMenuHandler.h"
#include "GameSetupMenuHandler.h"

MenusScene::MenusScene(void)
	:IScene(NULL, NULL, "MenusScene", "MenusResources")
{
	for (unsigned int i = 0; i < Screens::Count; ++i)
	{
		handlers[i] = NULL;
	}
}

MenusScene::MenusScene(SceneManager* _owningManager, Ogre::Root* root, Screens screenToSet)
	:IScene(_owningManager, root, "MenusScene", "MenusResources"), currentScreen(screenToSet)
{
	for (unsigned int i = 0; i < Screens::Count; ++i)
	{
		handlers[i] = NULL;
	}
}

MenusScene::~MenusScene(void)
{
	for (unsigned int i = 0; i < Screens::Count; ++i)
	{
		if(handlers[i])
		{
			delete handlers[i];
		}
	}
}

void MenusScene::Initialize()
{
	InitializeResources(resourceGroupName);

	player1Nav = MenuNavigator(this);
	player2Nav = MenuNavigator(this);

	Player1Data.MainElement = ElementEnum::Air;
	Player2Data.MainElement = ElementEnum::Air;

	InputNotifier::GetInstance()->AddObserver(&player1Nav);
	InputNotifier::GetInstance()->AddObserver(&player2Nav);

	guiManager->AddScheme("MainMenu.scheme");
	guiManager->LoadLayout("MainMenuLayout.layout", nullptr);
	guiManager->LoadLayout("GameSetupMenu.layout", nullptr);
	guiManager->LoadLayout("CharacterSetupMenu.layout", nullptr);

	handlers[Screens::MainMenu] = new MainMenuHandler(this);
	handlers[Screens::CharacterSetup] = new CharacterMenuHandler(this);
	handlers[Screens::GameSetup] = new GameSetupMenuHandler(this);

	for (unsigned int i = 0; i < Screens::Count; i++)
	{
		handlers[i]->Hide();
	}

	handlers[currentScreen]->Show();

	ogreSceneManager->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f, 1.0f));

	CreateCameraAndViewport(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 0.0f));

}

void MenusScene::Start()
{
}

bool MenusScene::Update(float gameTime)
{
	return true;
}

void MenusScene::Close()
{

}

void MenusScene::SetScreen(Screens screenToSet)
{
	if(currentScreen == screenToSet || screenToSet == Count)
		return;

	handlers[currentScreen]->Hide();
	//handlers[currentScreen]->Disable();

	currentScreen = screenToSet;

	handlers[currentScreen]->Show();
	//handlers[currentScreen]->Enable();

	switch (screenToSet)
	{
	case MenusScene::MainMenu:
		break;
	case MenusScene::CharacterSetup:
		break;
	case MenusScene::GameSetup:
		break;
	default:
		break;
	}
}

void MenusScene::SwitchToGame()
{

}
