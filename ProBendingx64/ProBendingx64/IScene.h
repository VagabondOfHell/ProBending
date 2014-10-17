#pragma once
#include "SceneManager.h"
#include "GUIManager.h"
#include "OgreRenderWindow.h"
#include <string>
#include "PxPhysicsAPI.h"

class IScene
{
protected:

	SceneManager* owningManager;

	std::string resourceGroupName;
	
	Ogre::SceneManager* ogreSceneManager;

	Ogre::Camera* mainOgreCamera;

	GUIManager* guiManager;

	physx::PxScene* physicsWorld;
	physx::PxDefaultCpuDispatcher* mCpuDispatcher;

	bool started;

	inline void SetResourceGroupName(std::string _resourceGroupName)
	{
		resourceGroupName = _resourceGroupName;
	}

public:
	IScene(SceneManager* _owningManager, Ogre::Root* root, std::string _sceneName, std::string _resourceGroupName)
	{
		physicsWorld = NULL;

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

		owningManager->ogreRoot->destroySceneManager(ogreSceneManager);

	}

	virtual void Initialize()
	{
		ogreSceneManager->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f, 1.0f));
	}

	virtual void InitializePhysics(physx::PxVec3& gravity)
	{
		physx::PxSceneDesc descriptor(PxGetPhysics().getTolerancesScale());
		
		descriptor.gravity = gravity;
	
		if(!descriptor.cpuDispatcher)
		{
			mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
			descriptor.cpuDispatcher = mCpuDispatcher;
		}

		if(!descriptor.filterShader)
			descriptor.filterShader  = physx::PxDefaultSimulationFilterShader;
			
		physicsWorld = PxGetPhysics().createScene(descriptor);
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
	
	inline Ogre::SceneNode* GetSceneRootNode()const
	{
		return ogreSceneManager->getRootSceneNode();
	}

	inline Ogre::SceneManager* GetOgreSceneManager()const
	{
		return ogreSceneManager;
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