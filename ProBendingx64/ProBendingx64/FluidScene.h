#pragma once
#include "IScene.h"
#include "GameObject.h"
#include "InputObserver.h"

class ParticlePointEmitter;
class ParticleSystemBase;
class ParticleComponent;
class Projectile;
class Probender;

class FluidScene :
	public IScene, public InputObserver
{
private:
	static const int NUM_PARTICLES = 1000;

	GameObject* gameObject;
	Probender* probender;

	ParticleSystemBase* particleSystem;
	ParticleSystemBase* particleSystem2;

	ParticleComponent* particleComponent;
	ParticleComponent* particleComponent2;

	ParticlePointEmitter* particlePointEmitter;

	Projectile* projectile;

	Ogre::SceneNode* testNode;
	Ogre::SceneNode* testNode2;

public:
	FluidScene(void);
	FluidScene(SceneManager* _owningManager, Ogre::Root* root, std::string _sceneName, std::string _resourceGroupName);

	~FluidScene(void);

	virtual void Initialize();

	virtual void Start();

	virtual bool Update(float gameTime);

	virtual void Close();

	virtual bool keyPressed( const OIS::KeyEvent &arg );
	virtual bool keyReleased( const OIS::KeyEvent &arg ){return true;}
	virtual bool mouseMoved( const OIS::MouseEvent &arg ) {return true;}
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {return true;}
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ){return true;}
};

