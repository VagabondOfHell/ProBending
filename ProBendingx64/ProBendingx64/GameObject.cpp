#include "GameObject.h"
#include "PhysXDebugDraw.h"
#include "OgreEntity.h"
#include "OgreManualObject.h"

unsigned int GameObject::InstanceCounter = 0;

GameObject::GameObject(IScene* _owningScene, std::string objectName)
	:owningScene(_owningScene), name(objectName), entity(NULL), physxDebugDraw(NULL), rigidBody(NULL)
{
	if(name.empty())
	{
		name = "GameObject" + std::to_string(InstanceCounter);
		++InstanceCounter;
	}

	gameObjectNode = owningScene->GetOgreSceneManager()->getRootSceneNode()->
		createChildSceneNode();
}


GameObject::~GameObject(void)
{
	//loop through and destroy all children
	auto childIterator = children.begin();
	while (childIterator != children.end())
	{
		delete childIterator->second;
		childIterator++;
	}

	//Delete all components attached to this object
	auto componentIter = components.begin();
	while (componentIter != components.end())
	{
		delete componentIter->second;
		componentIter++;
	}

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
		gameObjectNode->getParent()->removeChild(gameObjectNode);
		owningScene->GetOgreSceneManager()->destroySceneNode(gameObjectNode);
		gameObjectNode = NULL;
	}

	//destroy the physx rigid body
	if(rigidBody)
	{
		rigidBody->release();
		rigidBody = NULL;
	}
}

void GameObject::Start()
{
	//Children and Components have their start called upon Addition. 
	//Possibly add bool to children and components that check if start has been called 
	//and provide a Bool in attach and add that checks if they should call start when added?
}

void GameObject::Update(float gameTime)
{
	//If there is a rigid body, update our draw position
	if(rigidBody)
	{
		gameObjectNode->setPosition(HelperFunctions::PhysXToOgreVec3(rigidBody->getGlobalPose().p));

		if(debugNode)
			debugNode->setPosition(HelperFunctions::PhysXToOgreVec3(rigidBody->getGlobalPose().p));
	}

	//Update componets
	auto componentIter = components.begin();
	while (componentIter != components.end())
	{
		componentIter->second->Update(gameTime);
		++componentIter;
	}

	//Update children
	auto childIter = children.begin();
	while (childIter != children.end())
	{
		childIter->second->Update(gameTime);
		++childIter;
	}
}

bool GameObject::AddChild(GameObject* newChild)
{
	//Add the child
	auto iter = children.insert(std::pair<std::string, GameObject*>(newChild->name, newChild));

	if(iter.second)
		newChild->Start();

	//Return success
	return iter.second;
}

bool GameObject::RemoveChild(GameObject* childToRemove)
{
	//The caller has the object already, so check if child was successfully removed instead
	return RemoveChild(childToRemove->name) != NULL;
}

GameObject* GameObject::RemoveChild(std::string name)
{
	auto childIterator = children.find(name);

	if(childIterator != children.end())
	{
		GameObject* objectRemoved = childIterator->second;

		children.erase(childIterator);
		
		return objectRemoved;
	}

	return NULL;
}

bool GameObject::DeleteChild(GameObject* childToRemove)
{
	return DeleteChild(childToRemove->name);
}

bool GameObject::DeleteChild(std::string name)
{
	auto childIterator = children.find(name);

	if(childIterator != children.end())
	{
		GameObject* object = childIterator->second;
		children.erase(childIterator);
		delete object;
		return true;
	}

	return false;
}

void GameObject::AttachComponent(Component* newComponent)
{
	if(newComponent)
	{
		components.insert(std::pair<Component::ComponentType, Component*>(newComponent->GetComponentType(), newComponent));
		newComponent->Start();
	}
}

bool GameObject::LoadModel(const Ogre::String& modelFileName)
{
	if(entity)
		//Detach, modify, reattach
		gameObjectNode->detachObject(entity);
	///SHOULD OLD ENTITY BE DESTROYED HERE?
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

bool GameObject::ConstructBoxFromEntity(physx::PxBoxGeometry& boxGeometry)const
{
	if(entity)
	{
		//Get the half size of the entity bounding box and modify by scale factor
		Ogre::Vector3 boxHalfSize = entity->getBoundingBox().getHalfSize() * gameObjectNode->getScale();
		//Generate physx geometry
		boxGeometry = physx::PxBoxGeometry(physx::PxVec3(boxHalfSize.x, boxHalfSize.y, boxHalfSize.z));
		//indicate success
		return true;
	}

	return false;
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