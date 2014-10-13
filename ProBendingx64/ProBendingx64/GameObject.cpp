#include "GameObject.h"
#include "OgreEntity.h"

GameObject::GameObject(IScene* _owningScene)
{
	owningScene = _owningScene;
	gameObjectNode = owningScene->GetOgreSceneManager()->getRootSceneNode()->
		createChildSceneNode(transform.position, transform.rotation);
	gameObjectNode->setScale(transform.scale);

	entity = NULL;
}


GameObject::~GameObject(void)
{
	
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