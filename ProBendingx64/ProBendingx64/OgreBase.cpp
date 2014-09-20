#include "OgreBase.h"
 
#include <OgreConfigFile.h>
#include <OgreException.h>
#include <OgreRenderSystem.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreWindowEventUtilities.h>

OgreBase* instance;

OgreBase::OgreBase(void)
    : mRoot(0), mResourcesCfg(Ogre::StringUtil::BLANK),
	mPluginsCfg(Ogre::StringUtil::BLANK)
{
//Load appropriate files
#ifdef _DEBUG
	mResourcesCfg = "resources_d.cfg";
	mPluginsCfg = "plugins_d.cfg";
#else
	mResourcesCfg = "resources.cfg";
	mPluginsCfg = "plugins.cfg";
#endif
}
//-------------------------------------------------------------------------------------
OgreBase::~OgreBase(void)
{
    //Remove ourself as a Window listener
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
	delete mRoot;
}

//Adjust mouse clipping area
void OgreBase::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);
 
    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}
 
//Unattach OIS before window shutdown (very important under Linux)
void OgreBase::windowClosed(Ogre::RenderWindow* rw)
{
    //Only close for window that created OIS (the main window in these demos)
    if(rw == mWindow)
    {
        if(mInputManager)
        {
            mInputManager->destroyInputObject( mMouse );
            mInputManager->destroyInputObject( mKeyboard );
 
            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
}

OgreBase* OgreBase::GetInstance()
{
	if(!instance)
		instance = new OgreBase();

	return instance;
}

void OgreBase::DestroySingleton()
{
	if(instance)
	{
		delete instance;
		instance = nullptr;
	}
}

void OgreBase::SetResourcesConfigFile(const Ogre::String& fileName)
{
	mResourcesCfg = fileName;
}

void OgreBase::SetPluginConfigFile(const Ogre::String& fileName)
{
	mPluginsCfg = fileName;
}

void OgreBase::InitializeRootResourcesAndPlugins()
{ 
    // construct Ogre::Root
    mRoot = new Ogre::Root(mPluginsCfg);
	
	// set up resources
	// Load resource paths from config file
	Ogre::ConfigFile cf;
	cf.load(mResourcesCfg);

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
 
	/*secName is the name of each section: Essential, Popular, General
    typeName is the type of the resource being defined: FileSystem (folder) or Zip file
    archName is an absolute path to the resource */
	Ogre::String secName, typeName, archName;
	//Loop through each section and store the location of the resource
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
		}
	}
}

void OgreBase::SetRenderSystem(Ogre::RenderSystem* renderSystem)const
{
	mRoot->setRenderSystem(renderSystem);
}

void OgreBase::CreateOgreWindow(const Ogre::String& windowTitle = "Ogre App")
{
	mWindow = mRoot->initialise(true, windowTitle);

	//Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	//Create a log and register this class to listen to it
	mMainLog = Ogre::LogManager::getSingleton().createLog("Main Log", false, true, true);
	mMainLog->addListener(this);
}

void OgreBase::InitializeOIS(const bool bufferedInput, OIS::MouseListener* _mouseListener, OIS::KeyListener* _keyboardListener)
{
	//Set up logger
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");

	//Initialize OIS
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;
 
	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
 
	mInputManager = OIS::InputManager::createInputSystem( pl );

	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, bufferedInput ));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, bufferedInput ));
	
	mKeyboard->setEventCallback(_keyboardListener);
	mMouse->setEventCallback(_mouseListener);

	//Set initial mouse clipping size
	windowResized(mWindow);
}

void OgreBase::AddFrameListener(Ogre::FrameListener* listener)
{
	mRoot->addFrameListener(listener);
}

void OgreBase::messageLogged(const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug, 
		const Ogre::String &logName, bool &skipThisMessage)
{
	std::cout<< message <<std::endl;
}

Ogre::Entity* OgreBase::LoadEntity(const Ogre::String& entityName, const Ogre::String& fileName)
{
	Ogre::Entity* loadedEntity = NULL;

	try
	{
		loadedEntity = mSceneMgr->createEntity(entityName, fileName);
	}
	catch(Ogre::Exception e)
	{
		mMainLog->logMessage(e.getFullDescription());
	};
	
	return loadedEntity;
}

void OgreBase::BeginRender()
{ 
	mRoot->startRendering();
}

