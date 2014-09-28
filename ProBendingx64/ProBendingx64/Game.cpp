#include "Game.h"
#include "GUIManager.h"

Game::Game(void) 
{
}


Game::~Game(void)
{
	GUIManager::DestroySingleton();
	::FreeConsole();
}

void Game::Run()
{
	OgreBase* ogreBase = OgreBase::GetInstance();

	ogreBase->InitializeRootResourcesAndPlugins();

	//Manually set the Rendering System
	Ogre::RenderSystem *rs =ogreBase->mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
	rs->setConfigOption("Full Screen","No");
	rs->setConfigOption("Video Mode","1024 x 768 @ 32-bit colour");
	rs->setConfigOption("Display Frequency","50 Hz");
	rs->setConfigOption("FSAA","16");
	rs->setConfigOption("Fixed Pipeline Enabled","Yes");
	rs->setConfigOption("RTT Preferred Mode","FBO");
	rs->setConfigOption("VSync","No");
	rs->setConfigOption("sRGB Gamma Conversion","No");

	ogreBase->SetRenderSystem(rs);

	ogreBase->CreateOgreWindow("Physics Lab 1");
	
	// initialise all resource groups
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	
	// Set default mipmap level (note: some APIs ignore this)
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	//Initialize the GUI Manager
	GUIManager::GetInstance()->InitializeGUI();
	
	inputManager = InputNotifier(false);

	ogreBase->InitializeOIS(true, &inputManager, &inputManager);
	
	inputManager.AddObserver(GUIManager::GetInstance());


	/////////////////////////////////////KINECT TEST/////////////////////////////////////////
	kinectReader.InitializeKinect(ogreBase->mWindow->getWidth(), ogreBase->mWindow->getHeight());
	kinectReader.OpenBodyReader();
	
	gestureReader.Initialize(&kinectReader);
	
	KinectGestureDatabase::GetInstance()->OpenDatabase(std::wstring(L"C:\\Users\\Adam\\Desktop\\Test.gbd"));

	KinectGestureDatabase::GetInstance()->FillSourceWithAllGestures(&gestureReader);
	
	KinectGestureDatabase::GetInstance()->CloseDatabase();
	
	KinectGestureDatabase::GetInstance()->DestroySingleton();
	
	kinectController = BodyController();
	
	AllocConsole();
	freopen("conin$","r",stdin);
freopen("conout$","w",stdout);
freopen("conout$","w",stderr);
printf("Debugging Window:\n");
	/////////////////////////////////////KINECT TEST/////////////////////////////////////////

	CreateScene();
	
	ogreBase->mRoot->addFrameListener(this);

	ogreBase->BeginRender();
}

	// configure
	// Show the configuration dialog and initialise the system
	/*if(!( mRoot->showConfigDialog()))
	{
		return false;
	}*/

void Game::CreateScene()
{
	OgreBase* ogreBase = OgreBase::GetInstance();

	// Create the SceneManager, in this case a generic one
	ogreBase->mSceneMgr = ogreBase->mRoot->createSceneManager(Ogre::SceneType::ST_GENERIC, "DefaultSceneManager");
	ogreBase->mSceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1, 1));

	// Create a light
	Ogre::Light* l = ogreBase->mSceneMgr->createLight("MainLight");
	l->setPosition(20,80,50);
	
	// Create the camera
	ogreBase->mCamera = ogreBase->mSceneMgr->createCamera("PlayerCam");
	Ogre::Camera* mCamera = ogreBase->mCamera;
	
	mCamRotationSpeed = 0.13f;

	mCamNode = ogreBase->mSceneMgr->getRootSceneNode()->createChildSceneNode("CameraNode1");
	mCamNode->attachObject(mCamera);
	
	// Position it at 80 in Z direction
	mCamNode->setPosition(Ogre::Vector3(0,0,80));
	// Look back along -Z
	mCamera->lookAt(Ogre::Vector3(0,0,0));
	mCamera->setNearClipDistance(5);
	mCamera->setFarClipDistance(1000);

	// Create one viewport, entire window
	Ogre::Viewport* vp = ogreBase->mWindow->addViewport(mCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
 
	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

bool Game::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	OgreBase* instance = OgreBase::GetInstance();

	if(instance->mWindow->isClosed())
        return false;
 
    //Need to capture/update each device
    instance->mKeyboard->capture();
    instance->mMouse->capture();

	/////////////////////////////////////KINECT TEST/////////////////////////////////////////
	kinectReader.Capture();


	if(!kinectController.IsListening())
	{
		const KinectBodyReader* bodyReader = kinectReader.GetBodyReader();

		int bodyID = bodyReader->GetFirstValidUnlistenedBodyIndex();

		kinectReader.RegisterSensorListener(&kinectController);

		if(bodyID >=0)
		{
			KinectBody* body = bodyReader->GetBodyAtIndex(bodyID);
			body->AttachGestureReader(&gestureReader);
				
			if(KinectBodyEventNotifier::GetInstance()->RegisterListener(body, &kinectController))
				printf("Body Connected - Body Index: %i", bodyID);
			/*	std::cout << "Body Connected - Body Index: " << std::to_string(bodyID)
				<< " IsListening = " << kinectController.IsListening() <<
				"Tracking ID: " << std::to_string(body->GetBodyTrackingID()) << std::endl;*/
		}

	}

	

	
	/////////////////////////////////////KINECT TEST/////////////////////////////////////////

	CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);

    if(instance->mKeyboard->isKeyDown(OIS::KC_ESCAPE))
        return false;

    return true;
}