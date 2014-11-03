#include "FluidScene.h"
#include "SceneManager.h"
#include "InputNotifier.h"
#include "OgreEntity.h"
#include "OgreParticleAffector.h"
#include "OgreParticle.h"
#include "OgreParticleEmitter.h"

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

	if(object)
	{
		delete object;
		object = NULL;
	}

	if(particleRenderer)
	{
		delete particleRenderer;
		particleRenderer = NULL;
	}
}

void FluidScene::Start()
{
	InitializePhysics(physx::PxVec3(0.0f, -9.8f, 0.0f), true);

	mainOgreCamera = ogreSceneManager->createCamera("MainCamera");

	owningManager->GetRenderWindow()->removeAllViewports();

	Ogre::Viewport* viewport = owningManager->GetRenderWindow()->addViewport(mainOgreCamera);
	viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));

	mainOgreCamera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

	mainOgreCamera->setPosition(0, 0, 180.0f);
	mainOgreCamera->lookAt(0, 0, 0);
	mainOgreCamera->setNearClipDistance(5);
	mainOgreCamera->setFarClipDistance(10000);
		
	InputNotifier::GetInstance()->AddObserver(guiManager);

	Ogre::Light* light = ogreSceneManager->createLight();
	light->setType(Ogre::Light::LightTypes::LT_DIRECTIONAL);
	light->setAttenuation(10000, 1.0, 1, 1);
	
	//ogreSceneManager->setSkyBox(true, "Examples/SceneCubeMap2" );

	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName("SkyChange");
	Ogre::GpuProgramParametersSharedPtr params = material->getTechnique(0)->getPass(0)->getVertexProgramParameters();
	params->setNamedConstant("newcolor", Ogre::Vector4(0.0, 0.0, 1.0, 1.0));
	
	//ogreSceneManager->setSkyBox(true, "SkyChange");

	object = new GameObject(this);

	object->LoadModel("sinbad.mesh");

	//object->entity->setMaterialName("BlueShader");

	for (physx::PxU32 i = 0; i < NUM_PARTICLES; i++)
	{
		particleIndices[i] = i;
		particlePositions[i] = physx::PxVec3(1 + i * 0.2f, 0.0f, 0);
		particleVelocities[i] = physx::PxVec3(0.0f, 0.0f, 0.0f);
	}

	particleRenderer = new ParticleRenderer(this, NUM_PARTICLES);

	particleRenderer->Resize(NUM_PARTICLES);

	particleRenderer->setMaterial("SkyChange");

	particleRenderer->UpdateParticles(particlePositions, NUM_PARTICLES);

	testNode = ogreSceneManager->getRootSceneNode()->createChildSceneNode();

	testNode->attachObject(particleRenderer);

	testNode->translate(5.0f, 0.0f, 0.0f);
}

bool FluidScene::Update(float gameTime)
{	
	physicsWorld->simulate(gameTime);

	

	physicsWorld->fetchResults(true);

	particleRenderer->Update(gameTime);
	/*for (int i = 0; i < NUM_PARTICLES; i++)
	{
		particleVelocities[i] = physx::PxVec3(0.0f, 0.2f, 0.0f);
		particlePositions[i] += particleVelocities[i];
	}

	particleRenderer->UpdateParticles(particlePositions, NUM_PARTICLES);*/

	object->Update(gameTime);


	return true;
}

void FluidScene::Close()
{

}