#include "SceneManager.h"
#include "IScene.h"
#include "GUIManager.h"

SceneManager::SceneManager(void)
{
	switchScenes = false;
	sceneToSwitchTo = NULL;
}

SceneManager::~SceneManager(void)
{
	GUIManager::DestroySystem();
}

std::shared_ptr<IScene> SceneManager::SwapScene(std::shared_ptr<IScene> newScene)
{
	std::shared_ptr<IScene> oldScene = currentScene;

	if(oldScene)
	{
		oldScene->Close();
		oldScene.reset();
	}

	currentScene = newScene;

	currentScene->Start();

	return oldScene;
}

void SceneManager::Initialize(Ogre::RenderWindow* _window, Ogre::Root* _ogreRoot)
{
	window = _window;
	ogreRoot = _ogreRoot;

	GUIManager::BootstrapSystem();
}

bool SceneManager::Update(float gameTime)
{
	//If we were flagged to switch scenes, do so
	if(switchScenes)
	{
		SwapScene(sceneToSwitchTo);
		switchScenes = false;
		sceneToSwitchTo = nullptr;
	}

	if(currentScene)
		return currentScene->Update(gameTime);

	return true;
}

std::shared_ptr<IScene> SceneManager::FlagSceneSwitch(std::shared_ptr<IScene> newScene, bool overwriteSceneSwitch)
{
	if(newScene != NULL)
	{
		//if there isn't a previous flag to switch scenes with
		if(!switchScenes)
		{
			//set
			switchScenes = true;
			sceneToSwitchTo = newScene;
		}
		else
		{
			if(overwriteSceneSwitch)
			{
				//if there is a previous flag, return the old one
				std::shared_ptr<IScene> oldSwitch = sceneToSwitchTo;
				sceneToSwitchTo = newScene;
				return oldSwitch;
			}
		}
	}

	return NULL;
}