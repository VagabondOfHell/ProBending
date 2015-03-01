

#include "Game.h"
#include "InputManager.h"
#include "GameScene.h"
#include "PhysXSerializerWrapper.h"

#define USE_PHYSX_COOKING

#ifdef USE_PHYSX_COOKING

#include "PhysXCookingWrapper.h"
#endif // USE_PHYSX_COOKING

#include <OgreConfigFile.h>
#include <OgreException.h>
#include <OgreRenderSystem.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include "RandomNumberGenerator.h"

SpeechController speechController = SpeechController(NULL);

physx::PxPhysics* Game::gPhysicsSDK;
//Custom error reporter
PhysXErrorReporter Game::gMyPhysXErrorReporter;
//Default Allocator
physx::PxDefaultAllocator Game::gDefaultAllocatorCallback;

Game::Game()
    : mRoot(0), mResourcesCfg(Ogre::StringUtil::BLANK),
	mPluginsCfg(Ogre::StringUtil::BLANK)
{
	mRoot = NULL;
	mWindow = NULL;
	
	mInputManager = NULL;
	mMouse = NULL;
	mKeyboard = NULL;
	mMainLog = NULL;
	
	sceneManager = NULL;
	
	mProfileZoneManager = NULL;
	mPvdConnection = NULL;

	//Load appropriate files
	#ifdef _DEBUG
		mResourcesCfg = "resources_d.cfg";
		mPluginsCfg = "plugins_d.cfg";
	#else
		mResourcesCfg = "resources.cfg";
		mPluginsCfg = "plugins.cfg";
	#endif
}

Game::~Game()
{
	CloseGame();

	//Remove ourself as a Window listener
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
	delete mRoot;
}

//Adjust mouse clipping area
void Game::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);
 
    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}
 
//Unattach OIS before window shutdown (very important under Linux)
void Game::windowClosed(Ogre::RenderWindow* rw)
{
    //Only close for window that created OIS
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

void Game::InitializeRootResourcesAndPlugins()
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

	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("General");
}

void Game::CreateOgreWindow(const Ogre::String& windowTitle = "Ogre App")
{
	mWindow = mRoot->initialise(true, windowTitle);

	//Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	//Create a log and register this class to listen to it
	mMainLog = Ogre::LogManager::getSingleton().createLog("Main Log", false, true, true);
	mMainLog->addListener(this);
}

void Game::InitializeOIS(bool useBufferedInput)
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
 pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
   pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
    pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
    pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
    pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));

	mInputManager = OIS::InputManager::createInputSystem( pl );

	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, useBufferedInput ));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, useBufferedInput ));

	mKeyboard->setEventCallback(InputNotifier::GetInstance());
	mMouse->setEventCallback(InputNotifier::GetInstance());

	//Set initial mouse clipping size
	windowResized(mWindow);
}

void Game::InitializePhysX()
{
	//Create the foundation of the physX SDK to check for SDK Version validity and create Allocation and Error callbacks
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gMyPhysXErrorReporter);
	
	//If in debug mode, activate the Profiling Manager
#if _DEBUG
	mProfileZoneManager = &physx::PxProfileZoneManager::createProfileZoneManager(foundation);		if(!mProfileZoneManager)		printf("PxProfileZoneManager::createProfileZoneManager failed!");

	gPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, 
		physx::PxTolerancesScale(), true, mProfileZoneManager);	
#else
	gPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), false, NULL);
#endif
	
#ifdef USE_PHYSX_COOKING
	physx::PxCookingParams pa = physx::PxCookingParams(gPhysicsSDK->getTolerancesScale());
	PhysXCookingWrapper::CreateCooking(PX_PHYSICS_VERSION, *foundation, pa);
#endif // USE_PHYSX_COOKING

	PxInitExtensions(*gPhysicsSDK);
}

bool Game::ConnectToPVD()
{
	if (!gPhysicsSDK->getPvdConnectionManager())
	{
		std::cout << "Warning: PhysX Visual Debugger not found running!\n";
		return false;
	}

	if(gPhysicsSDK->getPvdConnectionManager()->isConnected())
		return true;

	const char* pvdHostIP = "127.0.0.1";
	int port = 5425;
	unsigned int timeout = 100;
	physx::PxVisualDebuggerConnectionFlags flags =
		physx::PxVisualDebuggerConnectionFlag::eDEBUG
		| physx::PxVisualDebuggerConnectionFlag::eMEMORY
		| physx::PxVisualDebuggerConnectionFlag::ePROFILE;
	
	// Create connection with PhysX Visual Debugger
	mPvdConnection = physx::PxVisualDebuggerExt::createConnection(
	gPhysicsSDK->getPvdConnectionManager(),	pvdHostIP,	port, timeout, flags);
 
	if (mPvdConnection)
	{
		std::cout << "Connected to PhysX Visual Debugger!\n";
 
		gPhysicsSDK->getVisualDebugger()->setVisualizeConstraints(true);
		gPhysicsSDK->getVisualDebugger()->setVisualDebuggerFlag(physx::PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
		gPhysicsSDK->getVisualDebugger()->setVisualDebuggerFlag(physx::PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true);

		return true;
	}

	return false;
}

void Game::messageLogged(const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug, 
		const Ogre::String &logName, bool &skipThisMessage)
{
	printf(message.c_str());
	printf("\n");
}

void Game::InitializeGame()
{
	InitializeRootResourcesAndPlugins();
	//Manually set the Rendering System
	Ogre::RenderSystem *rs = mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
	rs->setConfigOption("Full Screen","No");
	rs->setConfigOption("Video Mode","1024 x 768 @ 32-bit colour");
	rs->setConfigOption("FSAA","16");
	rs->setConfigOption("Fixed Pipeline Enabled","Yes");
	rs->setConfigOption("RTT Preferred Mode","FBO");
	rs->setConfigOption("VSync","Yes");
	rs->setConfigOption("sRGB Gamma Conversion","No");

	mRoot->setRenderSystem(rs);

	CreateOgreWindow("Probending");
	///https://github.com/oysteinkrog/gpusphsim/tree/master/SPHSimOgreApp
	// Set default mipmap level (note: some APIs ignore this)
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	InitializeOIS(true);

	InitializePhysX();
}

void Game::InitializeKinect()
{
	InputManager* inputManager = InputManager::GetInstance();
	
	inputManager->InitializeKinect(mWindow->getWidth(),mWindow->getHeight());
	
	//inputManager->FillGestureReader(L"C:\\Users\\Adam\\Desktop\\Test.gbd");

	KinectSpeechReader* speechReader = inputManager->GetSpeechReader();

}

void Game::CloseGame()
{
	if(sceneManager)
	{
		delete sceneManager;
		sceneManager = NULL;
	}

	InputNotifier::DestroySingleton();
	InputManager::GetInstance()->DestroySingleton();

	RandomNumberGenerator::DestroyInstance();

	///Release Physx Systems
	if(mProfileZoneManager)
	{
		mProfileZoneManager->release();
		mProfileZoneManager = NULL;
	}

	if(mPvdConnection)
	{
		mPvdConnection->release();
		mPvdConnection = NULL;
	}

#ifdef USE_PHYSX_COOKING
	PhysXCookingWrapper::ShutdownCookingLibrary();//Checks for NULL before shutting down
#endif

	if(gPhysicsSDK)
	{
		gPhysicsSDK->release();
		gPhysicsSDK = NULL;
	}

	PhysXSerializerWrapper::CleanMemory();

	PxCloseExtensions();

	if(foundation)
	{
		foundation->release();
		foundation = NULL;
	}
	

	::FreeConsole();
}

void Game::Run()
{
	InitializeGame();
	ConnectToPVD();
	InitializeKinect();

	kinectController = BodyController();

	InputManager* inputManager = InputManager::GetInstance();

	inputManager->RegisterSensorListener(&kinectController);

	inputManager->BeginAllCapture();

	sceneManager = new SceneManager();
	sceneManager->Initialize(mWindow, mRoot);

	AllocConsole();
	freopen("conin$","r",stdin);
	freopen("conout$","w",stdout);
	freopen("conout$","w",stderr);
	printf("Debugging Window:\n");
	
	std::vector<ProbenderData> contestantData;
	ProbenderData player1Data = ProbenderData(ElementEnum::Earth);
	player1Data.TeamDatas.Team = ArenaData::BLUE_TEAM;
	player1Data.TeamDatas.CurrentZone = ArenaData::BLUE_ZONE_1;
	player1Data.TeamDatas.PlayerColour = TeamData::RED;

	player1Data.BaseAttributes.SetAttribute(ProbenderAttributes::Agility, 100);
	

	ProbenderData player2Data = ProbenderData(ElementEnum::Earth);
	player2Data.TeamDatas.Team = ArenaData::RED_TEAM;
	player2Data.TeamDatas.CurrentZone = ArenaData::RED_ZONE_1;
	player2Data.TeamDatas.PlayerColour = TeamData::BLUE;

	player2Data.BaseAttributes.SetAttribute(ProbenderAttributes::Agility, 0);


	contestantData.push_back(player1Data);
	contestantData.push_back(player2Data);

	std::shared_ptr<GameScene> gameScene(new GameScene(sceneManager, mRoot, "Probending Arena", contestantData));
	sceneManager->FlagSceneSwitch(gameScene, true);

	gameScene.reset();

	bool rendering = true;

	Ogre::Timer gameTimer = Ogre::Timer();
	float currentTime = 0;
	float previousTime = 0;

	while (rendering)
	{
		gameTimer.reset();
		
		if(!Update(currentTime / 1000.0f))
			rendering = false;

		//Ogre::WindowEventUtilities::messagePump();
#if(!MEMORY_LEAK_DETECT)
		mRoot->renderOneFrame();
#endif
		currentTime = (float)gameTimer.getMilliseconds();
		
		previousTime = currentTime;
	}
}

bool once = false;

bool Game::Update(float gameTime)
{
	if(mWindow->isClosed())
        return false;
 
    //Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();

	InputManager* inputManager = InputManager::GetInstance();
	inputManager->ProcessEvents();

	if(!sceneManager->Update(0.016f))
		return false;

    if(mKeyboard->isKeyDown(OIS::KC_ESCAPE))
        return false;


	return true;
}