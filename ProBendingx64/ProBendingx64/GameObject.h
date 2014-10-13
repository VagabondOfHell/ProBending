#pragma once
#include "OgreVector3.h"
#include "IScene.h"

class Component;
class RenderComponent;

struct Transform
{
	Ogre::Vector3 position;
	Ogre::Vector3 scale;
	Ogre::Quaternion rotation;

	Transform(Ogre::Vector3 _position = Ogre::Vector3(0.0f), Ogre::Vector3 _scale = Ogre::Vector3(1.0f, 1.0f, 1.0f), 
		Ogre::Quaternion _rotation = Ogre::Quaternion::IDENTITY)
	{
		position = _position;
		scale = _scale;
		rotation = _rotation;
	}

	~Transform()
	{}
};

class GameObject
{
protected:


public:
	IScene* owningScene;
	Ogre::SceneNode* gameObjectNode;
	Ogre::Entity* entity;

	Transform transform;

	enum ComponentType
	{
		AUDIOCOMPONENT
	};

	GameObject(IScene* owningScene);
	virtual ~GameObject(void);

	void AttachComponent(ComponentType componentType);
	
	bool LoadModel(const Ogre::String& modelFileName);
};

