#include "GameScene.h"
#include "Arena.h"
#include "ArenaBuilder.h"
#include "SceneManager.h"
#include "GUIManager.h"
#include "InputNotifier.h"
#include "InputManager.h"
#include "KinectGestureReader.h"
#include "KinectReader.h"
#include "CudaModuleManager.h"
#include "CollisionFilterShaders.h"
#include "PhysXSerializerWrapper.h"
#include "PhysXDataManager.h"

#include "OgreSceneManager.h"
#include "OgreRenderWindow.h"
#include "OgreCamera.h"
#include "OgreViewport.h"

#include "PxScene.h"
#include "geometry/PxConvexMesh.h"
#include "PxPhysics.h"

bool save = false;
bool load = false;

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

physx::PxSceneDesc* GameScene::GetSceneDescription(physx::PxVec3& gravity, bool initializeCuda)
{
	//Get the default scene descriptor, because we are only changing one component of it
	physx::PxSceneDesc* sceneDescriptor = GetDefaultSceneDescription(gravity, initializeCuda);

	sceneDescriptor->filterShader = CollisionFilterShaders::GameSceneFilterShader;
	//use the following to pass Constant data to the shader
	//sceneDescriptor->filterShaderData;sceneDescriptor->filterShaderSize;

	return sceneDescriptor;
}

void GameScene::Initialize()
{
	///Create the arena first, then initialize resources
	InitializeResources(resourceGroupName);
	InitializeResources(battleArena->GetResourceGroupName());

	ogreSceneManager->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f, 1.0f));

	CreateCameraAndViewport(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 0.0f), Ogre::Vector3(0.0f, 0.0f, 40.5f));

	InputNotifier::GetInstance()->AddObserver(this);

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
	
	if(!physxSimulating && save)
	{
		PxDataManSerializeOptions options = 
			PxDataManSerializeOptions(PxDataManSerializeOptions::ALL, "SerializeCollection", "MyResources\\DataManagerData");

		if(PhysXDataManager::GetSingletonPtr()->SerializeData(options))
			printf("Data Manager Serialize Data successful\n");
		else
			printf("Data Manager Serialize Data unsuccessful \n");

		save = false;
	}

	if(!physxSimulating && load)
	{
		PxDataManSerializeOptions options = 
			PxDataManSerializeOptions(PxDataManSerializeOptions::ALL, "SerializeCollection", "MyResources\\DataManagerData");

		if(PhysXDataManager::GetSingletonPtr()->DeserializeData(options))
			printf("Data Manager Deserialize Data successful\n");
		else
			printf("Data Manager Deserialize Data unsuccessful \n");
	
		load = false;
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
		mainOgreCamera->moveRelative(Ogre::Vector3(10.0f, 0.0f, 0.0f));
	}

	if(arg.key == OIS::KC_E)
	{
		mainOgreCamera->moveRelative(Ogre::Vector3(-10.0f, 0.0f, 0.0f));
	}

	if(arg.key == OIS::KC_I)
	{
		mainOgreCamera->moveRelative(Ogre::Vector3(0.0f, 0.0f, 40.0f));
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
		mainOgreCamera->moveRelative(Ogre::Vector3(0.0f, 0.0f, -40.0f));
	}

	if(arg.key == OIS::KC_Z)
		save = true;

	if(arg.key == OIS::KC_L)
		load = true;

	return true;
}