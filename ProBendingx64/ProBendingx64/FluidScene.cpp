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
	
	ogreSceneManager->setSkyBox(true, "Examples/SceneCubeMap2" );
	
	sunParticle = ogreSceneManager->createParticleSystem("Sun", "Space/Sun");
	particleNode = ogreSceneManager->getRootSceneNode()->createChildSceneNode("Particle");
	particleNode->translate(5.0f, 0.0f, 0.0f);
	particleNode->attachObject(sunParticle);

	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName("SkyChange");
	Ogre::GpuProgramParametersSharedPtr params = material->getTechnique(0)->getPass(0)->getVertexProgramParameters();
	params->setNamedConstant("newcolor", Ogre::Vector4(0.0, 0.0, 1.0, 1.0));
	
	//ogreSceneManager->setSkyBox(true, "SkyChange");

	object = new GameObject(this);

	object->LoadModel("sinbad.mesh");

	//object->entity->setMaterialName("BlueShader");

	physx::PxParticleSystem* ps = PxGetPhysics().createParticleSystem(NUM_PARTICLES, false);

	if(ps)
		object->particleSystem = ps;

	for (physx::PxU32 i = 0; i < NUM_PARTICLES; i++)
	{
		particleIndices[i] = i;
		particlePositions[i] = physx::PxVec3(10, -10.0f, 0);
		particleVelocities[i] = physx::PxVec3(0.0f, 0.0f, 0.0f);
	}

	physx::PxParticleCreationData data = physx::PxParticleCreationData();
	data.numParticles = NUM_PARTICLES;
	data.indexBuffer = physx::PxStrideIterator<physx::PxU32>(particleIndices);
	data.positionBuffer = physx::PxStrideIterator<physx::PxVec3>(particlePositions);
	data.velocityBuffer = physx::PxStrideIterator<physx::PxVec3>(particleVelocities);
	
	ps->setParticleBaseFlag(physx::PxParticleBaseFlag::eGPU, true);
	
	physx::PxParticleBaseFlags flags = ps->getParticleBaseFlags();
	
	if(flags & physx::PxParticleBaseFlag::eGPU)
		printf("On GPU");

	ps->createParticles(data);
	ps->setParticleReadDataFlag(physx::PxParticleReadDataFlag::eVELOCITY_BUFFER, true);
	
	ps->setMaxMotionDistance(10.0f);
	physicsWorld->addActor(*object->particleSystem);

	for (int i = 0; i < NUM_PARTICLES; i++)
	{
		particleVelocities[i] = physx::PxVec3(10.0f, 0.0f, 0.0f);
	}

	object->particleSystem->addForces(NUM_PARTICLES, physx::PxStrideIterator<physx::PxU32>(particleIndices),
		physx::PxStrideIterator<physx::PxVec3>(particleVelocities), physx::PxForceMode::eFORCE);


}

bool FluidScene::Update(float gameTime)
{
	
	object->particleSystem->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);

	
	physicsWorld->simulate(gameTime);

	

	physicsWorld->fetchResults(true);
	
	object->Update(gameTime);

	physx::PxParticleReadData* rd = object->particleSystem->lockParticleReadData(physx::PxDataAccessFlag::eREADABLE);
	
	if(rd)
	{
		physx::PxStrideIterator<const physx::PxParticleFlags> flagsIter(rd->flagsBuffer);
		physx::PxStrideIterator<const physx::PxVec3> positionIter (rd->positionBuffer);
		physx::PxStrideIterator<const physx::PxVec3> velocityIter (rd->velocityBuffer);
		
		Ogre::ParticleIterator particleIter = sunParticle->_getIterator();

		for (unsigned int i = 0; i < rd->validParticleRange; ++i, ++flagsIter, ++velocityIter, ++positionIter)
		{
			if(*flagsIter & physx::PxParticleFlag::eVALID)
			{
				const physx::PxVec3& position = *positionIter;
				const physx::PxVec3& velocity = *velocityIter;

				//particlePositions[i] = position;
				//particleVelocities[i] = velocity;

				if(i == 0)
				if(position.magnitudeSquared() > 100)
				{
					object->particleSystem->setPositions(1, physx::PxStrideIterator<const physx::PxU32>(&i), 
						physx::PxStrideIterator<const physx::PxVec3>(&physx::PxVec3(0.0f, 0.0f, 0.0f)));
				}

				if(!particleIter.end())
				{
					Ogre::Particle* current = particleIter.getNext();
					current->position = object->PhysXToOgreVec3(position);
				}
				else
					break;
			}
		}

		rd->unlock();
	}

	return true;
}

void FluidScene::Close()
{

}