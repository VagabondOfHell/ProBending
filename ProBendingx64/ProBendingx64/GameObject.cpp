#include "GameObject.h"
#include "OgreEntity.h"
#include "OgreManualObject.h"
#include "IScene.h"
#include "OgreSceneManager.h"
#include "PxRigidActor.h"
#include "PxRigidDynamic.h"
#include "PxRigidStatic.h"

unsigned int GameObject::InstanceCounter = 0;

GameObject::GameObject(IScene* _owningScene, std::string objectName)
	:owningScene(_owningScene), name(objectName), started(false)
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

	//Destroy the scene node
	if(gameObjectNode)
	{
		gameObjectNode->getParent()->removeChild(gameObjectNode);
		owningScene->GetOgreSceneManager()->destroySceneNode(gameObjectNode);
		gameObjectNode = NULL;
	}
}

void GameObject::Start()
{
	//Children and Components have their start called upon Addition. 
	auto componentIter = components.begin();
	while (componentIter != components.end())
	{
		componentIter->second->Start();
		++componentIter;
	}

	auto childIter = children.begin();
	while (childIter != children.end())
	{
		childIter->second->Start();
		++childIter;
	}

	started = true;
}

void GameObject::Update(float gameTime)
{
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

	if(started && iter.second) //if game object start has already been called, call start now
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
		components.insert(std::pair<Component::ComponentType, Component*>(
			newComponent->GetComponentType(), newComponent));
		newComponent->AttachToObject(this);
		
		if(started)//if game object was already started, start component here
			newComponent->Start();
	}
}

#pragma region Transform Getters and Setters

Ogre::Vector3 GameObject::GetLocalPosition() const
{
	return gameObjectNode->getPosition();
}

Ogre::Vector3 GameObject::GetWorldPosition() const
{
	return gameObjectNode->_getDerivedPosition();
}

void GameObject::SetLocalPosition(const Ogre::Vector3& newPos)
{
	gameObjectNode->setPosition(newPos);
}

void GameObject::SetLocalPosition(const float x, const float y, const float z)
{
	gameObjectNode->setPosition(x, y, z);
}

void GameObject::SetWorldPosition(const Ogre::Vector3& newPos)
{
	gameObjectNode->_setDerivedPosition(newPos);
}

void GameObject::SetWorldPosition(const float x, const float y, const float z)
{
	gameObjectNode->_setDerivedPosition(Ogre::Vector3(x, y, z));
}

Ogre::Quaternion GameObject::GetLocalOrientation() const
{
	return gameObjectNode->getOrientation();
}

void GameObject::SetLocalOrientation(const Ogre::Quaternion& newOrientation)
{
	gameObjectNode->setOrientation(newOrientation);
}

void GameObject::SetLocalOrientation(const float w, const float x, const float y, const float z)
{
	gameObjectNode->setOrientation(w, x, y, z);
}

Ogre::Quaternion GameObject::GetWorldOrientation() const
{
	return gameObjectNode->_getDerivedOrientation();
}

void GameObject::SetWorldOrientation(const Ogre::Quaternion& newOrientation)
{
	gameObjectNode->_setDerivedOrientation(newOrientation);
}

void GameObject::SetWorldOrientation(const float w, const float x, const float y, const float z)
{
	gameObjectNode->_setDerivedOrientation(Ogre::Quaternion(w, x, y, z));
}

void GameObject::SetInheritOrientation(const bool val)
{
	gameObjectNode->setInheritOrientation(val);
}

Ogre::Vector3 GameObject::GetLocalScale() const
{
	return gameObjectNode->getScale();
}

void GameObject::SetScale(const Ogre::Vector3& newScale)
{
	gameObjectNode->setScale(newScale);
}

void GameObject::SetScale(const float x, const float y, const float z)
{
	gameObjectNode->setScale(x, y, z);
}

Ogre::Vector3 GameObject::GetWorldScale() const
{
	return gameObjectNode->_getDerivedScale();
}

void GameObject::SetInheritScale(const bool val)
{
	gameObjectNode->setInheritScale(val);
}

#pragma endregion
