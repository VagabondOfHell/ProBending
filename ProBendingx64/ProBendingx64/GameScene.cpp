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

#include "OgreMeshManager.h"
#include "OgreSceneManager.h"
#include "OgreRenderWindow.h"
#include "OgreCamera.h"
#include "OgreViewport.h"

#include "PxScene.h"
#include "geometry/PxConvexMesh.h"
#include "PxPhysics.h"
#include "PxRigidDynamic.h"

bool save = false;
bool load = false;
bool savePhysX = false;

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

	CreateCameraAndViewport(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 0.0f), Ogre::Vector3(-10.0f, 1.0f, 0.0f));

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
			int i = 0;
			for (auto start = gameObjectList.begin();
				start != gameObjectList.end(); ++start)
			{
				start->get()->Update(gameTime);
			}

			physxSimulating = false;
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

		FindAllByName("ProbendPillarRL(Clone)");

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
		mainOgreCamera->rotate(Ogre::Vector3::UNIT_X, Ogre::Radian(Ogre::Degree(1)));
		//mainOgreCamera->yaw(Ogre::Radian(Ogre::Degree(10)));
	}

	if(arg.key == OIS::KC_NUMPAD2)
	{
		mainOgreCamera->rotate(Ogre::Vector3::UNIT_X, Ogre::Radian(Ogre::Degree(-1)));
		//mainOgreCamera->yaw(Ogre::Radian(Ogre::Degree(-10)));
	}

	if(arg.key == OIS::KC_NUMPAD4)
	{
		mainOgreCamera->rotate(Ogre::Vector3::UNIT_Y, Ogre::Radian(Ogre::Degree(1)));
		//mainOgreCamera->pitch(Ogre::Radian(Ogre::Degree(10)));
	}
	if(arg.key == OIS::KC_NUMPAD6)
	{
		mainOgreCamera->rotate(Ogre::Vector3::UNIT_Y, Ogre::Radian(Ogre::Degree(-1)));
		//mainOgreCamera->pitch(Ogre::Radian(Ogre::Degree(-10)));
	}

	if(arg.key == OIS::KC_NUMPAD7)
	{
		
		mainOgreCamera->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(Ogre::Degree(1)));
		//mainOgreCamera->roll(Ogre::Radian(Ogre::Degree(10)));
	}
	if(arg.key == OIS::KC_NUMPAD9)
	{
		mainOgreCamera->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(Ogre::Degree(-1)));
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
		save = true;

	if(arg.key == OIS::KC_F2)
		load = true;

	if(arg.key == OIS::KC_F9)
		savePhysX = true;

	return true;
}