#include "ParticleComponent.h"
#include "ParticleSystemBase.h"
#include "AbstractParticleEmitter.h"
#include "OgreSceneNode.h"
#include "HelperFunctions.h"
#include "OgreSceneManager.h"
#include "IScene.h"
#include "GameObject.h"

ParticleComponent::ParticleComponent(GameObject* owningObject, ParticleSystemBase* _particleSystem, 
									 AbstractParticleEmitter* _particleEmitter, bool _useLocalSpace)
	:Component(owningObject), particleSystem(_particleSystem), particleEmitter(_particleEmitter), useLocalSpace(_useLocalSpace)
{
}

ParticleComponent::~ParticleComponent(void)
{
	DestroySceneNode();

	if(particleSystem)
		delete particleSystem;
}

void ParticleComponent::CreateSceneNode()
{
	sceneNode = owningGameObject->gameObjectNode->createChildSceneNode(HelperFunctions::PhysXToOgreVec3(particleEmitter->position));
	
	if(useLocalSpace)
		sceneNode->attachObject(particleSystem);
	else
		owningGameObject->GetOwningScene()->GetSceneRootNode()->attachObject(particleSystem);

	sceneNode->setInheritScale(false);
}

void ParticleComponent::DestroySceneNode()
{
	if(sceneNode)
	{
		if(!useLocalSpace)
			owningGameObject->GetOwningScene()->GetSceneRootNode()->detachObject(particleSystem);

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
		particleEmitter->position = particleEmitter->position = physx::PxVec3(0.0f);// HelperFunctions::OgreToPhysXVec3(sceneNode->getPosition());
	else
		particleEmitter->position = HelperFunctions::OgreToPhysXVec3(sceneNode->_getDerivedPosition());
	
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