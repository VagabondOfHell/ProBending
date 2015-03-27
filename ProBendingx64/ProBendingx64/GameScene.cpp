#include "GameScene.h"
#include "Arena.h"
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
#include "SceneSerializer.h"

#include "ArenaBuilder.h"
#include "RigidBodyComponent.h"
#include "MeshRenderComponent.h"

#include "CEGUI/WindowManager.h"
#include "CEGUI/Window.h"

#include "OgreMeshManager.h"
#include "OgreSceneManager.h"
#include "OgreRenderWindow.h"
#include "OgreCamera.h"
#include "OgreViewport.h"

#include "PxScene.h"
#include "geometry/PxConvexMesh.h"
#include "PxPhysics.h"
#include "PxRigidDynamic.h"
#include "PxBatchQueryDesc.h"
#include "PxBatchQuery.h"
#include "PxSceneLock.h"
#include "PxQueryReport.h"

using namespace physx;

bool save = false;
bool load = false;
bool savePhysX = false;

bool raycast = false;

GameScene::GameScene(void)
	:IScene(NULL, NULL, "", ""), horizontalScreens(false), Camera2(nullptr), currentState(GS_COUNTDOWN)
{
}

 //empty string for resources. We fill this in with arena 
GameScene::GameScene(SceneManager* _owningManager, Ogre::Root* root, 
					 std::string _arenaNameToLoad, std::vector<ProbenderData> contestantData)
	:IScene(_owningManager, root, _arenaNameToLoad, "CommonArenaResources"), horizontalScreens(false),
	Camera2(nullptr), currentState(GS_COUNTDOWN)
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
	sceneDescriptor->flags |= physx::PxSceneFlag::eENABLE_KINEMATIC_STATIC_PAIRS;

	//use the following to pass Constant data to the shader
	//sceneDescriptor->filterShaderData;sceneDescriptor->filterShaderSize;
	collisionReporter = CollisionReporter();

	sceneDescriptor->simulationEventCallback = &collisionReporter;

	return sceneDescriptor;
}

void GameScene::SetUpCameras()
{
	//Create the camera with the name MainCamera
	mainOgreCamera = ogreSceneManager->createCamera("MainCamera");
	Camera2 = ogreSceneManager->createCamera("Camera2");

	mainOgreCamera->setPosition(Ogre::Vector3(-10.0f, 1.0f, 0.0f));
	Camera2->setPosition(Ogre::Vector3(10.0f, 1.0f, 0.0f));

	mainOgreCamera->lookAt(Ogre::Vector3(0.0f, 0.0f, 0.0f));
	Camera2->lookAt(Ogre::Vector3(0.0f, 0.0f, 0.0f));

	mainOgreCamera->setNearClipDistance(0.1f);
	mainOgreCamera->setFarClipDistance(10000.0f);

	Camera2->setNearClipDistance(0.1f);
	Camera2->setFarClipDistance(10000.0f);

	horizontalScreens = true;

	ChangeScreenSplit();
}

void GameScene::ChangeScreenSplit()
{
	unsigned short viewPortCount = owningManager->GetRenderWindow()->getNumViewports();

	for (int i = 0; i < viewPortCount; i++)
	{
		owningManager->GetRenderWindow()->getViewport(i)->clear();
	}

	//Remove any viewports from before
	owningManager->GetRenderWindow()->removeAllViewports();
	
	float width = 1.0f;
	float height = 1.0f;
	float cam2XPos = 0.0f;
	float cam2YPos = 0.0f;

	if(horizontalScreens)
	{
		height = 0.5f;
		cam2YPos = 0.5f;
	}
	else
	{
		width = 0.5f;
		cam2XPos = 0.5f;
	}

	//Add a viewport for the specified camera
	Ogre::Viewport* viewport1 = owningManager->
		GetRenderWindow()->addViewport(mainOgreCamera, 0, 0.0f, 0.0f, width, height);
	Ogre::Viewport* viewport2 = owningManager->
		GetRenderWindow()->addViewport(Camera2, 1, cam2XPos, cam2YPos, width, height);

	viewport1->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 0.0f));
	viewport2->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 0.0f));

	mainOgreCamera->setAspectRatio(Ogre::Real(viewport1->getActualWidth()) / 
		Ogre::Real(viewport1->getActualHeight()));
	
	Camera2->setAspectRatio(Ogre::Real(viewport2->getActualWidth()) / 
		Ogre::Real(viewport2->getActualHeight()));

	
}

void GameScene::Initialize()
{
	///Create the arena first, then initialize resources
	InitializeResources(resourceGroupName);
	InitializeResources(battleArena->GetResourceGroupName());

	ogreSceneManager->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f, 1.0f));

	guiManager->AddScheme("ProbendArenaGUIScheme.scheme");
	guiManager->LoadLayout("ProbendArenaGUILayout.layout", false);
	
	progressTrackerWindow = guiManager->GetChildWindow("InGameGUIRoot");

	SetUpCameras();

	InputNotifier::GetInstance()->AddObserver(this);

	InitializePhysics(physx::PxVec3(0.0f, -9.8f, 0.0f), true);
	
	MeshRenderComponent::CreatePlane("BasicPlane");

	printf("Material Count: %i\n", PhysXDataManager::GetSingletonPtr()->GetMaterialCount());

	printf("Convex Mesh Count: %i\n", PhysXDataManager::GetSingletonPtr()->GetConvexMeshCount());

	printf("Shape Count: %i \n", PhysXDataManager::GetSingletonPtr()->GetShapeCount());

	//ArenaBuilder::CreateProbendingPhysXData(this);
	//ArenaBuilder::GenerateProbendingArena(this);
	
	battleArena->DeserializeArena();
	
	printf("Material Count: %i\n", PhysXDataManager::GetSingletonPtr()->GetMaterialCount());

	printf("Convex Mesh Count: %i\n", PhysXDataManager::GetSingletonPtr()->GetConvexMeshCount());

	printf("Shape Count: %i \n", PhysXDataManager::GetSingletonPtr()->GetShapeCount());

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
	ogreSceneManager->setSkyDome(true, "CloudySkyBox");
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
	{
		if(physicsWorld->checkResults())
		{
			physicsWorld->fetchResults(true);

			battleArena->Update(gameTime);
			int i = 0;
			for (auto start = gameObjectList.begin();
				start != gameObjectList.end(); ++start)
			{
				start->get()->Update(gameTime);
			}

			physxSimulating = false;
		}
	}
	
	if(!physxSimulating && savePhysX)
	{
		PhysXSerializerWrapper::CreateSerializer();
		std::string arenaFileName =PxDataManSerializeOptions::DEFAULT_FILE_PATH + battleArena->GetArenaName() + "\\" + battleArena->GetArenaName();
		arenaFileName.erase(std::remove_if(arenaFileName.begin(), arenaFileName.end(), isspace), arenaFileName.end());

		battleArena->SavePhysXData(arenaFileName, "ArenaCollection");

		PhysXSerializerWrapper::DestroySerializer();
		savePhysX = false;
	}
	if(!physxSimulating && save)
	{
		PhysXSerializerWrapper::CreateSerializer();

		battleArena->SerializeArena();

		PhysXSerializerWrapper::DestroySerializer();
		save = false;
	}

	if(!physxSimulating && load)
	{
		PhysXSerializerWrapper::CreateSerializer();
		gameObjectList.clear();
		PhysXDataManager::GetSingletonPtr()->ReleaseAll();

		battleArena->DeserializeArena();
	
			printf("Num Actors: %i\n", GetPhysXScene()->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC));
			printf("Num Game Objects: %i\n", gameObjectList.size());

		PhysXSerializerWrapper::DestroySerializer();
		
		load = false;
	}

	return true;
}

void GameScene::SetGameState(GameState newState)
{
	if(currentState == newState)
		return;

	switch (newState)
	{
	case GameScene::GS_COUNTDOWN:
		break;
	case GameScene::GS_TRANSITION:
		break;
	case GameScene::GS_GAMEPLAY:
		break;
	}
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

void GameScene::keyPressed( const OIS::KeyEvent &arg )
{
	if(arg.key == OIS::KC_W)
	{
		mainOgreCamera->moveRelative(Ogre::Vector3(1.0f, 0.0f, 0.0f));
	}

	if(arg.key == OIS::KC_E)
	{
		mainOgreCamera->moveRelative(Ogre::Vector3(-1.0f, 0.0f, 0.0f));
	}

	if(arg.key == OIS::KC_I)
	{
		mainOgreCamera->moveRelative(Ogre::Vector3(0.0f, 0.0f, 1.0f));
	}

	if(arg.key == OIS::KC_NUMPAD8)
	{
		mainOgreCamera->rotate(Ogre::Vector3::UNIT_X, Ogre::Radian(Ogre::Degree(10)));
		//mainOgreCamera->yaw(Ogre::Radian(Ogre::Degree(10)));
	}

	if(arg.key == OIS::KC_NUMPAD2)
	{
		mainOgreCamera->rotate(Ogre::Vector3::UNIT_X, Ogre::Radian(Ogre::Degree(-10)));
		//mainOgreCamera->yaw(Ogre::Radian(Ogre::Degree(-10)));
	}

	if(arg.key == OIS::KC_NUMPAD4)
	{
		mainOgreCamera->rotate(Ogre::Vector3::UNIT_Y, Ogre::Radian(Ogre::Degree(10)));
		//mainOgreCamera->pitch(Ogre::Radian(Ogre::Degree(10)));
	}
	if(arg.key == OIS::KC_NUMPAD6)
	{
		mainOgreCamera->rotate(Ogre::Vector3::UNIT_Y, Ogre::Radian(Ogre::Degree(-10)));
		//mainOgreCamera->pitch(Ogre::Radian(Ogre::Degree(-10)));
	}

	if(arg.key == OIS::KC_NUMPAD7)
	{
		
		mainOgreCamera->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(Ogre::Degree(10)));
		//mainOgreCamera->roll(Ogre::Radian(Ogre::Degree(10)));
	}
	if(arg.key == OIS::KC_NUMPAD9)
	{
		mainOgreCamera->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(Ogre::Degree(-10)));
		//mainOgreCamera->roll(Ogre::Radian(Ogre::Degree(-10)));
	}

	if(arg.key == OIS::KC_M)
		mainOgreCamera->moveRelative(Ogre::Vector3(0.0f, -1.0f, 0.0f));

	if(arg.key == OIS::KC_N)
		mainOgreCamera->moveRelative(Ogre::Vector3(0.0f, 1.0f, 0.0f));

	if(arg.key == OIS::KC_J)
	{
		mainOgreCamera->moveRelative(Ogre::Vector3(0.0f, 0.0f, -1.0f));
	}

	if(arg.key == OIS::KC_F11)
	{
			//save = true;
	}
	if(arg.key == OIS::KC_F2)
	{
	//	load = true;
	}

	if(arg.key == OIS::KC_F9)
	{
		//savePhysX = true;
	}

	if(arg.key == OIS::KC_R)
	{
		//ChangeScreenOrientation(!horizontalScreens);
	}
	//	raycast = true;
}