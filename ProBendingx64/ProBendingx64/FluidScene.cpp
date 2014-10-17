#include "FluidScene.h"
#include "SceneManager.h"
#include "InputNotifier.h"
#include "OgreEntity.h"
#include "OgreParticleAffector.h"
#include "OgreParticle.h"

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
}

void FluidScene::Start()
{
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
	
	ogreSceneManager->setSkyBox(true, "Examples/SpaceSkyBox" );
	
	sunParticle = ogreSceneManager->createParticleSystem("Sun", "Space/Sun");
	particleNode = ogreSceneManager->getRootSceneNode()->createChildSceneNode("Particle");
	particleNode->translate(5.0f, 0.0f, 0.0f);
	particleNode->attachObject(sunParticle);

	ogreSceneManager->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_NONE);
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName("SkyChange");
	Ogre::GpuProgramParametersSharedPtr params = material->getTechnique(0)->getPass(0)->getVertexProgramParameters();
	//params->setNamedConstant("newcolor", Ogre::Vector4(0.7, 0.0, 0.0, 0.5));
	
	ogreSceneManager->setSkyBox(true, "SkyChange");

	object = new GameObject(this);

	object->LoadModel("sinbad.mesh");

	object->entity->setMaterialName("BlueShader");

}

bool FluidScene::Update(float gameTime)
{
	for (int i = 0; i < sunParticle->getNumParticles(); i++)
	{
		Ogre::Particle* current = sunParticle->getParticle(i);
		current->position = Ogre::Vector3(5.0f, -5.0f, 0.0f);
		
	}

	return true;
}

void FluidScene::Close()
{

}