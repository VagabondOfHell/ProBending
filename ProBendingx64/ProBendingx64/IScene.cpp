#include "IScene.h"
#include "GUIManager.h"
#include "SceneManager.h"
#include "OgreRenderWindow.h"
#include "OgreRoot.h"
#include "OgreSceneManager.h"

#include "PxPhysics.h"
#include "PxScene.h"
#include "extensions\PxDefaultCpuDispatcher.h"
#include "extensions\PxDefaultSimulationFilterShader.h"
#include "foundation\PxFoundation.h"
#include "pxtask\PxCudaContextManager.h"

IScene::IScene(SceneManager* _owningManager, Ogre::Root* root, std::string _sceneName, std::string _resourceGroupName)
	: physicsWorld(NULL), physxSimulating(false), physxEnabled(false), cudaContextManager(NULL), guiManager(new GUIManager()), started(false),
	owningManager(_owningManager), resourceGroupName(_resourceGroupName), mainOgreCamera(NULL)
{
	guiManager->InitializeGUI();

	ogreSceneManager = root->createSceneManager(Ogre::SceneType::ST_GENERIC, _sceneName);
}

IScene::~IScene()
{
	if(guiManager)
	{
		delete guiManager;
		guiManager = NULL;
	}

	if(mCpuDispatcher)
	{
		delete mCpuDispatcher;
		mCpuDispatcher = NULL;
	}

	if(physicsWorld)
		physicsWorld->release();

	//Delete cuda after deleting the physics world
	if(cudaContextManager)
	{
		cudaContextManager->release();
		cudaContextManager = NULL;
	}

	Ogre::ResourceGroupManager::getSingletonPtr()->unloadResourceGroup(resourceGroupName);

	//DestroyResources();
	owningManager->ogreRoot->destroySceneManager(ogreSceneManager);
}

void IScene::Initialize()
{
	InitializeResources(resourceGroupName);

	ogreSceneManager->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f, 1.0f));
}

void IScene::InitializePhysics(physx::PxVec3& gravity, bool initializeCuda)
{
	//Fill the scene description
	physx::PxSceneDesc descriptor(PxGetPhysics().getTolerancesScale());	
	descriptor.gravity = gravity;
	if(!descriptor.cpuDispatcher)
	{
		mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
		descriptor.cpuDispatcher = mCpuDispatcher;
	}
	if(initializeCuda)
	{
		if(!descriptor.gpuDispatcher)
		{
			Ogre::RenderWindow* window = (Ogre::RenderWindow*)Ogre::Root::getSingleton().getRenderSystem()->getRenderTarget("Probending");
			HWND hg;
			window->getCustomAttribute("WINDOW", &hg);

			// create cuda context manager
			physx::PxCudaContextManagerDesc cudaContextManagerDesc;
			cudaContextManagerDesc.interopMode = physx::PxCudaInteropMode::OGL_INTEROP;
			cudaContextManagerDesc.graphicsDevice = (void*)&hg;
			cudaContextManagerDesc.ctx = NULL;
			cudaContextManager = physx::PxCreateCudaContextManager(PxGetFoundation(), cudaContextManagerDesc, PxGetPhysics().getProfileZoneManager());

			///Log the result with OGRE Log if in Debug mode
			#ifdef _DEBUG
			std::string result;
			if(cudaContextManager->contextIsValid())
				result = "CUDA Context Valid";
			else
				result = "CUDA Context Not Valid";
			Ogre::LogManager::getSingleton().logMessage(Ogre::LogMessageLevel::LML_NORMAL, result.c_str());
			#endif

			//Assign the cuda context to the description
			if(cudaContextManager)
				descriptor.gpuDispatcher = cudaContextManager->getGpuDispatcher();
		}
	}
	//Set the filter shader
	if(!descriptor.filterShader)
		descriptor.filterShader  = physx::PxDefaultSimulationFilterShader;
		
	//Create the scene
	physicsWorld = PxGetPhysics().createScene(descriptor);
	//Indicate physx has been created and is therefore enabled
	physxEnabled = true;
}

const std::string IScene::GetSceneName()const{return ogreSceneManager->getName();}

Ogre::SceneNode* IScene::GetSceneRootNode()const{return ogreSceneManager->getRootSceneNode();}

void IScene::InitializeResources(const std::string& _resourceGroupName)
{
	if(!_resourceGroupName.empty())
	{
		Ogre::ResourceGroupManager::getSingletonPtr()->initialiseResourceGroup(_resourceGroupName);
	}
}

void IScene::ClearResources(const std::string& _resourceGroupName)
{
	if(!_resourceGroupName.empty())
		Ogre::ResourceGroupManager::getSingleton().clearResourceGroup(_resourceGroupName);
}

void IScene::UnloadResources(const std::string& _resourceGroupName)
{
	if(!_resourceGroupName.empty())
		Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(_resourceGroupName);
}

void IScene::DestroyResources(const std::string& _resourceGroupName)
{
	if(!_resourceGroupName.empty())
		Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(_resourceGroupName);
}