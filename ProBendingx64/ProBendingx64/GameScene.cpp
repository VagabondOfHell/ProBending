#include "GameScene.h"
#include "Arena.h"
#include "ArenaBuilder.h"
#include "SceneManager.h"
#include "OgreSceneManager.h"
#include "OgreRenderWindow.h"
#include "OgreCamera.h"
#include "OgreViewport.h"
#include "PxScene.h"
#include "GUIManager.h"
#include "InputNotifier.h"
#include "InputManager.h"
#include "KinectGestureReader.h"
#include "KinectReader.h"
#include "CudaModuleManager.h"

GameScene::GameScene(void)
	:IScene(NULL, NULL, "", "")
{
}

 //empty string for resources. We fill this in with arena 
GameScene::GameScene(SceneManager* _owningManager, Ogre::Root* root, 
					 std::string _arenaNameToLoad, std::vector<ProbenderData> contestantData)
	:IScene(_owningManager, root, _arenaNameToLoad, "CommonArenaResources")
{
	 battleArena = new Arena(this, _arenaNameToLoad);
	 battleArena->Initialize(contestantData);
}

GameScene::~GameScene(void)
{
	if(battleArena)
		delete battleArena;

	CudaModuleManager::GetSingleton()->DestroySingleton();
}

void GameScene::Initialize()
{
	///Create the arena first, then initialize resources
	InitializeResources(resourceGroupName);
	InitializeResources(battleArena->GetResourceGroupName());

	ogreSceneManager->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f, 1.0f));

	mainOgreCamera = ogreSceneManager->createCamera("MainCamera");

	owningManager->GetRenderWindow()->removeAllViewports();

	Ogre::Viewport* viewport = owningManager->GetRenderWindow()->addViewport(mainOgreCamera);
	viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));

	InputNotifier::GetInstance()->AddObserver(this);

	mainOgreCamera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

	mainOgreCamera->setPosition(0, 0, 40.50f);
	mainOgreCamera->lookAt(0, 0, 0);
	mainOgreCamera->setNearClipDistance(0.01);
	mainOgreCamera->setFarClipDistance(10000);

	InitializePhysics(physx::PxVec3(0.0f, -9.8f, 0.0f), true);

	InputManager* inputManager = InputManager::GetInstance();

inputManager->FillGestureReader(L"C:\\Users\\Adam\\Desktop\\Capstone\\GestureData\\ProbendingGestures.gbd");
	KinectSpeechReader* speechReader = inputManager->GetSpeechReader();
	if(speechReader)
	{
		//Load the grammar for this scene
		speechReader->LoadGrammarFile("PrototypeSpeech.grxml");
		//Set the confidence threshold
		speechReader->SetConfidenceThreshold(0.6f);
	}

	
}

void GameScene::Start()
{
	battleArena->Start();
}

///<summary>Updates the game scene</summary>
///<param name="gameTime">The time passed between frames</param>
///<returns>True to continue, false to close</returns>
bool GameScene::Update(float gameTime)
{
	if(!physxSimulating)
	{
		physicsWorld->simulate(gameTime);
		physxSimulating = true;
	}

	if(physxSimulating)
		if(physicsWorld->checkResults())
		{
			physicsWorld->fetchResults(true);

			battleArena->Update(gameTime);

			physxSimulating = false;
		}
	
	return true;
}

void GameScene::Close()
{
	if(physxSimulating)
		if(physicsWorld->checkResults(true))
		{
			physicsWorld->fetchResults(true);
			physxSimulating = false;
		}
}

bool GameScene::keyPressed( const OIS::KeyEvent &arg )
{
	if(arg.key == OIS::KC_W)
	{
		Ogre::Vector3 camPos = mainOgreCamera->getPosition();
		camPos.x += 10;
		mainOgreCamera->setPosition(camPos);
	}

	if(arg.key == OIS::KC_E)
	{
		Ogre::Vector3 camPos = mainOgreCamera->getPosition();
		camPos.x -= 10;
		mainOgreCamera->setPosition(camPos);
	}

	if(arg.key == OIS::KC_I)
	{
		Ogre::Vector3 camPos = mainOgreCamera->getPosition();
		camPos.z += 40;
		mainOgreCamera->setPosition(camPos);
	}

	if(arg.key == OIS::KC_P)
	{
		mainOgreCamera->yaw(Ogre::Radian(Ogre::Degree(10)));
	}

	if(arg.key == OIS::KC_O)
	{
		mainOgreCamera->yaw(Ogre::Radian(Ogre::Degree(-10)));
	}

	if(arg.key == OIS::KC_J)
	{
		Ogre::Vector3 camPos = mainOgreCamera->getPosition();
		camPos.z -= 40;
		mainOgreCamera->setPosition(camPos);
	}

	return true;
}