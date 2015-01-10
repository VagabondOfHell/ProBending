#pragma once
#include "OgreVector3.h"
#include "HelperFunctions.h"
#include <map>
#include "Component.h"

namespace physx
{
	class PxRigidActor;
	class PxRigidDynamic;
	class PxRigidStatic;
};

class IScene;

class GameObject
{
	static unsigned int InstanceCounter;

protected:
	std::string name;

	IScene* owningScene;
	
	std::map<std::string, GameObject*> children;
	std::multimap<Component::ComponentType, Component*> components; //Multimap of components based on type

	bool started;

public:

	Ogre::SceneNode* gameObjectNode;//The node representing the game object

	GameObject(IScene* owningScene, std::string objectName = "");

	virtual ~GameObject(void);

	virtual void Start();

	virtual void Update(float gameTime);

	///<summary>Adds a new child to this game object and if successful calls Start</summary>
	///<param name="newChild">The new child to be attached</param>
	///<returns>True if successful, false if the name already exists</returns>
	bool AddChild(GameObject* newChild);

	///<summary>Removes a child from this game object (DOES NOT DELETE)</summary>
	///<param name="childToRemove">The child game object to be removed</param>
	///<returns>True if successful, false if not</returns>
	bool RemoveChild(GameObject* childToRemove);

	///<summary>Removes a child from this game object by name (DOES NOT DELETE)</summary>
	///<param name="name">The name of the child to be removed</param>
	///<returns>The game object that was removed</returns>
	GameObject* RemoveChild(std::string name);

	///<summary>Deletes a child from this game object </summary>
	///<param name="childToDelete">The child to delete</param>
	///<returns>True if successful, false if not</returns>
	bool DeleteChild(GameObject* childToDelete);

	///<summary>Deletes a child from this game object </summary>
	///<param name="name">The name of the child to delete</param>
	///<returns>True if successful, false if not</returns>
	bool DeleteChild(std::string name);

#pragma region Transform Getter and Setters

	Ogre::Vector3 GetLocalPosition()const;

	Ogre::Vector3 GetWorldPosition()const;

	void SetLocalPosition(const Ogre::Vector3& newPos);

	void SetLocalPosition(const float x, const float y, const float z);

	void SetWorldPosition(const Ogre::Vector3& newPos);

	void SetWorldPosition(const float x, const float y, const float z);

	Ogre::Quaternion GetLocalOrientation()const;

	void SetLocalOrientation(const Ogre::Quaternion& newOrientation);

	void SetLocalOrientation(const float w, const float x, const float y, const float z);

	Ogre::Quaternion GetWorldOrientation()const;

	void SetWorldOrientation(const Ogre::Quaternion& newOrientation);

	void SetWorldOrientation(const float w, const float x, const float y, const float z);

	void SetInheritOrientation(const bool val);

	Ogre::Vector3 GetLocalScale()const;

	void SetScale(const Ogre::Vector3& newScale);

	void SetScale(const float x, const float y, const float z);

	Ogre::Vector3 GetWorldScale()const;

	///<summary>True to inherit scale from parents</summary>
	///<param name="val">True to inherit, false to not</param>
	void SetInheritScale(const bool val);

#pragma endregion

	///<summary>Attaches a new component to the GameObject and calls its Start method</summary>
	///<param name="newComponent">The new component that will be attached</param>
	void AttachComponent(Component* newComponent);

	///<summary>Gets the name associated with the game object</summary>
	inline std::string GetName()const {return name;}

	///<summary>Gets the scene this object was created on</summary>
	inline IScene* const GetOwningScene()const{return owningScene;}

	inline Component* GetComponent(Component::ComponentType typeToGet)
	{
		auto result = components.find(typeToGet);

		if(result != components.end())
			return result->second;
		else
			return NULL;
	}
};