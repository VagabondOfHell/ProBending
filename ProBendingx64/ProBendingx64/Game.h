#pragma once
#include "ogrebase.h"
#include "InputNotifier.h"
#include "KinectReader.h"
#include "BodyReader.h"
#include "KinectController.h"

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
	BodyReader bodyReader;
	KinectController* kinectController;

	Ogre::SceneNode* mCamNode;

	Game(void);
	
	void Run();

	virtual ~Game(void);

	//Creates a new scene
	void CreateScene();
};

