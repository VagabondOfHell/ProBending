#include "MenusScene.h"

#include "InputNotifier.h"
#include "InputManager.h"

#include "GUIManager.h"
#include "OgreSceneManager.h"

#include "CharacterMenuHandler.h"
#include "GameSetupMenuHandler.h"

#include "GameScene.h"
#include "SceneManager.h"
#include "OgreRoot.h"

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

	//currentScreen = GameSetup;
	handlers[currentScreen]->Show();

	ogreSceneManager->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f, 1.0f));

	CreateCameraAndViewport(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 0.0f));

}

void MenusScene::Start()
{
}

bool MenusScene::Update(float gameTime)
{
	player1Nav.Update(gameTime);
	player2Nav.Update(gameTime);

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
	std::vector<ProbenderData> contestantData;
	/*ProbenderData player1Data = ProbenderData(ElementEnum::Fire);
	player1Data.TeamDatas.Team = ArenaData::BLUE_TEAM;
	player1Data.TeamDatas.CurrentZone = ArenaData::BLUE_ZONE_1;
	player1Data.TeamDatas.PlayerColour = TeamData::PURPLE;

	player1Data.BaseAttributes.SetAttribute(ProbenderAttributes::Agility, 10);
	player1Data.BaseAttributes.SetAttribute(ProbenderAttributes::Endurance, 5);
	player1Data.BaseAttributes.SetAttribute(ProbenderAttributes::Recovery, 5);

	ProbenderData player2Data = ProbenderData(ElementEnum::Earth);
	player2Data.TeamDatas.Team = ArenaData::RED_TEAM;
	player2Data.TeamDatas.CurrentZone = ArenaData::RED_ZONE_1;
	player2Data.TeamDatas.PlayerColour = TeamData::BLUE;

	player2Data.BaseAttributes.SetAttribute(ProbenderAttributes::Agility, 0);
	player2Data.BaseAttributes.SetAttribute(ProbenderAttributes::Endurance, 5);
	player2Data.BaseAttributes.SetAttribute(ProbenderAttributes::Recovery, 7);*/
	KinectBody* body = player1Nav.GetBody();

	if(body)
		Player1Data.BodyID = body->GetBodyID();
	else
		Player1Data.BodyID = -1;

	body = player2Nav.GetBody();

	if(body)
		Player2Data.BodyID = body->GetBodyID();
	else
		Player2Data.BodyID = -1;

	contestantData.push_back(Player1Data);
	contestantData.push_back(Player2Data);

	std::shared_ptr<GameScene> gameScene(new GameScene(owningManager, Ogre::Root::getSingletonPtr(), "Probending Arena", contestantData));
	owningManager->FlagSceneSwitch(gameScene, true);
	gameScene.reset();
}
