#include "GameObject.h"
#include "PhysXDebugDraw.h"
#include "OgreEntity.h"
#include "OgreManualObject.h"

GameObject::GameObject(IScene* _owningScene)
{
	owningScene = _owningScene;
	gameObjectNode = owningScene->GetOgreSceneManager()->getRootSceneNode()->
		createChildSceneNode();
	
	entity = NULL;
	physxDebugDraw = NULL;
	rigidBody = NULL;
	particleSystem = NULL;
}


GameObject::~GameObject(void)
{
	//Detach and destroy any entities
	if(entity)
	{
		gameObjectNode->detachObject(entity);
		owningScene->GetOgreSceneManager()->destroyEntity(entity);
		entity = NULL;
	}

	//Destroy any physx debugging
	DestroyPhysXDebug();

	//Destroy the scene node
	if(gameObjectNode)
	{
		owningScene->GetOgreSceneManager()->destroySceneNode(gameObjectNode);
		gameObjectNode = NULL;
	}

	//destroy the physx rigid body
	if(rigidBody)
	{
		rigidBody->release();
		rigidBody = NULL;
	}

	//Destroy the particle system
	if(particleSystem)
	{
		particleSystem->release();
		particleSystem = NULL;
	}
}

void GameObject::Start()
{
	
}

bool GameObject::Update(float gameTime)
{
	//If there is a rigid body, update our draw position
	if(rigidBody)
	{
		gameObjectNode->setPosition(PhysXToOgreVec3(rigidBody->getGlobalPose().p));

		if(debugNode)
			debugNode->setPosition(PhysXToOgreVec3(rigidBody->getGlobalPose().p));
	}

	return true;
}

void GameObject::AttachComponent(ComponentType componentType)
{
	switch (componentType)
	{
	case GameObject::AUDIOCOMPONENT:
		break;

	default:
		break;
	}
}

bool GameObject::LoadModel(const Ogre::String& modelFileName)
{
	if(entity)
		//Detach, modify, reattach
		gameObjectNode->detachObject(entity);
	
	try
	{
		entity = owningScene->GetOgreSceneManager()->createEntity(modelFileName);
	}
	catch(Ogre::Exception e)
	{
		entity = NULL;
		return false;
	}

	gameObjectNode->attachObject(entity);

	return true;
}

void GameObject::CreatePhysXDebug()
{
	if(!physxDebugDraw && rigidBody)
	{
		// create ManualObject
		physxDebugDraw = owningScene->GetOgreSceneManager()->createManualObject();
 
		int numShapes = rigidBody->getNbShapes();

		physx::PxShape** shapes = new physx::PxShape*[numShapes];

		rigidBody->getShapes(shapes, numShapes);

		physx::PxBoxGeometry boxGeometry;
		
		for (int i = 0; i < numShapes; i++)
		{
			switch (shapes[i]->getGeometryType())
			{
			case::physx::PxGeometryType::eBOX:
				shapes[i]->getBoxGeometry(boxGeometry);

				PhysXDebugDraw::DrawBoxGeometry(rigidBody->getGlobalPose().p + shapes[i]->getLocalPose().p, &boxGeometry, physxDebugDraw);
				break;

			default:
				break;
			}

		}
 
		if(shapes)
			delete shapes;

		debugNode = owningScene->GetOgreSceneManager()->getRootSceneNode()->createChildSceneNode();
		// add ManualObject to the node so it will be visible
		debugNode->attachObject(physxDebugDraw);
	}
}

void GameObject::DestroyPhysXDebug()
{
	if(physxDebugDraw)
	{
		debugNode->detachObject(physxDebugDraw);
		owningScene->GetOgreSceneManager()->destroyManualObject(physxDebugDraw);
		owningScene->GetOgreSceneManager()->destroySceneNode(debugNode);
		physxDebugDraw = NULL;
	}
}