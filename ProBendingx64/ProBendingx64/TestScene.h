#pragma once
#include "IScene.h"

class TestScene :
	public IScene
{
public:
	TestScene();

	TestScene(SceneManager* _owningManager, Ogre::Root* root, std::string _sceneName, std::string _resourceGroupName);

	virtual ~TestScene(void);

	virtual void Start();

	virtual bool Update(float gameTime);

	virtual void Close();
};

