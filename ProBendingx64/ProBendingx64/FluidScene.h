#pragma once
#include "IScene.h"
#include "GameObject.h"
#include "ParticleSystem.h"
#include "DefaultParticlePolicy.h"
#include "InputObserver.h"

class FluidScene :
	public IScene, public InputObserver
{
private:
	static const int NUM_PARTICLES = 1000;

	ParticleSystem<DefaultParticlePolicy>* particleSystem;

	Ogre::SceneNode* testNode;

public:
	FluidScene(void);
	FluidScene(SceneManager* _owningManager, Ogre::Root* root, std::string _sceneName, std::string _resourceGroupName);

	~FluidScene(void);

	virtual void Start();

	virtual bool Update(float gameTime);

	virtual void Close();

	virtual bool keyPressed( const OIS::KeyEvent &arg );
	virtual bool keyReleased( const OIS::KeyEvent &arg ){return true;}
	virtual bool mouseMoved( const OIS::MouseEvent &arg ) {return true;}
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {return true;}
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ){return true;}
};

