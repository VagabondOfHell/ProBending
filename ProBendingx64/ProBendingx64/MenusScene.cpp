#include "MenusScene.h"

#include "InputNotifier.h"
#include "InputManager.h"

#include "GUIManager.h"
#include "SceneManager.h"

#include "CharacterMenuHandler.h"
#include "GameSetupMenuHandler.h"

#include "GameScene.h"

#include "OgreSceneManager.h"
#include "OgreRenderWindow.h"
#include "OgreRoot.h"

#include "CEGUI/WindowManager.h"
#include "CEGUI/widgets/PushButton.h"

const float MenusScene::HOVER_TIME = 1.25f;

RadialProgressBar MenusScene::progressBar;

MenusScene::MenusScene(void)
	:IScene(NULL, NULL, "MenusScene", "MenusResources"), timePassed(0), 
	onWindow(false), hoverWindow(NULL)
{
	for (unsigned int i = 0; i < Screens::Count; ++i)
	{
		handlers[i] = NULL;
	}
}

MenusScene::MenusScene(SceneManager* _owningManager, Ogre::Root* root, Screens screenToSet)
	:IScene(_owningManager, root, "MenusScene", "MenusResources"), currentScreen(screenToSet), 
	timePassed(0), onWindow(false), hoverWindow(NULL)
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

	progressBar.SetWindow(nullptr);
}

void MenusScene::Initialize()
{
	InitializeResources(resourceGroupName);

	player1Nav = MenuNavigator(this);
	player1Nav.AllowMenuControls = true;

	player2Nav = MenuNavigator(this);

	Player1Data.MainElement = ElementEnum::Earth;
	Player2Data.MainElement = ElementEnum::Earth;

	InputNotifier::GetInstance()->AddObserver(&player1Nav);
	InputNotifier::GetInstance()->AddObserver(&player2Nav);

	guiManager->AddScheme("MainMenu.scheme");
	guiManager->LoadLayout("MainMenuLayout.layout", nullptr);
	guiManager->LoadLayout("GameSetupMenu.layout", nullptr);
	//guiManager->LoadLayout("CharacterSetupMenu.layout", nullptr);
	
	if(progressBar.GetWindow() == NULL)
	{
		CEGUI::Window* progressWindow = CEGUI::WindowManager::getSingleton().createWindow("Generic/Image", "CursorProgress");
		progressWindow->setSize( CEGUI::USize(CEGUI::UDim( 0.1f, 0 ), CEGUI::UDim( 0.1f, 0 ) ) );
		progressWindow->setPosition( CEGUI::UVector2(CEGUI::UDim( 0.5f, 0 ), CEGUI::UDim( 0.5f, 0 ) ) );
		progressWindow->setAlwaysOnTop(true);
		progressWindow->setMousePassThroughEnabled(true);

		guiManager->GetRootWindow()->addChild(progressWindow);
		progressBar.SetWindow(progressWindow);
		progressWindow->setVisible(true);
	}

	handlers[Screens::MainMenu] = new MainMenuHandler(this);
	//handlers[Screens::CharacterSetup] = new CharacterMenuHandler(this);
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
	if(!kinectConnected)
		InputManager::GetInstance()->InitializeKinect(
			owningManager->GetRenderWindow()->getWidth(), owningManager->GetRenderWindow()->getHeight());

	player1Nav.Update(gameTime);
	player2Nav.Update(gameTime);

	if(onWindow)
	{
		timePassed += gameTime;

		if(timePassed >= HOVER_TIME)
		{
			hoverWindow->fireEvent(CEGUI::PushButton::EventClicked, 
				CEGUI::WindowEventArgs(hoverWindow));

			progressBar.Reset();
			timePassed = 0.0f;
		}

		float progress = (timePassed / HOVER_TIME) * 100.0f;

		progressBar.SetProgress((unsigned int)progress);
	}

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

	progressBar.Hide();
	progressBar.Reset();

	//handlers[currentScreen]->Enable();
}

void MenusScene::SwitchToGame()
{
	std::vector<ProbenderData> contestantData;

	KinectBody* body = player1Nav.GetBody();

	if(body)
		Player1Data.BodyID = -1;// body->GetBodyID();
	else
		Player1Data.BodyID = -1;

	body = player2Nav.GetBody();

	if(body)
		Player2Data.BodyID = -1;// body->GetBodyID();
	else
		Player2Data.BodyID = -1;

	contestantData.push_back(Player1Data);
	contestantData.push_back(Player2Data);

	std::shared_ptr<GameScene> gameScene(new GameScene(owningManager, Ogre::Root::getSingletonPtr(), "Probending Arena", contestantData));
	owningManager->FlagSceneSwitch(gameScene, true);
	gameScene.reset();
}

void MenusScene::SensorDisconnected()
{
	InputManager::GetInstance()->CloseKinect();
	kinectConnected = false;
}

bool MenusScene::HoverBeginEvent(const CEGUI::EventArgs& e)
{
	timePassed = 0.0f;
	progressBar.Reset();

	progressBar.Show();

	CEGUI::WindowEventArgs& windowEvent = (CEGUI::WindowEventArgs&)e;
	onWindow = true;
	hoverWindow = windowEvent.window;

	return true;
}

bool MenusScene::HoverEndEvent(const CEGUI::EventArgs& e)
{
	progressBar.Reset();

	timePassed = 0.0f;

	onWindow = false;
	hoverWindow = NULL;

	progressBar.Hide();

	return true;
}

void MenusScene::RegisterHoverBeginEvent(CEGUI::Window* window)
{
	window->subscribeEvent(CEGUI::Window::EventMouseEntersArea, 
		CEGUI::Event::Subscriber(&MenusScene::HoverBeginEvent,this));/**/
}

void MenusScene::RegisterHoverEndEvent(CEGUI::Window* window)
{
	window->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, 
		CEGUI::Event::Subscriber(&MenusScene::HoverEndEvent,this));/**/
}

void MenusScene::UnregisterHoverBeginEvent(CEGUI::Window* window)
{
	window->removeEvent(CEGUI::Window::EventMouseEntersArea);
}

void MenusScene::UnregisterHoverEndEvent(CEGUI::Window* window)
{
	window->removeEvent(CEGUI::Window::EventMouseLeavesArea);
}

