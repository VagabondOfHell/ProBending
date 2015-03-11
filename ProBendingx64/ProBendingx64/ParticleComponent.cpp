#include "ParticleComponent.h"
#include "ParticleSystemBase.h"
#include "AbstractParticleEmitter.h"
#include "OgreSceneNode.h"
#include "HelperFunctions.h"
#include "OgreSceneManager.h"
#include "IScene.h"
#include "GameObject.h"

#include "NotImplementedException.h"

Ogre::SceneNode* ParticleComponent::WORLD_PARTICLES_NODE = NULL;
int ParticleComponent::NUM_INSTANCES = 0;

ParticleComponent::ParticleComponent(FluidAndParticleBase* _particleSystem, bool _useLocalSpace)
	:Component(), particleSystem(_particleSystem), useLocalSpace(_useLocalSpace)
{
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
		if(WORLD_PARTICLES_NODE)
		{
			//Remove the node from the scene
			owningGameObject->GetOwningScene()->GetOgreSceneManager()->destroySceneNode(WORLD_PARTICLES_NODE);
			WORLD_PARTICLES_NODE = NULL;
			NUM_INSTANCES = 0;
		}
	}
}

void ParticleComponent::OnAttach()
{
	if(WORLD_PARTICLES_NODE == NULL)//Check if the static scene node has been initialized
	{
		WORLD_PARTICLES_NODE = owningGameObject->GetOwningScene()->
			GetSceneRootNode()->createChildSceneNode("WORLD_PARTICLE_NODE");
	}

	CreateSceneNode();

	particleSystem->Initialize(owningGameObject->GetOwningScene()->GetPhysXScene());

	ParticleSystemBase* cast = dynamic_cast<ParticleSystemBase*>(particleSystem);

	if(cast != NULL)
	{
		if(cast->GetParticleKernel() == NULL)
			cast->AssignAffectorKernel(cast->FindBestKernel());
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
}

void ParticleComponent::Update(float gameTime)
{
	if(enabled)
	{
		if(useLocalSpace)
			particleSystem->GetEmitter()->position = HelperFunctions::OgreToPhysXVec3(sceneNode->getPosition());
		else
			particleSystem->GetEmitter()->position = HelperFunctions::OgreToPhysXVec3(sceneNode->_getDerivedPosition());
	
		particleSystem->Update(gameTime);
	}
}

void ParticleComponent::SetTransformationSpace(const bool _useLocalSpace)
{
	if(useLocalSpace == _useLocalSpace)
		return;

	//If switching from world to local
	if(_useLocalSpace)
	{
		WORLD_PARTICLES_NODE->detachObject(particleSystem);
		sceneNode->attachObject(particleSystem);
	}
	else
	{
		sceneNode->detachObject(particleSystem);
		WORLD_PARTICLES_NODE->attachObject(particleSystem);
	}

	useLocalSpace = _useLocalSpace;
}

ParticleComponent* ParticleComponent::Clone(GameObject* gameObject)
{
	ParticleComponent* clone = new ParticleComponent(particleSystem->Clone(), useLocalSpace);
	
	return clone;
}

void ParticleComponent::Enable()
{
	if(!enabled)
	{
		particleSystem->EnableSimulation();
		enabled = true;
	}
}

void ParticleComponent::Disable()
{
	if(enabled)
	{
		particleSystem->DisableSimulation();
		enabled = false;
	}
	
}
