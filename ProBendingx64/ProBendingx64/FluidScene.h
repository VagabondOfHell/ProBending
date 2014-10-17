#pragma once
#include "IScene.h"
#include "OgreParticleSystem.h"
#include "GameObject.h"

class FluidScene :
	public IScene
{
private:
	Ogre::ParticleSystem* sunParticle;
	Ogre::SceneNode* particleNode;

	GameObject* object;

public:
	FluidScene(void);
	FluidScene(SceneManager* _owningManager, Ogre::Root* root, std::string _sceneName, std::string _resourceGroupName);

	~FluidScene(void);

	virtual void Start();

	virtual bool Update(float gameTime);

	virtual void Close();
};

