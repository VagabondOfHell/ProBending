#pragma once
#include <OgreRoot.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include "SceneManager.h"
#include "InputNotifier.h"
#include "BodyController.h"
#include "SpeechController.h"

#include "PxPhysicsAPI.h"
#include "PhysXErrorReporter.h"

class Game:public Ogre::WindowEventListener, public Ogre::LogListener
{
private:
	//Ogre Root
    Ogre::Root* mRoot;

	//Render Window
	Ogre::RenderWindow* mWindow;

	//Resources File
	Ogre::String mResourcesCfg;
	//Plugin File
    Ogre::String mPluginsCfg;

	// OIS Input devices
	OIS::InputManager* mInputManager;
	//OIS Devices
	OIS::Mouse*    mMouse;
	OIS::Keyboard* mKeyboard;

	//Log
	Ogre::Log* mMainLog;
	
	//Scene manager
	SceneManager* sceneManager;

	//Version Checker
	physx::PxFoundation* foundation;
	//The base of PhysX
	static physx::PxPhysics* gPhysicsSDK;
	//Custom error reporter
	static PhysXErrorReporter gMyPhysXErrorReporter;
	//Default Allocator
	static physx::PxDefaultAllocator gDefaultAllocatorCallback;
	//For Profiling (in Debug Mode)
	physx::PxProfileZoneManager* mProfileZoneManager;

	//Initializes the Root, Resource File, and Plugins. This method should be
	//called first. Uses default resource and plugin files unless changed
	virtual void InitializeRootResourcesAndPlugins();

	//Create an Auto-Generated Ogre Window with the specified title
	virtual void CreateOgreWindow(const Ogre::String& windowTitle);

	//Initialize the OIS Input System
	virtual void InitializeOIS(bool useBufferedInput = true);
	
	///Initialize the PhysX system
	void InitializePhysX();

	bool ConnectToPVD();

	void InitializeKinect();
	void InitializeGame();

protected:
	// Ogre::WindowEventListener
	virtual void windowResized(Ogre::RenderWindow* rw);
	virtual void windowClosed(Ogre::RenderWindow* rw);

	//What happens when the message is logged
	virtual void messageLogged(const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug, 
		const Ogre::String &logName, bool &skipThisMessage);

public:
	Game();
	~Game();

	void CloseGame();

	BodyController kinectController;

	Ogre::SceneNode* mCamNode;

	void Run();

	bool Update(float gameTime);
};

