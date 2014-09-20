#pragma once
#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>
#include <OgreEntity.h>
#include <OgreRenderWindow.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

class OgreBase:public Ogre::WindowEventListener, public Ogre::LogListener
{
private:
protected:
	
	//Resources File
	Ogre::String mResourcesCfg;
	//Plugin File
    Ogre::String mPluginsCfg;
	
	// OIS Input devices
	OIS::InputManager* mInputManager;
	
	OgreBase(void);

	// Ogre::WindowEventListener
	virtual void windowResized(Ogre::RenderWindow* rw);
	virtual void windowClosed(Ogre::RenderWindow* rw);

	//What happens when the message is logged
	virtual void messageLogged(const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug, 
		const Ogre::String &logName, bool &skipThisMessage);
public:
	//Ogre Root
    Ogre::Root* mRoot;

	//OIS Devices
	OIS::Mouse*    mMouse;
	OIS::Keyboard* mKeyboard;
	
	//Scene Manager
	Ogre::SceneManager* mSceneMgr;
	//Camera
	Ogre::Camera* mCamera;
	//Render Window
	Ogre::RenderWindow* mWindow;
	//Log
	Ogre::Log* mMainLog;

	//Get the instance of the singleton
	static OgreBase* GetInstance();

	//Destroy the singleton instance
	static void DestroySingleton();

	//Sets the name and path of the Resources.cfg to use. Call this before
	//InitializeRootResourcesAndPlugins if not using default
	void SetResourcesConfigFile(const Ogre::String& fileName);

	//Sets the name and path of the Plugins.cfg to use. Call this before
	//InitializeRootResourcesAndPlugins if not using default
	void SetPluginConfigFile(const Ogre::String& fileName);

//------------------------------INITIALIZATION-----------------------------//
	//Initializes the Root, Resource File, and Plugins. This method should be
	//called first. Uses default resource and plugin files unless changed
	virtual void InitializeRootResourcesAndPlugins();

	//Set the render system that will be used
	virtual void SetRenderSystem( Ogre::RenderSystem* renderSystem)const;

	//Create an Auto-Generated Ogre Window with the specified title
	virtual void CreateOgreWindow(const Ogre::String& windowTitle);

	//Initialize the OIS Input System
	virtual void InitializeOIS(const bool bufferedInput, OIS::MouseListener* _mouseListener, OIS::KeyListener* _keyboardListener);

    virtual ~OgreBase(void);

	//Abstract
	virtual void BeginRender();

	//Adds a frame listener to the scene manager
	//<param: listener> The listener to add
	virtual void AddFrameListener(Ogre::FrameListener* listener);

	//Loads an Entity, performing error checking as well
	//<param: entityName> The name to assign to the entity
	//<param: fileName> The name of the mesh to load
	//<return> returns a pointer to the entity, or NULL if failed
	virtual Ogre::Entity* LoadEntity(const Ogre::String& entityName, const Ogre::String& fileName);
};

