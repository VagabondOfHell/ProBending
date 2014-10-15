#pragma once
#include "OgreVector3.h"
#include "IScene.h"
#include "PxPhysics.h"

class GameObject
{
protected:


public:
	IScene* owningScene;
	
	Ogre::SceneNode* gameObjectNode;
	Ogre::Entity* entity;

	Ogre::ManualObject* physxDebugDraw;
	Ogre::SceneNode* debugNode;

	physx::PxRigidActor* rigidBody;
	physx::PxParticleBase* particleSystem;

	enum ComponentType
	{
		AUDIOCOMPONENT
	};

	GameObject(IScene* owningScene);

	virtual ~GameObject(void);

	virtual void Start();

	virtual bool Update(float gameTime);

	void AttachComponent(ComponentType componentType);
	
	///<summary>Loads a model for Ogre, providing a Try Catch to handle errors</summary>
	///<param "modelFileName">The name of the model with the extension to load from Ogre resources</param>
	///<returns>True if loaded, false if failed</returns>
	bool LoadModel(const Ogre::String& modelFileName);

	///<summary>Creates an outline of all physX shapes attached to the rigid body
	///CURRENTLY ONLY SUPPORTS CUBES</summary>
	void CreatePhysXDebug();

	///<summary>Destroys and deletes the Debug component of the physx rigidbody</summary>
	void DestroyPhysXDebug();

	///<summary>Provides efficient, inlined, type casting to a Dynamic Rigid Body</summary>
	///<returns>The dynamic rigid body representation of the rigid body, or NULL if failed</returns>
	inline physx::PxRigidDynamic* GetDynamicRigidBody()
	{
		if(rigidBody->isRigidDynamic())
			return (physx::PxRigidDynamic*)rigidBody;

		return NULL;
	}

	///<summary>Convenience function for converting an Ogre Vector3 to a physx Vector3</summary>
	///<param "ogreVector">The ogre vector to convert</param>
	///<returns>The physX vector3 representation</returns>
	inline physx::PxVec3 OgreToPhysXVec3(const Ogre::Vector3& ogreVector)const
	{
		return physx::PxVec3(ogreVector.x, ogreVector.y, ogreVector.z);
	}

	///<summary>Convenience function for converting a physx Vector3 to an Ogre Vector3</summary>
	///<param "physxVector">The physX vector to convert</param>
	///<returns>The ogre vector3 representation</returns>
	inline Ogre::Vector3 PhysXToOgreVec3(const physx::PxVec3& physxVector)const
	{
		return Ogre::Vector3(physxVector.x, physxVector.y, physxVector.z);
	}
};

