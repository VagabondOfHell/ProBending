#pragma once
#include "OgreVector3.h"
#include "IScene.h"
#include "PxPhysics.h"
#include "HelperFunctions.h"
#include <map>
#include "Component.h"

class GameObject
{
	static unsigned int InstanceCounter;

protected:
	std::string name;

	IScene* owningScene;
	Ogre::Entity* entity;
	
	std::map<std::string, GameObject*> children;
	std::multimap<Component::ComponentType, Component*> components; //Multimap of components based on type

	Ogre::ManualObject* physxDebugDraw;
	Ogre::SceneNode* debugNode;
public:
	Ogre::SceneNode* gameObjectNode;
	physx::PxRigidActor* rigidBody;
	
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

	///<summary>Attaches a new component to the GameObject and calls its Start method</summary>
	///<param name="newComponent">The new component that will be attached</param>
	void AttachComponent(Component* newComponent);
	
	///<summary>Loads a model for Ogre, providing a Try Catch to handle errors</summary>
	///<param name="modelFileName">The name of the model with the extension to load from Ogre resources</param>
	///<returns>True if loaded, false if failed</returns>
	bool LoadModel(const Ogre::String& modelFileName);

	///<summary>Creates a box from the ogre entity dimensions, with scaling</summary>
	///<param name="boxGeometry">The out value to be filled</param>
	///<returns>True if successful, false if not. Unsuccessful when entity has not been set</returns>
	bool ConstructBoxFromEntity(physx::PxBoxGeometry& boxGeometry)const;

	///<summary>Creates an outline of all physX shapes attached to the rigid body
	///CURRENTLY ONLY SUPPORTS CUBES</summary>
	void CreatePhysXDebug();

	///<summary>Destroys and deletes the Debug component of the physx rigidbody</summary>
	void DestroyPhysXDebug();

	///<summary>Provides efficient, inlined, type casting to a Dynamic Rigid Body</summary>
	///<returns>The dynamic rigid body representation of the rigid body, or NULL if failed</returns>
	inline physx::PxRigidDynamic* GetDynamicRigidBody()const
	{
		if(rigidBody->isRigidDynamic())
			return (physx::PxRigidDynamic*)rigidBody;

		return NULL;
	}

	///<summary>Provides efficient, inlined, type casting to a Static Rigid Body</summary>
	///<returns>The static rigid body representation of the rigid body, or NULL if failed</returns>
	inline physx::PxRigidStatic* GetStaticRigidBody()const
	{
		if(rigidBody->isRigidStatic())
			return (physx::PxRigidStatic*)rigidBody;

		return NULL;
	}

	///<summary>Gets the name associated with the game object</summary>
	inline std::string GetName()const {return name;}

	///<summary>Gets the scene this object was created on</summary>
	inline IScene* const GetOwningScene()const{return owningScene;}
};

