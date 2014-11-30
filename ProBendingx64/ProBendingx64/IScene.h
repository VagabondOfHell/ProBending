#pragma once
#include "SceneManager.h"
#include "GUIManager.h"
#include "OgreRenderWindow.h"
#include <string>
#include "PxPhysicsAPI.h"
//#include <gl\GL.h>

#include <OgreGLRenderSystem.h>
#include <OgreWin32Window.h>
#include <OgreWin32Context.h>
#pragma comment(lib, "opengl32")
class IScene
{
protected:

	SceneManager* owningManager;

	std::string resourceGroupName;
	
	Ogre::SceneManager* ogreSceneManager;

	Ogre::Camera* mainOgreCamera;

	GUIManager* guiManager;

	physx::PxScene* physicsWorld;
	bool physxSimulating;

	bool physxEnabled;

	physx::PxDefaultCpuDispatcher* mCpuDispatcher;
	
	//Cuda Manager for GPU Processor usage
	physx::PxCudaContextManager* cudaContextManager;

	bool started;

	inline void SetResourceGroupName(std::string _resourceGroupName)
	{
		resourceGroupName = _resourceGroupName;
	}

public:
	IScene(SceneManager* _owningManager, Ogre::Root* root, std::string _sceneName, std::string _resourceGroupName)
	{
		physicsWorld = NULL;
		physxSimulating = false;
		physxEnabled = false;

		guiManager = new GUIManager();
		guiManager->InitializeGUI();

		started = false;

		owningManager = _owningManager;

		ogreSceneManager = root->createSceneManager(Ogre::SceneType::ST_GENERIC, _sceneName);

		if(!_resourceGroupName.empty())
		{
			Ogre::ResourceGroupManager::getSingletonPtr()->initialiseResourceGroup(_resourceGroupName);
		}
		resourceGroupName = _resourceGroupName;

		mainOgreCamera = NULL;
	}

	virtual ~IScene()
	{
		UnloadResources();

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

		owningManager->ogreRoot->destroySceneManager(ogreSceneManager);

	}

	virtual void Initialize()
	{
		ogreSceneManager->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f, 1.0f));
	}

	virtual void InitializePhysics(physx::PxVec3& gravity = physx::PxVec3(0.0f, -9.8f, 0.0f), bool initializeCuda = false)
	{
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
				   
				std::string result;

				if(cudaContextManager->contextIsValid())
					result = "CUDA Context Valid";
				else
					result = "CUDA Context Not Valid";

				Ogre::LogManager::getSingleton().logMessage(Ogre::LogMessageLevel::LML_NORMAL, result.c_str());
			
				if(cudaContextManager)
					descriptor.gpuDispatcher = cudaContextManager->getGpuDispatcher();
			}
		}
		if(!descriptor.filterShader)
			descriptor.filterShader  = physx::PxDefaultSimulationFilterShader;
			
		physicsWorld = PxGetPhysics().createScene(descriptor);

		physxEnabled = true;
	}

	virtual void Start() = 0;

	virtual bool Update(float gameTime) = 0;

	virtual void Close() = 0;

	inline Ogre::Camera* GetCamera()const
	{
		return mainOgreCamera;
	}

	inline const bool GetIsStarted()const
	{
		return started;
	}

	inline const std::string GetResourceGroupName()const
	{
		return resourceGroupName;
	}

	inline const std::string GetSceneName()const
	{
		return ogreSceneManager->getName();
	}
	
	///<summary>Checks if this scene is utilizing physx for physics</summary>
	///<returns>True if physx has been initialized, false if not</returns>
	inline const bool IsUsingPhysX()const
	{
		return physxEnabled;
	}

	inline Ogre::SceneNode* GetSceneRootNode()const
	{
		return ogreSceneManager->getRootSceneNode();
	}

	inline Ogre::SceneManager* GetOgreSceneManager()const
	{
		return ogreSceneManager;
	}

	inline physx::PxCudaContextManager* const GetCudaContextManager()const
	{
		return cudaContextManager;
	}

	inline physx::PxScene* const GetPhysXScene()const
	{
		return physicsWorld;
	}

	///This method unloads all resources in the group, but in addition it removes all those 
	///resources from their ResourceManagers, and then clears all the members from the list. 
	///That means after calling this method, there are no resources declared as part of the 
	///named group any more. Resource locations still persist though. 
	virtual void ClearResources()
	{
		if(!resourceGroupName.empty())
			Ogre::ResourceGroupManager::getSingleton().clearResourceGroup(resourceGroupName);
	}

	virtual void UnloadResources()
	{
		if(!resourceGroupName.empty())
			Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(resourceGroupName);
	}

	virtual void DestroyResources()
	{
		if(!resourceGroupName.empty())
			Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(resourceGroupName);
	}

};