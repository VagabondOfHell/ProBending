#pragma once
#include "IScene.h"
#include "OgreParticleSystem.h"
#include "GameObject.h"
#include "ParticleRenderer.h"

class FluidScene :
	public IScene
{
private:
	static const int NUM_PARTICLES = 10000;

	Ogre::ParticleSystem* sunParticle;
	Ogre::SceneNode* particleNode;

	physx::PxVec3 particlePositions[NUM_PARTICLES];
	physx::PxVec3 particleVelocities[NUM_PARTICLES];
	physx::PxU32 particleIndices[NUM_PARTICLES];

	ParticleRenderer* particleRenderer;

	GameObject* object;
	Ogre::SceneNode* testNode;

public:
	FluidScene(void);
	FluidScene(SceneManager* _owningManager, Ogre::Root* root, std::string _sceneName, std::string _resourceGroupName);

	~FluidScene(void);

	virtual void Start();

	virtual bool Update(float gameTime);

	virtual void Close();
};

