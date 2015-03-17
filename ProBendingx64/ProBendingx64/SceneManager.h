#pragma once
#include <memory>

namespace Ogre
{
	class Root;
	class Camera;
	class SceneManager;
	class ResourceGroupManager;
	class RenderWindow;
};

class IScene;

///This will start by using classes for scenes. Later on the scenes will be adapted to use XML
///To run and be generated

class SceneManager
{
private:
	Ogre::RenderWindow* window;

	Ogre::Camera* mainCamera;

	std::shared_ptr<IScene> currentScene;

	std::shared_ptr<IScene> sceneToSwitchTo;
	bool switchScenes;
	
	///<summary>Swaps the current scene with the passed scene</summary>
	///<param "newScene">The new scene to manage</param>
	///<returns>The old scene, or NULL if none</returns>
	std::shared_ptr<IScene> SwapScene(std::shared_ptr<IScene> newScene);

public:
	//Pointer to the Ogre Root
	Ogre::Root* ogreRoot;


	SceneManager(void);
	~SceneManager(void);

	inline Ogre::RenderWindow* GetRenderWindow()const
	{
		return window;
	}

	float GetWindowWidth()const;
	float GetWindowHeight()const;

	void Initialize(Ogre::RenderWindow* _window, Ogre::Root* _ogreRoot);

	///<summary>Gets the current scene</summary>
	///<returns>A shared pointer to the current scene</returns>
	inline std::shared_ptr<IScene> const GetCurrentScene()const
	{
		return currentScene;
	}

	///<summary>Updates the current Scene</summary>
	///<param "gameTime">The time between frames</param>
	///<returns>False to end game, true to continue</returns>
	bool Update(float gameTime);

	///<summary>Indicates to switch scenes when the current frame is finished</summary>
	///<param "newScene">The new scene to switch to</param>
	///<param "overwriteSceneSwitch">True to overwrite any scene that is currently queued to be switched, false
	///to not do anything if a switch has been indicated already</param>
	///<returns>NULL if successful, or the scene that was switched with if overwrite is true</returns>
	std::shared_ptr<IScene> FlagSceneSwitch(std::shared_ptr<IScene> newScene, bool overwriteSceneSwitch = true);
};

