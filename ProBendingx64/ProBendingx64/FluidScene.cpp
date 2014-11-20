#include "FluidScene.h"
#include "SceneManager.h"
#include "InputNotifier.h"
#include "OgreEntity.h"

FluidScene::FluidScene(void)
	:IScene(NULL, NULL, "", "")
{
}

FluidScene::FluidScene(SceneManager* _owningManager, Ogre::Root* root, std::string _sceneName, std::string _resourceGroupName)
	:IScene(_owningManager, root, _sceneName, _resourceGroupName)
{
	
}

FluidScene::~FluidScene(void)
{
	InputNotifier::GetInstance()->RemoveObserver(guiManager);

	if(particleSystem)
		delete particleSystem;
}

void FluidScene::Start()
{
	InitializePhysics(physx::PxVec3(0.0f, -9.8f, 0.0f), true);

	mainOgreCamera = ogreSceneManager->createCamera("MainCamera");

	owningManager->GetRenderWindow()->removeAllViewports();

	Ogre::Viewport* viewport = owningManager->GetRenderWindow()->addViewport(mainOgreCamera);
	viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));

	mainOgreCamera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

	mainOgreCamera->setPosition(0, 0, 250.0f);
	mainOgreCamera->lookAt(0, 0, 0);
	mainOgreCamera->setNearClipDistance(5);
	mainOgreCamera->setFarClipDistance(10000);
		
	InputNotifier::GetInstance()->AddObserver(guiManager);
	InputNotifier::GetInstance()->AddObserver(this);

	Ogre::Light* light = ogreSceneManager->createLight();
	light->setType(Ogre::Light::LightTypes::LT_DIRECTIONAL);
	light->setAttenuation(10000, 1.0, 1, 1);
	
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName("DefaultParticleShader");
	Ogre::GpuProgramParametersSharedPtr params = material->getTechnique(0)->getPass(0)->getVertexProgramParameters();
	params->setNamedConstant("newcolor", Ogre::Vector4(1.0, 1.0, 1.0, 1.0));
	Ogre::Pass* pass = material->getTechnique(0)->getPass(0);
	//pass->setPointMaxSize(-0.500f);
	//pass->setPointMinSize(0.0f);
	pass->setPointAttenuation(true, 0, 1, 0);
pass->setPointSize(0.01f);

//pass->setPointSpritesEnabled(true);
	particleSystem = new ParticleSystem<DefaultParticlePolicy>(new DefaultParticlePolicy(1000), 
		physx::PxParticleReadDataFlag::ePOSITION_BUFFER | physx::PxParticleReadDataFlag::eFLAGS_BUFFER,
		NUM_PARTICLES, false, cudaContextManager);

	particleSystem->Initialize(physicsWorld);

	testNode = ogreSceneManager->getRootSceneNode()->createChildSceneNode();

	testNode->attachObject((DefaultParticlePolicy*)particleSystem->GetPolicy());

	testNode->translate(5.0f, 0.0f, 0.0f);
}

bool FluidScene::Update(float gameTime)
{	
	if(!physxSimulating)
	{
		physicsWorld->simulate(gameTime);
		physxSimulating = true;
	}

	if(physxSimulating)
		if(physicsWorld->checkResults())
		{
			physicsWorld->fetchResults(true);
			particleSystem->Update(gameTime);
			physxSimulating = false;
		}


	return true;
}

void FluidScene::Close()
{
	if(physxSimulating)
		if(physicsWorld->checkResults(true))
		{
			physicsWorld->fetchResults(true);
			physxSimulating = false;
		}
}

bool FluidScene::keyPressed( const OIS::KeyEvent &arg )
{
	if(arg.key == OIS::KC_UP)
	{
		Ogre::Vector3 camPos = mainOgreCamera->getPosition();
		camPos.z += 40;
		mainOgreCamera->setPosition(camPos);
	}

	if(arg.key == OIS::KC_DOWN)
	{
		Ogre::Vector3 camPos = mainOgreCamera->getPosition();
		camPos.z -= 40;
		mainOgreCamera->setPosition(camPos);
	}

	

	return true;
}