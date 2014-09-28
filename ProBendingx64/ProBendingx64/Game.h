#pragma once
#include "ogrebase.h"
#include "InputNotifier.h"
#include "KinectInput.h"
#include "BodyController.h"

class Game : public Ogre::FrameListener
{
private:
	float mCamRotationSpeed;
protected:
	// Ogre::FrameListener
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

public:
	InputNotifier inputManager;

	KinectReader kinectReader;
	BodyController kinectController;
	KinectGestureReader gestureReader;

	Ogre::SceneNode* mCamNode;

	Game(void);
	
	void Run();

	virtual ~Game(void);

	//Creates a new scene
	void CreateScene();
};

