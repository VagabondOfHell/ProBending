#include "FluidScene.h"
#include "SceneManager.h"
#include "InputNotifier.h"
#include "OgreEntity.h"
#include "OgreSceneManager.h"
#include "OgreRenderWindow.h"
#include "ParticlePointEmitter.h"
#include "ParticleComponent.h"
#include "Projectile.h"
#include "AbilityDescriptor.h"
#include "Probender.h"
#include "GUIManager.h"
#include "PxScene.h"

#include "ParticleSystemBase.h"

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
	InputNotifier::GetInstance()->RemoveObserver(this);

	if(particlePointEmitter)
		delete particlePointEmitter;

	if(probender)
		delete probender;

	if(gameObject)
		delete gameObject;

	if(projectile)
		delete projectile;

}

void FluidScene::Initialize()
{
	IScene::Initialize();
}

void FluidScene::Start()
{
	InitializePhysics(physx::PxVec3(0.0f, -9.8f, 0.0f), true);

	mainOgreCamera = ogreSceneManager->createCamera("MainCamera");

	owningManager->GetRenderWindow()->removeAllViewports();

	Ogre::Viewport* viewport = owningManager->GetRenderWindow()->addViewport(mainOgreCamera);
	viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));

	mainOgreCamera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

	mainOgreCamera->setPosition(0, 0, 40.50f);
	mainOgreCamera->lookAt(0, 0, 0);
	mainOgreCamera->setNearClipDistance(0.01);
	mainOgreCamera->setFarClipDistance(10000);
		
	InputNotifier::GetInstance()->AddObserver(guiManager);
	InputNotifier::GetInstance()->AddObserver(this);
	probender = new Probender();

	Ogre::Light* light = ogreSceneManager->createLight();
	light->setType(Ogre::Light::LightTypes::LT_DIRECTIONAL);
	light->setAttenuation(10000, 1.0, 1, 1);
	projectile = new Projectile(this, SharedAbilityDescriptor(new AbilityDescriptor(probender)));
	projectile->LoadModel("Rock_01.mesh");
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName("DefaultParticleShader");
	Ogre::GpuProgramParametersSharedPtr params = material->getTechnique(0)->getPass(0)->getVertexProgramParameters();
	Ogre::GpuProgramParametersSharedPtr fragParams = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	params->setNamedConstant("newcolor", Ogre::Vector4(1.0, 1.0, 1.0, 0.50));
	
	Ogre::Pass* pass = material->getTechnique(0)->getPass(0);
	//pass->setPointSpritesEnabled(true);
	pass->setPointAttenuation(true, 0, 1, 0);
	pass->setPointMaxSize(1.0f);
	pass->setPointMinSize(0.00f);

	Ogre::MaterialPtr redDefault = material->clone("RedDefaultParticleShader");
	params = redDefault->getTechnique(0)->getPass(0)->getVertexProgramParameters();
	params->setNamedConstant("newcolor", Ogre::Vector4(1.0f, 0.0f, 0.0f, 0.50f));
	
	particlePointEmitter = new ParticlePointEmitter(100, physx::PxVec3(65.0f, 0.0f, 0.0f),
		physx::PxVec3(-2, 1.0, 0).getNormalized(), physx::PxVec3(2, 1.0f, 1).getNormalized(), 20.0f, 40.0f);
	
	ParticleSystemParams psParams = ParticleSystemParams();
	psParams.cudaContext = cudaContextManager;
	psParams.useGravity = false;

	particleSystem = new ParticleSystemBase(particlePointEmitter, NUM_PARTICLES, 5.0f, psParams, false);
	particleSystem2 = new ParticleSystemBase(particlePointEmitter, NUM_PARTICLES, 2.0f, psParams, false);
	
	particleComponent = new ParticleComponent(projectile, particleSystem, false);
	projectile->AttachComponent(particleComponent);
	projectile->gameObjectNode->setScale(0.3f, 0.3f, 0.3f);

	particlePointEmitter->position = physx::PxVec3(45.0f, 0.0f, 0.0f);
	particleComponent2 = new ParticleComponent(projectile, particleSystem2, false);
	projectile->AttachComponent(particleComponent2);

	particleSystem->setMaterial("TexturedParticleShader");
	//particleSystem2->setMaterial(redDefault->getName());
	particleSystem2->setMaterial("DefaultParticleShader");
	
	projectile->Start();
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

			projectile->Update(gameTime);
		
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

	if(arg.key == OIS::KC_P)
	{
		mainOgreCamera->yaw(Ogre::Radian(Ogre::Degree(10)));
	}

	if(arg.key == OIS::KC_O)
	{
		mainOgreCamera->yaw(Ogre::Radian(Ogre::Degree(-10)));
	}

	if(arg.key == OIS::KC_J)
	{
		Ogre::Vector3 camPos = mainOgreCamera->getPosition();
		camPos.z -= 40;
		mainOgreCamera->setPosition(camPos);
	}

	if(arg.key == OIS::KC_W)
	{
		projectile->gameObjectNode->translate(0.0f, 1.0f, 0.0f);
	}

	if(arg.key == OIS::KC_S)
	{
		projectile->gameObjectNode->translate(0.0f, -1.0f, 0.0f);
	}

	if(arg.key == OIS::KC_A)
	{
		projectile->gameObjectNode->translate(-3.0f, 0.0f, 0.0f);
	}

	if(arg.key == OIS::KC_D)
	{
		projectile->gameObjectNode->translate(3.0f, 0.0f, 0.0f);
	}

	if(arg.key == OIS::KC_UP)
	{
		projectile->gameObjectNode->translate(0.0f, 1.0f, 0.0f);
	}

	if(arg.key == OIS::KC_DOWN)
	{
		projectile->gameObjectNode->translate(0.0f, -1.0f, 0.0f);
	}

	if(arg.key == OIS::KC_LEFT)
	{
		projectile->gameObjectNode->translate(-3.0f, 0.0f, 0.0f);
	}

	if(arg.key == OIS::KC_RIGHT)
	{
		projectile->gameObjectNode->translate(3.0f, 0.0f, 0.0f);
	}
	
	if(arg.key == OIS::KC_SPACE)
	{
		particleComponent2->SetTransformationSpace(!particleComponent2->GetTransformationSpace());
	}

	return true;
}