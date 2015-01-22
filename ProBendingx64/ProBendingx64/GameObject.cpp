#include "GameObject.h"
#include "OgreEntity.h"
#include "OgreManualObject.h"
#include "IScene.h"
#include "OgreSceneManager.h"
#include "PxRigidActor.h"
#include "PxRigidDynamic.h"
#include "PxRigidStatic.h"

GameObject::GameObject(IScene* _owningScene, std::string objectName)
	:owningScene(_owningScene), name(objectName), started(false)
{
	gameObjectNode = owningScene->GetOgreSceneManager()->getRootSceneNode()->
		createChildSceneNode();
}


GameObject::~GameObject(void)
{
	//loop through and destroy all children
	/*auto childIterator = children.begin();
	while (childIterator != children.end())
	{
		childIterator->reset();
		childIterator++;
	}*/
	children.clear();

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
		childIter->get()->Start();
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
		childIter->get()->Update(gameTime);
		++childIter;
	}
}

bool GameObject::AddChild(SharedGameObject newChild)
{
	//Add the child
	auto iter = children.insert(newChild);

	if(started && iter.second) //if game object start has already been called, call start now
		newChild->Start();

	//Return success
	return iter.second;
}

bool GameObject::RemoveChild(SharedGameObject childToRemove)
{
	//Try to erase. 1 for success, 0 for failure
	return children.erase(childToRemove) > 0;
}

SharedGameObject GameObject::RemoveChild(std::string name)
{
	//lambda comparison to find by name
	auto childIterator = std::find_if(children.begin(), children.end(), 
		[&name](const SharedGameObject object){return name == object->name;});

	if(childIterator != children.end())
	{
		SharedGameObject objectRemoved = *childIterator;
		children.erase(childIterator);
		return objectRemoved;
	}

	return NULL;
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
