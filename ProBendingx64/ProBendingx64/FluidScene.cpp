#include "FluidScene.h"
#include "SceneManager.h"
#include "InputNotifier.h"
#include "OgreEntity.h"
#include "ParticlePointEmitter.h"
#include "BasicParticleSystem.h"

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

	if(particlePointEmitter)
		delete particlePointEmitter;

	if(particleSystem)
		delete particleSystem;

	if(particleSystem2)
		delete particleSystem2;
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
	Ogre::GpuProgramParametersSharedPtr fragParams = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	params->setNamedConstant("newcolor", Ogre::Vector4(1.0, 1.0, 1.0, 0.50));
	
	Ogre::MaterialPtr redDefault = material->clone("RedDefaultParticleShader");
	params = redDefault->getTechnique(0)->getPass(0)->getVertexProgramParameters();
	params->setNamedConstant("newcolor", Ogre::Vector4(1.0f, 0.0f, 0.0f, 0.50f));

	//Ogre::Pass* pass = material->getTechnique(0)->getPass(0);
	/*pass->setPointAttenuation(true, 0, 1, 0);
	pass->setPointSize(00.10f);*/

	particlePointEmitter = new ParticlePointEmitter(10, physx::PxVec3(0.0f),
		physx::PxVec3(0, -3, 0).getNormalized(), physx::PxVec3(0, 3, 0).getNormalized(), 1.0f, 4.0f);
	
	ParticleSystemParams psParams = ParticleSystemParams();
	psParams.cudaContext = cudaContextManager;
	psParams.useGravity = false;

	particleSystem = new BasicParticleSystem(particlePointEmitter, NUM_PARTICLES, psParams, false);
	particleSystem2 = new BasicParticleSystem(particlePointEmitter, NUM_PARTICLES, psParams, false);

	/*particleSystem = new ParticleSystem<DefaultParticlePolicy>(new DefaultParticlePolicy(), particlePointEmitter, 
		NUM_PARTICLES, false, ParticleSystemParams(), false, cudaContextManager);
	
	particleSystem2 = new ParticleSystem<DefaultParticlePolicy>(new DefaultParticlePolicy(), particlePointEmitter, 
		NUM_PARTICLES, false, ParticleSystemParams(), false, cudaContextManager);*/

	particleSystem->Initialize(physicsWorld);
	particleSystem2->Initialize(physicsWorld);

	particleSystem->setMaterial("DefaultParticleShader");
	particleSystem2->setMaterial(redDefault->getName());
/*
	particleSystem->GetPolicy()->setMaterial("DefaultParticleShader");
	particleSystem2->GetPolicy()->setMaterial(redDefault->getName());*/

	testNode = ogreSceneManager->getRootSceneNode()->createChildSceneNode();
	
	testNode->attachObject(particleSystem);
/*
	testNode->attachObject((DefaultParticlePolicy*)particleSystem->GetPolicy());*/

	testNode->translate(5.0f, -10.0f, 0.0f);

	testNode2 = ogreSceneManager->getRootSceneNode()->createChildSceneNode();

	testNode2->attachObject(particleSystem2);

	/*testNode2->attachObject((DefaultParticlePolicy*)particleSystem2->GetPolicy());*/
	testNode2->translate(-5.0f, 10.0f, 0.0f);

	particlePointEmitter->position = OgreToPhysXVec3(testNode->getPosition());
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
			particlePointEmitter->position = OgreToPhysXVec3(testNode->getPosition());
			particleSystem->Update(gameTime);
			particlePointEmitter->position = OgreToPhysXVec3(testNode2->getPosition());
			particleSystem2->Update(gameTime);
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
	if(arg.key == OIS::KC_I)
	{
		Ogre::Vector3 camPos = mainOgreCamera->getPosition();
		camPos.z += 40;
		mainOgreCamera->setPosition(camPos);
	}

	if(arg.key == OIS::KC_J)
	{
		Ogre::Vector3 camPos = mainOgreCamera->getPosition();
		camPos.z -= 40;
		mainOgreCamera->setPosition(camPos);
	}

	if(arg.key == OIS::KC_W)
	{
		testNode2->translate(0.0f, 1.0f, 0.0f);
	}

	if(arg.key == OIS::KC_S)
	{
		testNode2->translate(0.0f, -1.0f, 0.0f);
	}

	if(arg.key == OIS::KC_A)
	{
		testNode2->translate(-3.0f, 0.0f, 0.0f);
	}

	if(arg.key == OIS::KC_D)
	{
		testNode2->translate(3.0f, 0.0f, 0.0f);
	}

	if(arg.key == OIS::KC_UP)
	{
		testNode->translate(0.0f, 1.0f, 0.0f);
		particleSystem->GetEmitter()->position = OgreToPhysXVec3(testNode->getPosition());
	}

	if(arg.key == OIS::KC_DOWN)
	{
		testNode->translate(0.0f, -1.0f, 0.0f);
		particleSystem->GetEmitter()->position = OgreToPhysXVec3(testNode->getPosition());
	}

	if(arg.key == OIS::KC_LEFT)
	{
		testNode->translate(-3.0f, 0.0f, 0.0f);
		particleSystem->GetEmitter()->position = OgreToPhysXVec3(testNode->getPosition());
	}

	if(arg.key == OIS::KC_RIGHT)
	{
		testNode->translate(3.0f, 0.0f, 0.0f);
		particleSystem->GetEmitter()->position = OgreToPhysXVec3(testNode->getPosition());
	}
	

	return true;
}