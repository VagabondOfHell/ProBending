#include "ParticleComponent.h"
#include "ParticleSystemBase.h"
#include "AbstractParticleEmitter.h"
#include "OgreSceneNode.h"
#include "HelperFunctions.h"
#include "OgreSceneManager.h"
#include "IScene.h"
#include "GameObject.h"

Ogre::SceneNode* ParticleComponent::WORLD_PARTICLES_NODE = NULL;
int ParticleComponent::NUM_INSTANCES = 0;

ParticleComponent::ParticleComponent(GameObject* owningObject, ParticleSystemBase* _particleSystem, bool _useLocalSpace)
	:Component(owningObject), particleSystem(_particleSystem), useLocalSpace(_useLocalSpace)
{
	if(WORLD_PARTICLES_NODE == NULL)
	{
		WORLD_PARTICLES_NODE = owningObject->GetOwningScene()->GetSceneRootNode()->createChildSceneNode("WORLD_PARTICLE_NODE");
	}

	++NUM_INSTANCES;
}

ParticleComponent::~ParticleComponent(void)
{
	DestroySceneNode();

	if(particleSystem)
		delete particleSystem;

	--NUM_INSTANCES;

	//If our static is the only reference
	if(NUM_INSTANCES <= 0)
	{
		//Remove the node from the scene
		owningGameObject->GetOwningScene()->GetOgreSceneManager()->destroySceneNode(WORLD_PARTICLES_NODE);
		WORLD_PARTICLES_NODE = NULL;
	}
}

void ParticleComponent::CreateSceneNode()
{
	sceneNode = owningGameObject->gameObjectNode->createChildSceneNode(
		HelperFunctions::PhysXToOgreVec3(particleSystem->GetEmitter()->position));
	
	if(useLocalSpace)
		sceneNode->attachObject(particleSystem);
	else
		WORLD_PARTICLES_NODE->attachObject(particleSystem);

	sceneNode->setInheritScale(false);
}

void ParticleComponent::DestroySceneNode()
{
	if(sceneNode)
	{
		if(!useLocalSpace)
			WORLD_PARTICLES_NODE->detachObject(particleSystem);

		sceneNode->detachAllObjects();

		sceneNode->getParentSceneNode()->removeChild(sceneNode);
		
		owningGameObject->GetOwningScene()->GetOgreSceneManager()->destroySceneNode(sceneNode);

		sceneNode = NULL;
	}
}

void ParticleComponent::Start()
{
	CreateSceneNode();
	particleSystem->Initialize(owningGameObject->GetOwningScene()->GetPhysXScene());
}

void ParticleComponent::Update(float gameTime)
{
	if(useLocalSpace)
		particleSystem->GetEmitter()->position = particleSystem->GetEmitter()->position = physx::PxVec3(0.0f);// HelperFunctions::OgreToPhysXVec3(sceneNode->getPosition());
	else
		particleSystem->GetEmitter()->position = HelperFunctions::OgreToPhysXVec3(sceneNode->_getDerivedPosition());
	
	particleSystem->Update(gameTime);
}

void ParticleComponent::SetTransformationSpace(const bool _useLocalSpace)
{
	if(useLocalSpace == _useLocalSpace)
		return;

	//If switching from world to local
	if(_useLocalSpace)
	{
		owningGameObject->GetOwningScene()->GetSceneRootNode()->detachObject(particleSystem);
		sceneNode->attachObject(particleSystem);
	}
	else
	{
		sceneNode->detachObject(particleSystem);
		owningGameObject->GetOwningScene()->GetSceneRootNode()->attachObject(particleSystem);
	}

	useLocalSpace = _useLocalSpace;
}