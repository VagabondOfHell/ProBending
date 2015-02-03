#pragma once
#include "OgreVector3.h"
#include "HelperFunctions.h"
#include <map>
#include <unordered_set>

#include "Component.h"

#include <memory>

namespace physx
{
	class PxRigidActor;
	class PxRigidDynamic;
	class PxRigidStatic;
};

class IScene;

typedef std::shared_ptr<GameObject> SharedGameObject;
class RigidBodyComponent;

class GameObject
{
	friend class SceneSerializer;

protected:
	std::string name;

	IScene* owningScene;
	
	std::unordered_set<SharedGameObject> children;
	std::multimap<Component::ComponentType, Component*> components; //Multimap of components based on type
	RigidBodyComponent* rigidBody;

	bool started;

public:

	Ogre::SceneNode* gameObjectNode;//The node representing the game object

	GameObject(IScene* owningScene, std::string objectName = "");

	virtual ~GameObject(void);

	virtual void Start();

	virtual void Update(float gameTime);

	///<summary>At the moment this is used to differentiate between standard Game Objects and Projectiles and Probenders
	///Eventually may move the serialization into Game Objects, but at the moment this is easier, since Projectiles
	///and Probenders don't need serialization</summary>
	///<returns>True if serializable, false if not</returns>
	virtual inline bool IsSerializable()const{return true;}

	///<summary>Adds a new child to this game object and if successful calls Start</summary>
	///<param name="newChild">The new child to be attached</param>
	///<returns>True if successful, false if the name already exists</returns>
	bool AddChild(SharedGameObject newChild);

	///<summary>Removes a child from this game object</summary>
	///<param name="childToRemove">The child game object to be removed</param>
	///<returns>True if successful, false if not</returns>
	bool RemoveChild(SharedGameObject childToRemove);

	///<summary>Removes a child from this game object by name</summary>
	///<param name="name">The name of the child to be removed</param>
	///<returns>The first game object found that was removed, or NULL if not found</returns>
	SharedGameObject RemoveChild(std::string name);

#pragma region Transform Getter and Setters

	void SetWorldTransform(const Ogre::Vector3& pos, const Ogre::Quaternion& rot, const Ogre::Vector3& scale);

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

	bool GetInheritOrientation()const;

	void SetWorldOrientation(const Ogre::Quaternion& newOrientation);

	void SetWorldOrientation(const float w, const float x, const float y, const float z);

	void SetInheritOrientation(const bool val);

	Ogre::Vector3 GetLocalScale()const;

	void SetScale(const Ogre::Vector3& newScale);

	void SetScale(const float x, const float y, const float z);

	Ogre::Vector3 GetWorldScale()const;

	bool GetInheritScale()const;

	///<summary>True to inherit scale from parents</summary>
	///<param name="val">True to inherit, false to not</param>
	void SetInheritScale(const bool val);

#pragma endregion

	///<summary>Attaches a new component to the GameObject and calls its Start method</summary>
	///<param name="newComponent">The new component that will be attached</param>
	void AttachComponent(Component* newComponent);

	///<summary>Gets the name associated with the game object</summary>
	inline const std::string& GetName()const {return name;}

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

	SharedGameObject Clone();
};